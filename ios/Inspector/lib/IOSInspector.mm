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

static string buildHeaders(NSDictionary<NSString*,NSString*> *headers) {
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

+ (void)initializeWithDelegate:(nonnull id <IOSInspectorProtocol>)delegate port:(int)port {
    inspector = new Inspector(new DatabaseProviderAdapter(delegate));
    inspector->bind(port);
}

+ (void)setCipherKey:(nonnull NSString *) database password:(nonnull NSString *)password version:(int)version {
    inspector->setCipherKey(database.UTF8String, password.UTF8String, version);
}

+ (void)sendRequestWithUID:(nonnull NSString *) uid request:(nonnull NSURLRequest*)request {

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

+ (void)sendResponseWithUID:(nonnull NSString *) uid response:(nullable NSHTTPURLResponse*)response body:(NSData *)body {

    NSMutableDictionary<NSString*,NSString*> *headers = @{
        @"_Status": response ? @(response.statusCode).stringValue : @"Error"
    }.mutableCopy;

    if(response) {
        [headers addEntriesFromDictionary: response.allHeaderFields];
    }

    inspector->sendResponse(uid.UTF8String, buildHeaders(headers), body ? string((const char*)body.bytes, body.length) : "");
}

+ (void)addPlugin:(nonnull NSString *) key name:(nonnull NSString *)name action:(nonnull PluginActionBlock)action {
    inspector->addPlugin(key.UTF8String, name.UTF8String, [action] {
        return action().UTF8String;
    });
}

@end
