#include "MockClient.h"

using namespace std;

string MockClient::read() {
    return request;
}

bool MockClient::send(const string &data) {
    response = data;
    return true;
}
