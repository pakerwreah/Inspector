//
// Created by Paker on 22/09/20.
//

#include "MockHttpServer.h"

using namespace std;

void MockHttpServer::process(shared_ptr<Client> client) const {
    processor(client);
}
