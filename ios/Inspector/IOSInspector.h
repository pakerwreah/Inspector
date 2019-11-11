//
//  IOSInspectorDriver.h
//  IOSInspectorDriver
//
//  Created by Paker on 29/10/19.
//

#import <Foundation/Foundation.h>

@protocol IOSInspectorProtocol

- (nonnull NSArray<NSString *> *)databaseList;

@end

@interface IOSInspector : NSObject

+ (void)initializeWithDelegate:(nonnull id <IOSInspectorProtocol>)delegate port:(int)port;

+ (void)sendRequestWithUID:(nonnull NSString *) uid request:(nonnull NSURLRequest*)request;

+ (void)sendResponseWithUID:(nonnull NSString *) uid response:(nullable NSHTTPURLResponse*)response body:(nullable NSData *)body;

@end
