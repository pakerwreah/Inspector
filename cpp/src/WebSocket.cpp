//
// Created by Paker on 13/09/20.
//

#include "WebSocket.h"
#include "base64.h"
#include "sha1.h"
#include <sstream>

using namespace std;

WebSocket::WebSocket(shared_ptr<Client> client) : client(std::move(client)) {}

bool WebSocket::send(const string &data, bool binary) const {
    return client->send(pack(data, binary));
}

Response WebSocket::handshake(const Request &request) {
    const auto &it = request.headers.find("Sec-WebSocket-Key");
    if (it != request.headers.end()) {
        const string secKey = it->second;
        const string magic = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
        const string secAccept = base64::encode(sha1::calc(secKey + magic));

        Response response;
        response.code = 101;
        response.headers = {
                {"Connection",           "Upgrade"},
                {"Upgrade",              "websocket"},
                {"Sec-WebSocket-Accept", secAccept}
        };

        return response;
    } else {
        return Response::BadRequest("Sec-WebSocket-Key header not found");
    }
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wshift-count-overflow"

string WebSocket::pack(const string &msg, bool binary) {
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

#pragma clang diagnostic pop
