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
    fd_set fdset;
    constexpr inline static timeval timezero{0, 0};

public:
    Socket();

    virtual ~Socket();

    // Server initialization
    bool create();

    bool bind(const int port);

    bool listen() const;

    bool accept(Socket *&) const;

    // Client initialization
    bool connect(const std::string &host, const int port, const timeval timeout = timezero);

    bool waitTimeout(timeval tv);

    // Data Transimission
    bool send(const std::string &data) const;

    int recv(std::string &data, const timeval timeout = timezero) const;

    void set_non_blocking(const bool = true);

    bool is_valid() const;
};


#endif
