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

@protocol PluginActionProtocol
- (nonnull NSString *)action;
@end

typedef NSString * _Nonnull(^PluginActionBlock)();
typedef NSString * _Nonnull(^PluginAPIActionBlock)(NSDictionary<NSString *, NSString *> * _Nonnull);
typedef NSData * _Nullable(^PluginAPIActionDataBlock)(NSDictionary<NSString *, NSString *> * _Nonnull);

@interface IOSInspector : NSObject

+ (void)initializeWithDelegate:(nonnull id <IOSInspectorProtocol>)delegate;
+ (void)initializeWithDelegate:(nonnull id <IOSInspectorProtocol>)delegate port:(int)port;

// MARK: - Database
+ (void)createDatabase:(nonnull NSString *)path;
+ (void)createDatabase:(nonnull NSString *)path password:(nonnull NSString *)password version:(int)version;

+ (void)setCipherKey:(nonnull NSString *)database password:(nonnull NSString *)password version:(int)version;

// MARK: - Network
+ (void)sendRequestWithUID:(nonnull NSString *)uid request:(nonnull NSURLRequest *)request;
+ (void)sendResponseWithUID:(nonnull NSString *)uid response:(nullable NSHTTPURLResponse *)response body:(nullable NSData *)body;

// MARK: - Plugins
+ (void)addPlugin:(nonnull NSString *)key name:(nonnull NSString *)name action:(nonnull PluginActionBlock)action;
+ (void)addLivePlugin:(nonnull NSString *)key name:(nonnull NSString *)name action:(nonnull PluginActionBlock)action;
+ (void)addPluginAPIForMethod:(nonnull NSString *)method path:(nonnull NSString *)path action:(nonnull PluginAPIActionBlock)action;
+ (void)addPluginAPIForMethod:(nonnull NSString *)method path:(nonnull NSString *)path data:(nonnull PluginAPIActionDataBlock)action;
+ (void)sendMessageTo:(nonnull NSString *)key message:(nullable NSString *)message;

// MARK: - Convenience methods
+ (void)addPlugin:(nonnull NSString *)key name:(nonnull NSString *)name plugin:(nonnull id<PluginActionProtocol>)plugin;
+ (void)addLivePlugin:(nonnull NSString *)key name:(nonnull NSString *)name plugin:(nonnull id<PluginActionProtocol>)plugin;

@end
