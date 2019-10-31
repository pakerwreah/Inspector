//
//  IOSInspectorDriver.m
//  IOSInspectorDriver
//
//  Created by Paker on 29/10/19.
//  Copyright © 2019 NewM. All rights reserved.
//

#import "IOSInspector.h"
#import "Inspector.h"

using namespace std;

static id <IOSInspectorProtocol> delegate;

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

+ (void)initializeWithDelegate:(nonnull id <IOSInspectorProtocol>)_delegate port:(int)port {
    delegate = _delegate;
    auto inspector = new InspectorImpl;
    inspector->preselectDB();
    inspector->bind(port);
}

@end
