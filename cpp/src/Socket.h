#ifndef Socket_class
#define Socket_class

#include <netinet/in.h>
#include <string>

// MSG_NOSIGNAL does not exists on OS X
#if defined(__APPLE__) || defined(__MACH__)
# ifndef MSG_NOSIGNAL
#   define MSG_NOSIGNAL SO_NOSIGPIPE
# endif
#endif

const int MAXRECV = 500;

class Socket {
    int m_sock;
    sockaddr_in m_addr;

public:
    Socket();

    virtual ~Socket();

    bool close();

    bool create();

    bool bind(const int port);

    bool listen() const;

    bool accept(Socket *&) const;

    bool connect(const std::string &host, const int port);

    void set_non_blocking(bool non_blocking = true);

    bool is_valid() const;

    // Data Transimission
    bool send(const std::string &data) const;

    int recv(std::string &data, const timeval timeout) const;
};


#endif
