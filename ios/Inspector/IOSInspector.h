//
//  IOSInspectorDriver.h
//  IOSInspectorDriver
//
//  Created by Paker on 29/10/19.
//  Copyright © 2019 NewM. All rights reserved.
//

#import <Foundation/Foundation.h>

@protocol IOSInspectorProtocol

- (nonnull NSArray<NSString *> *)databaseList;

@end

@interface IOSInspector : NSObject

+ (void)initializeWithDelegate:(nonnull id <IOSInspectorProtocol>)delegate port:(int)port;

@end