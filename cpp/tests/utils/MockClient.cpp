#include "MockClient.h"

using namespace std;

string MockClient::read() const {
    return recv;
}

bool MockClient::send(const string &data) const {
    sent = connected ? data : "";
    return connected;
}
