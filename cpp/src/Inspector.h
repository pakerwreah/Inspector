//
// Created by Paker on 2019-10-23.
//

#ifndef INSPECTOR_INSPECTOR_H
#define INSPECTOR_INSPECTOR_H

#include <vector>
#include <thread>
#include "HttpServer.h"
#include "DatabasePlugin.h"

class Inspector : public DatabaseProvider {
    HttpServer *server;
    DatabasePlugin *databasePlugin;
public:
    Inspector();

    thread *bind(int port);

    void preselectDB();
};


#endif //INSPECTOR_INSPECTOR_H
