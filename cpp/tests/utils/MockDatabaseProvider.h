#pragma once

#include "DatabasePlugin.h"

struct MockDatabaseProvider : public DatabaseProvider {
    std::vector<std::string> databases;
    std::vector<std::string> databasePathList() const override;
};
