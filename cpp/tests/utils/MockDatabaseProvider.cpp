#include "MockDatabaseProvider.h"

using namespace std;

vector<string> MockDatabaseProvider::databasePathList() {
    return databases;
}
