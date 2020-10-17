#include "MockDatabaseProvider.h"

using namespace std;

vector<string> MockDatabaseProvider::databasePathList() const {
    return databases;
}
