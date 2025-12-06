//
//  IOSInspector.m
//  IOSInspector
//
//  Created by Paker on 29/10/19.
//

#import <UIKit/UIKit.h>
#import "IOSInspector.h"
#import "Inspector.h"

// MARK: - Database Adapter

class DatabaseProviderAdapter : public DatabaseProvider {
    __weak id<IOSInspectorProtocol> _Nonnull delegate;

public:
    DatabaseProviderAdapter(id<IOSInspectorProtocol> _Nonnull delegate): delegate(delegate) {}

protected:
    std::vector<std::string> databasePathList() const override {
        auto list = std::vector<std::string>();
        auto paths = delegate.databaseList;
        for (NSString *path in paths) {
            list.push_back(path.UTF8String);
        }
        return list;
    }
};

// MARK: - Utilities

static inline std::string stringFromData(NSData *str) {
    return std::string((const char*)str.bytes, str.length);
}

static inline NSString* NS(const std::string &str) {
    return [NSString stringWithUTF8String:str.c_str()];
}

static inline NSDictionary<NSString *, NSString *>* NS(const Params &params) {
    auto dict = [NSMutableDictionary<NSString *, NSString *> dictionaryWithCapacity:params.size()];
    for (const auto &p : params) {
        dict[NS(p.first)] = NS(p.second);
    }
    return dict;
}

static std::string buildHeaders(NSDictionary<NSString *,NSString *> *headers) {
    std::string headerstr;
    NSArray<NSString *> *sorted_headers = [[headers allKeys] sortedArrayUsingSelector:@selector(localizedCaseInsensitiveCompare:)];
    for (NSString *_key in sorted_headers) {
        NSString *key = _key;
        if ([key hasPrefix:@"_"]) {
            key = [key substringFromIndex: 1];
        }
        headerstr += std::string() + key.UTF8String + ": " + headers[_key].UTF8String + "\n";
    }
    return headerstr;
}

// MARK: - Implementation

static std::unique_ptr<Inspector> inspector; // singleton

@implementation IOSInspector

// MARK: - Initializer

+ (void)initializeWithDelegate:(nonnull id <IOSInspectorProtocol>)delegate {
    [self initializeWithDelegate:delegate port:30000];
}

+ (void)initializeWithDelegate:(nonnull id <IOSInspectorProtocol>)delegate port:(int)port {
    UIDevice *device = [UIDevice currentDevice];
    NSBundle *bundle = [NSBundle mainBundle];
    NSString *version = bundle.infoDictionary[@"CFBundleShortVersionString"];
    DeviceInfo info = {"ios", device.name.UTF8String, bundle.bundleIdentifier.UTF8String, version.UTF8String};
    inspector = std::make_unique<Inspector>(std::make_shared<DatabaseProviderAdapter>(delegate), info);
    inspector->bind(port);
}

// MARK: - Database
+ (void)createDatabase:(nonnull NSString *)path {
    Database(path.UTF8String, @"".UTF8String, 0, true);
}

+ (void)createDatabase:(nonnull NSString *)path password:(nonnull NSString *)password version:(int)version {
    Database(path.UTF8String, password.UTF8String, version, true);
}

+ (void)setCipherKey:(nonnull NSString *)database password:(nonnull NSString *)password version:(int)version {
    inspector->setCipherKey(database.UTF8String, password.UTF8String, version);
}

// MARK: - Network

+ (void)sendRequestWithUID:(nonnull NSString *)uid request:(nonnull NSURLRequest *)request {

    NSMutableDictionary<NSString*,NSString*> *headers = @{
        @"_URL": request.URL.absoluteString,
        @"_Method": request.HTTPMethod
    }.mutableCopy;

    if (request.allHTTPHeaderFields) {
        [headers addEntriesFromDictionary: request.allHTTPHeaderFields];
    }

    auto body = request.HTTPBody;

    inspector->sendRequest(uid.UTF8String, buildHeaders(headers), body ? std::string((const char*)body.bytes, body.length) : "");
}

+ (void)sendResponseWithUID:(nonnull NSString *)uid response:(nullable NSHTTPURLResponse *)response body:(NSData *)body {

    NSMutableDictionary<NSString*,NSString*> *headers = @{
        @"_Status": response ? @(response.statusCode).stringValue : @"Error"
    }.mutableCopy;

    if (response.allHeaderFields) {
        [headers addEntriesFromDictionary: response.allHeaderFields];
    }

    inspector->sendResponse(uid.UTF8String, buildHeaders(headers), body ? std::string((const char*)body.bytes, body.length) : "");
}

// MARK: - Plugins

+ (void)addPlugin:(nonnull NSString *)key name:(nonnull NSString *)name action:(nonnull PluginActionBlock)action {
    inspector->addPlugin(key.UTF8String, name.UTF8String, [action] {
        return action().UTF8String;
    });
}

+ (void)addLivePlugin:(nonnull NSString *)key name:(nonnull NSString *)name action:(nonnull PluginActionBlock)action {
    inspector->addLivePlugin(key.UTF8String, name.UTF8String, [action] {
        return action().UTF8String;
    });
}

+ (void)addPluginAPIForMethod:(nonnull NSString *)method path:(nonnull NSString *)path action:(nonnull PluginAPIActionBlock)action {
    inspector->addPluginAPI(method.UTF8String, path.UTF8String, [action](const Params &params) {
        return action(NS(params)).UTF8String;
    });
}

+ (void)addPluginAPIForMethod:(nonnull NSString *)method path:(nonnull NSString *)path data:(nonnull PluginAPIActionDataBlock)action {
    inspector->addPluginAPI(method.UTF8String, path.UTF8String, [action](const Params &params) {
        return stringFromData(action(NS(params)));
    });
}

+ (void)sendMessageTo:(nonnull NSString *)key message:(nullable NSString *)message {
    inspector->sendMessage(key.UTF8String, message.UTF8String);
}

// MARK: - Convenience methods

+ (void)addPlugin:(nonnull NSString *)key name:(nonnull NSString *)name plugin:(nonnull id<PluginActionProtocol>)plugin {
    [self addPlugin:key name:name action:^{
        return plugin.action;
    }];
}

+ (void)addLivePlugin:(nonnull NSString *)key name:(nonnull NSString *)name plugin:(nonnull id<PluginActionProtocol>)plugin {
    [self addLivePlugin:key name:name action:^{
        return plugin.action;
    }];
}

@end
