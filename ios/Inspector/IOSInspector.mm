//
//  IOSInspectorDriver.m
//  IOSInspectorDriver
//
//  Created by Paker on 29/10/19.
//

#import "IOSInspector.h"
#import "Inspector.h"

using namespace std;

static id <IOSInspectorProtocol> delegate;
static Inspector *inspector;

@implementation IOSInspector {
    class InspectorImpl : public Inspector {
    protected:
        vector<string> databaseList() override {
            auto list = vector<string>();
            auto paths = delegate.databaseList;
            for (NSString *path in paths) {
                list.push_back(path.UTF8String);
            }
            return list;
        }
    };
}

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

+ (void)initializeWithDelegate:(nonnull id <IOSInspectorProtocol>)_delegate port:(int)port {
    delegate = _delegate;
    inspector = new InspectorImpl;
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

@end
