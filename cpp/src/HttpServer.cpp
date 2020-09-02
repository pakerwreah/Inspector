//
// Created by Paker on 2019-10-23.
//

#include "HttpServer.h"
#include "SocketClient.h"
#include "compress.hpp"

using namespace std;

HttpServer::~HttpServer() {
    stop();
}

void HttpServer::stop() {
    _stop = true;
}

thread *HttpServer::start(int port) {
    _stop = false;
    return new thread([this, port] {
        Socket server;
        server.create();

        while (!_stop) {
            if (server.bind(port) && server.listen()) {
                for (int i = 0; !_stop && i < 3; i++) {
                    Socket *client;
                    if (server.accept(client)) {
                        i = -1;
                        auto socket_client = make_shared<SocketClient>(unique_ptr<Socket>(client));
                        thread(&HttpServer::process, this, socket_client).detach();
                    }
                }
            }
            sleep(1); // just to avoid an infinite cpu hogging loop
        }
    });
}

void HttpServer::process(shared_ptr<Client> client) const {
    string plain, buf;
    bool valid;
    int tries = 3;

    do {
        plain += client->read();

        Request request(client);

        if ((valid = request.parse(plain))) {
            if (request.method == "OPTIONS") {
                Response response;
                response.headers["Access-Control-Allow-Methods"] = "GET, POST, PUT";

                client->send(response);
            } else {
                try {
                    Response response = router.handle(request);

                    if (!response.body.empty() && request.headers["Accept-Encoding"].find("gzip") != string::npos) {
                        response.body = gzip::compress(response.body.data(), response.body.size());
                        response.headers["Content-Encoding"] = "gzip";
                    }

                    client->send(response);

                } catch (const out_of_range &ex) {
                    client->send(Response::NotFound(ex.what()));
                } catch (const exception &ex) {
                    client->send(Response::InternalError(ex.what()));
                }
            }
        }
    } while (--tries && !valid);

    if (!valid) {
        client->send(Response::BadRequest());
    }
}
