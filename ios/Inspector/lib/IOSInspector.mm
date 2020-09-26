//
//  IOSInspectorDriver.m
//  IOSInspectorDriver
//
//  Created by Paker on 29/10/19.
//

#import "IOSInspector.h"
#import "DatabaseProviderAdapter.h"

using namespace std;

static Inspector *inspector;

@implementation IOSInspector

// MARK: - Utilities

static inline string stringFromData(NSData *str) {
    return string((const char*)str.bytes, str.length);
}

static inline NSString* NS(const string &str) {
    return [NSString stringWithUTF8String:str.c_str()];
}

static inline NSDictionary<NSString *, NSString *>* NS(const Params &params) {
    auto dict = [NSMutableDictionary<NSString *, NSString *> dictionaryWithCapacity:params.size()];
    for (const auto &p : params) {
        dict[NS(p.first)] = NS(p.second);
    }
    return dict;
}

static string buildHeaders(NSDictionary<NSString *,NSString *> *headers) {
    string headerstr;
    NSArray<NSString *> *sorted_headers = [[headers allKeys] sortedArrayUsingSelector:@selector(localizedCaseInsensitiveCompare:)];
    for(NSString *_key in sorted_headers) {
        NSString *key = _key;
        if([key hasPrefix:@"_"]) {
            key = [key substringFromIndex: 1];
        }
        headerstr += string() + key.UTF8String + ": " + headers[_key].UTF8String + "\n";
    }
    return headerstr;
}

// MARK: - Initializer

+ (void)initializeWithDelegate:(nonnull id <IOSInspectorProtocol>)delegate port:(int)port {
    inspector = new Inspector(new DatabaseProviderAdapter(delegate));
    inspector->bind(port);
}

// MARK: - Database

+ (void)setCipherKey:(nonnull NSString *)database password:(nonnull NSString *)password version:(int)version {
    inspector->setCipherKey(database.UTF8String, password.UTF8String, version);
}

// MARK: - Network

+ (void)sendRequestWithUID:(nonnull NSString *)uid request:(nonnull NSURLRequest *)request {

    NSMutableDictionary<NSString*,NSString*> *headers = @{
        @"_URL": request.URL.absoluteString,
        @"_Method" : request.HTTPMethod
    }.mutableCopy;
    
    if (!headers) {
        [headers addEntriesFromDictionary: request.allHTTPHeaderFields];
    }

    auto body = request.HTTPBody;

    inspector->sendRequest(uid.UTF8String, buildHeaders(headers), body ? string((const char*)body.bytes, body.length) : "");
}

+ (void)sendResponseWithUID:(nonnull NSString *)uid response:(nullable NSHTTPURLResponse *)response body:(NSData *)body {

    NSMutableDictionary<NSString*,NSString*> *headers = @{
        @"_Status": response ? @(response.statusCode).stringValue : @"Error"
    }.mutableCopy;

    if(response) {
        [headers addEntriesFromDictionary: response.allHeaderFields];
    }

    inspector->sendResponse(uid.UTF8String, buildHeaders(headers), body ? string((const char*)body.bytes, body.length) : "");
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
