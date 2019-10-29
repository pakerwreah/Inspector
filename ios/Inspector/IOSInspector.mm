//
//  IOSInspectorDriver.m
//  IOSInspectorDriver
//
//  Created by Paker on 29/10/19.
//  Copyright © 2019 NewM. All rights reserved.
//

#import "IOSInspector.h"
#import "Inspector.h"
#import <UIKit/UIKit.h>

using namespace std;

class InspectorImpl : public Inspector {
protected:
    vector<string> databaseList() override {
        NSString *documentsPath = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) firstObject];
        
        auto list = vector<string>();

        for (id name : @[@"banco_1.db", @"banco_2.db"]) {
            auto path = [[documentsPath stringByAppendingPathComponent: name] UTF8String];
            list.push_back(path);
        }
        
        return list;
    }
};

@implementation IOSInspector

- (instancetype)init
{
    self = [super init];
    if (self) {
        auto inspector = new InspectorImpl;
        inspector->bind(30000);
    }
    return self;
}

@end
