#include "MockClient.h"

using namespace std;

string MockClient::read() {
    return recv;
}

bool MockClient::send(const string &data) {
    sent = connected ? data : "";
    return connected;
}
