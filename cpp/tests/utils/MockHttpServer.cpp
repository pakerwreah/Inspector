//
// Created by Paker on 22/09/20.
//

#include "MockHttpServer.h"

void MockHttpServer::process(std::shared_ptr<Client> client) const {
    processor(client);
}
