#include "UDPSocket.h"
#include <sys/socket.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <cstring>

// MSG_NOSIGNAL does not exists on OS X
#if defined(__APPLE__) || defined(__MACH__)
# ifndef MSG_NOSIGNAL
#   define MSG_NOSIGNAL SO_NOSIGPIPE
# endif
#endif

UDPSocket::UDPSocket() : m_sock(-1) {
    signal(SIGPIPE, SIG_IGN);
    std::memset(&m_addr, 0, sizeof(m_addr));
}

UDPSocket::~UDPSocket() {
    close();
}

bool UDPSocket::close() {
    if (is_valid()) {
        shutdown(m_sock, SHUT_RDWR);
        int sock = m_sock;
        m_sock = -1;
        return ::close(sock) == 0;
    }
    return true;
}

bool UDPSocket::create() {
    m_sock = socket(PF_INET, SOCK_DGRAM, 0);
    if (!is_valid()) return false;
    int yes = 1;

    return
    setsockopt(m_sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) != -1 &&
    setsockopt(m_sock, SOL_SOCKET, SO_BROADCAST, &yes, sizeof(yes)) != -1;
}

bool UDPSocket::bind(int port) {
    if (!is_valid()) return false;
    if (port < 0 || port > 65535) {
        errno = EINVAL;
        return false;
    }

    m_addr.sin_family = AF_INET;
    m_addr.sin_addr.s_addr = INADDR_BROADCAST;
    m_addr.sin_port = htons(port);

    return true;
}

bool UDPSocket::broadcast(const std::string &data) {
    return ::sendto(m_sock, data.c_str(), data.size(), MSG_NOSIGNAL, (sockaddr *) &m_addr, sizeof(m_addr)) != -1;
}

bool UDPSocket::is_valid() const {
    return m_sock != -1;
}
