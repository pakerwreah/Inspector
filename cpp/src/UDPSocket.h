#ifndef INSPECTOR_UDPSOCKET_H
#define INSPECTOR_UDPSOCKET_H

#include <netinet/in.h>
#include <string>

class UDPSocket {
private:
    int m_sock;
    sockaddr_in m_addr;

public:
    UDPSocket();

    virtual ~UDPSocket();

    bool close();

    bool create();

    bool bind(int port);

    bool broadcast(const std::string &datagram);

    bool is_valid() const;
};

#endif //INSPECTOR_UDPSOCKET_H
