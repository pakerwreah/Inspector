#ifndef INSPECTOR_UDPSOCKET_H
#define INSPECTOR_UDPSOCKET_H

#include <string>
#include <netinet/in.h>

class UDPSocket {
    int m_sock;
    sockaddr_in m_addr;

public:
    UDPSocket();

    virtual ~UDPSocket();

    bool create();

    bool bind(int port);

    bool broadcast(const std::string &datagram);

    bool is_valid() const;
};


#endif //INSPECTOR_UDPSOCKET_H
