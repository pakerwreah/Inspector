// Definition of the Socket class

#ifndef Socket_class
#define Socket_class

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <string>
#include <arpa/inet.h>

// MSG_NOSIGNAL does not exists on OS X
#if defined(__APPLE__) || defined(__MACH__)
# ifndef MSG_NOSIGNAL
#   define MSG_NOSIGNAL SO_NOSIGPIPE
# endif
#endif


const int MAXHOSTNAME = 200;
const int MAXRECV = 500;

class Socket {
    fd_set fdset{};
    struct timeval tv{};

public:
    Socket();

    virtual ~Socket();

    // Server initialization
    bool create();

    bool bind(const int port);

    bool listen() const;

    bool accept(Socket *&) const;

    // Client initialization
    bool connect(const std::string &host, const int port, int timeout = 0);

    bool waitTimeout();

    // Data Transimission
    bool send(const std::string &data) const;

    int recv(std::string &data, timeval timeout = {}) const;

    void set_non_blocking(const bool = true);

    bool is_valid() const { return m_sock != -1; }

private:

    int m_sock;
    sockaddr_in m_addr{};


};


#endif
