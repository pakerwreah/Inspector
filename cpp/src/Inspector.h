//
// Created by Paker on 2019-10-23.
//

#ifndef INSPECTOR_INSPECTOR_H
#define INSPECTOR_INSPECTOR_H

#include <vector>
#include <thread>
#include "HttpServer.h"

class Inspector {
    HttpServer server;
    string db_path;
public:
    Inspector();

    thread * bind(int port);

    void preselectDB();

protected:
    virtual vector<string> databaseList() = 0;

};


#endif //INSPECTOR_INSPECTOR_H
