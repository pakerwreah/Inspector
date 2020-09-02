//
// Created by Paker on 09/11/19.
//

#include "NetworkPlugin.h"
#include "compress.hpp"
#include "sha1.h"
#include "base64.h"
#include <sstream>
#include <thread>

using namespace std;

static Response handshake(const Request &request);

static string pack(const string &msg, bool binary = true);

NetworkPlugin::NetworkPlugin(Router *router) {
    router->get("/network/request", [this](const Request &request, const Params &) {
        request_client = request.client;
        return handshake(request);
    });

    router->get("/network/response", [this](const Request &request, const Params &) {
        response_client = request.client;
        return handshake(request);
    });
}

bool NetworkPlugin::isConnected() const {
    return request_client && response_client;
}

void NetworkPlugin::sendRequest(const string &uid, const string &headers, const string &body) {
    // create local reference to avoid deallocation
    auto client = request_client;
    if (client) {
        // create a thread to avoid hanging the client
        thread([=] {
            if (!client->send(pack(uid + "\n" + headers + "\n" + gzip::compress(body.c_str(), body.size())))) {
                request_client = nullptr;
            }
        }).detach();
    }
}

void NetworkPlugin::sendResponse(const string &uid, const string &headers, const string &body, bool compressed) {
    // create local reference to avoid deallocation
    auto client = response_client;
    if (client) {
        // create a thread to avoid hanging the client
        thread([=] {
            auto m_body = body;
            if (!compressed) {
                m_body = gzip::compress(body.c_str(), body.size());
            }
            if (!client->send(pack(uid + "\n" + headers + "\n" + m_body))) {
                response_client = nullptr;
            }
        }).detach();
    }
}

static Response handshake(const Request &request) {
    auto secKey = request.headers.at("Sec-WebSocket-Key");
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

static string pack(const string &msg, bool binary) {
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
