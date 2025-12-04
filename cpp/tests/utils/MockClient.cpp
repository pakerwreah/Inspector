#include "MockClient.h"

std::string MockClient::read() const {
    return recv;
}

bool MockClient::send(const std::string &data) const {
    sent = connected ? data : "";
    return connected;
}
