#ifndef INSPECTOR_MOCKDATABASEPROVIDER_H
#define INSPECTOR_MOCKDATABASEPROVIDER_H

#include "DatabasePlugin.h"

struct MockDatabaseProvider : public DatabaseProvider {
    std::vector<std::string> databases;
    std::vector<std::string> databasePathList() override;
};


#endif //INSPECTOR_MOCKDATABASEPROVIDER_H
