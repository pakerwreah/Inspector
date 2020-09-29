//
// Created by Paker on 2019-10-23.
//

#include "HttpServer.h"
#include "SocketClient.h"
#include "compress.hpp"

using namespace std;

HttpServer::HttpServer() : _listening(false), _stop(false) {}

HttpServer::~HttpServer() {
    stop();
}

bool HttpServer::stop() {
    _stop = true;
    return server.close();
}

bool HttpServer::listening() const {
    return _listening;
}

thread *HttpServer::start(int port) {
    _stop = false;
    return new thread([this, port] {
        if (server.create())
            do {
                if (server.bind(port) && server.listen()) {
                    for (int i = 0; !_stop && i < 3; i++) {
                        _listening = true;
                        Socket *client;
                        if (server.accept(client)) {
                            i = -1;
                            auto socket_client = make_shared<SocketClient>(unique_ptr<Socket>(client));
                            thread(&HttpServer::process, this, socket_client).detach();
                        }
                    }
                    _listening = false;
                }
                if (!_stop) {
                    sleep(1); // just to avoid an infinite cpu hogging loop
                }
            } while (!_stop);
    });
}

void HttpServer::process(shared_ptr<Client> client) const {
    Request request(client);
    string plain, buf;
    bool valid;
    int tries = 3;

    do {
        plain += client->read();

        if ((valid = request.parse(plain))) {
            if (request.method == "OPTIONS") {
                Response response;
                response.headers["Access-Control-Allow-Methods"] = "GET, POST, PUT, PATCH, DELETE";

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
