#ifndef Socket_class
#define Socket_class

#include <netinet/in.h>
#include <string>

class Socket {
    int m_sock;
    sockaddr_in m_addr;

public:
    Socket();

    virtual ~Socket();

    bool close();

    bool create();

    bool bind(int port);

    bool listen() const;

    bool accept(Socket *&) const;

    bool connect(const std::string &host, int port);

    void set_non_blocking(bool non_blocking = true);

    bool is_valid() const;

    // Data Transimission
    bool send(const std::string &data) const;

    int recv(std::string &data, const timeval timeout) const;

    int fd() const;
};


#endif
