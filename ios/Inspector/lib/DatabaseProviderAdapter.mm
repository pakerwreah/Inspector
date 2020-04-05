//
//  DatabaseProviderAdapter.m
//  Inspector
//
//  Created by Paker on 04/04/20.
//

#import "DatabaseProviderAdapter.h"
#import "Inspector.h"

using namespace std;

DatabaseProviderAdapter::DatabaseProviderAdapter(id<IOSInspectorProtocol> _Nonnull delegate) {
    this->delegate = delegate;
}

vector<string> DatabaseProviderAdapter::databasePathList() {
    auto list = vector<string>();
    auto paths = delegate.databaseList;
    for (NSString *path in paths) {
        list.push_back(path.UTF8String);
    }
    return list;
}
