//
// Created by Paker on 2019-10-23.
//

#include "HttpServer.h"
#include "SocketClient.h"
#include "compress.hpp"

#include <thread>

using namespace std;

HttpServer::HttpServer() : _listening(false), _stop(false), _error(0), interval(1s) {}

HttpServer::~HttpServer() {
    stop();
}

int HttpServer::error() const {
    return _error;
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
        do {
            if (server.create() && server.bind(port) && server.listen()) {
                for (int i = 0; !_stop && i < 3; i++) {
                    _listening = true;
                    if (Socket *socket; server.accept(socket)) {
                        i = -1;
                        auto socket_client = make_shared<SocketClient>(unique_ptr<Socket>(socket));
                        thread(&HttpServer::process, this, std::move(socket_client)).detach();
                    } else if (server.is_valid()) {
                        _error = errno;
                    }
                }
                server.close();
                _listening = false;
            } else {
                _error = errno;
            }
            if (!_stop) {
                this_thread::sleep_for(interval);
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

                (void)client->send(response);
            } else {
                try {
                    Response response = router.handle(request);

                    if (!response.body.empty() && request.headers["Accept-Encoding"].find("gzip") != string::npos) {
                        response.body = gzip::compress(response.body.data(), response.body.size());
                        response.headers["Content-Encoding"] = "gzip";
                    }

                    (void)client->send(response);

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

void HttpServer::setReconnectInterval(std::chrono::nanoseconds interval) {
    this->interval = interval;
}
