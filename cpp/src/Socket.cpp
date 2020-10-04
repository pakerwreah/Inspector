#include "Socket.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>
#include <fcntl.h>
#include <signal.h>

// MSG_NOSIGNAL does not exists on OS X
#if defined(__APPLE__) || defined(__MACH__)
# ifndef MSG_NOSIGNAL
#   define MSG_NOSIGNAL SO_NOSIGPIPE
# endif
#endif

#define MAXRECV 500

Socket::Socket() : m_sock(-1) {
    signal(SIGPIPE, SIG_IGN);
    std::memset(&m_addr, 0, sizeof(m_addr));
}

Socket::~Socket() {
    close();
}

bool Socket::close() {
    if (is_valid()) {
        shutdown(m_sock, SHUT_RDWR);
        return ::close(m_sock) == 0;
    }
    return true;
}

bool Socket::create() {
    m_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (!is_valid()) return false;
    int yes = 1;
    /* we want to be able to re-use ports quickly */
    return setsockopt(m_sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) != -1;
}

bool Socket::bind(int port) {
    if (!is_valid()) return false;

    m_addr.sin_family = AF_INET;
    m_addr.sin_addr.s_addr = INADDR_ANY;
    m_addr.sin_port = htons(port);

    return ::bind(m_sock, (struct sockaddr *) &m_addr, sizeof(m_addr)) != -1;
}

bool Socket::listen() const {
    return is_valid() && ::listen(m_sock, SOMAXCONN) != -1;
}

bool Socket::accept(Socket *&socket) const {
    socket = new Socket;
    socklen_t addr_length = sizeof(m_addr);
    socket->m_sock = ::accept(m_sock, (sockaddr *) &m_addr, &addr_length);
    socket->set_non_blocking(false);
    return socket->is_valid();
}

bool Socket::connect(const std::string &host, int port) {
    if (!is_valid()) return false;

    m_addr.sin_family = AF_INET;
    m_addr.sin_port = htons (port);
    m_addr.sin_addr.s_addr = inet_addr(host == "localhost" ? "127.0.0.1" : host.c_str());

    return ::connect(m_sock, (sockaddr *) &m_addr, sizeof(m_addr)) == 0;
}

void Socket::set_non_blocking(bool non_blocking) {
    int opts = fcntl(m_sock, F_GETFL);

    if (non_blocking)
        opts |= O_NONBLOCK;
    else
        opts &= ~O_NONBLOCK;

    fcntl(m_sock, F_SETFL, opts);
}

bool Socket::is_valid() const {
    return m_sock != -1;
}

bool Socket::send(const std::string &data) const {
    return ::send(m_sock, data.c_str(), data.size(), MSG_NOSIGNAL) != -1;
}

int Socket::recv(std::string &data, const timeval timeout) const {
    if (timeout.tv_sec || timeout.tv_usec) {
        setsockopt(m_sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    }

    char buf[MAXRECV + 1];

    data = "";

    std::memset(buf, 0, MAXRECV + 1);

    int status = (int) ::recv(m_sock, buf, MAXRECV, 0);

    if (status > 0) {
        data = buf;
    }

    return status;
}
