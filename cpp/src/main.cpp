//
// Created by Paker on 26/10/19.
//

#include "Inspector.h"
#include <vector>

class TestInspector : public Inspector {
protected:
    vector<string> databaseList() override {
        return {"database.db", "database2.db"};
    }
};

int main() {

    TestInspector inspector;

    auto th = inspector.bind(30000);

    inspector.preselectDB();

    th->join();

    return 0;
}