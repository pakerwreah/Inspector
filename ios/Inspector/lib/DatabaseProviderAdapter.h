//
//  DatabaseProviderAdapter.h
//  Inspector
//
//  Created by Paker on 04/04/20.
//

#import "IOSInspector.h"
#import "Inspector.h"

class DatabaseProviderAdapter : public DatabaseProvider {
    id<IOSInspectorProtocol> _Nonnull delegate;
    
public:
    DatabaseProviderAdapter(id<IOSInspectorProtocol> _Nonnull delegate);
    
protected:
    vector<string> databasePathList() override;
};
