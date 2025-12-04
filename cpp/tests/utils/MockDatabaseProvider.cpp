#include "MockDatabaseProvider.h"

std::vector<std::string> MockDatabaseProvider::databasePathList() const {
    return databases;
}
