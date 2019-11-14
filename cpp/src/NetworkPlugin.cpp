//
// Created by Paker on 09/11/19.
//

#include <sstream>
#include "NetworkPlugin.h"
#include "HttpServer.h"
#include "Socket.h"
#include "libs/compress.hpp"
#include "libs/sha1.h"
#include "libs/base64.h"

static Response handshake(Request &request);

static string pack(string msg, bool binary = true);

NetworkPlugin::NetworkPlugin(HttpServer *server) {
    server->get("/network/request", [this](Request req) {
        request_socket = req.socket;
        return handshake(req);
    });

    server->get("/network/response", [this](Request req) {
        response_socket = req.socket;
        return handshake(req);
    });
}

bool NetworkPlugin::isConnected() {
    return request_socket && response_socket;
}

void NetworkPlugin::sendRequest(string uid, string headers, string body) {
    // create local reference to avoid deallocation
    auto socket = request_socket;
    if (socket) {
        // create a thread to avoid hanging the client
        thread([=] {
            if (!socket->send(pack(uid + "\n" + headers + "\n" + gzip::compress(body.c_str(), body.size())))) {
                request_socket = nullptr;
            }
        }).detach();
    }
}

void NetworkPlugin::sendResponse(string uid, string headers, string body, bool compressed) {
    // create local reference to avoid deallocation
    auto socket = response_socket;
    if (socket) {
        // create a thread to avoid hanging the client
        thread([=, b = body] {
            auto body = b;
            if (!compressed) {
                body = gzip::compress(body.c_str(), body.size());
            }
            if (!socket->send(pack(uid + "\n" + headers + "\n" + body))) {
                response_socket = nullptr;
            }
        }).detach();
    }
}

static Response handshake(Request &request) {
    auto secKey = request.headers["Sec-WebSocket-Key"];
    auto magic = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
    auto secAccept = base64::encode(sha1::calc(secKey + magic));

    Response response;
    response.code = 101;
    response.headers = {
            {"Connection",           "Upgrade"},
            {"Upgrade",              "websocket"},
            {"Sec-WebSocket-Accept", secAccept}
    };

    return response;
}

static string pack(string msg, bool binary) {
    auto length = msg.size();

    ostringstream wrap;

    wrap << char(binary ? 130 : 129);
    if (length <= 125) {
        wrap << char(length);
    } else if (length <= 65535) {
        wrap << char(126);
        wrap << char((length >> 8) & 255);
        wrap << char((length) & 255);
    } else {
        wrap << char(127);
        wrap << char((length >> 56) & 255);
        wrap << char((length >> 48) & 255);
        wrap << char((length >> 40) & 255);
        wrap << char((length >> 32) & 255);
        wrap << char((length >> 24) & 255);
        wrap << char((length >> 16) & 255);
        wrap << char((length >> 8) & 255);
        wrap << char((length) & 255);
    }

    wrap << msg;

    return wrap.str();
}
