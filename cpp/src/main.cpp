//
// Created by Paker on 26/10/19.
//

#include "Inspector.h"

class TestInspector : public Inspector {
protected:
    vector<string> databaseList() override {
        return {"database_1.db"};
    }
};

int main() {

    TestInspector inspector;

    auto th = inspector.bind(30000);

    inspector.preselectDB();

    th->join();

    return 0;
}