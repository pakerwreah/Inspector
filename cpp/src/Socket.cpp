#include "Socket.h"
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>
#include <cerrno>
#include <fcntl.h>
#include <signal.h>

Socket::Socket() : m_sock(-1) {
    signal(SIGPIPE, SIG_IGN);
    std::memset(&m_addr, 0, sizeof(m_addr));
}

Socket::~Socket() {
    if (is_valid()) {
        close(m_sock);
    }
}

bool Socket::create() {
    m_sock = socket(AF_INET, SOCK_STREAM, 0);
    fcntl(m_sock, F_SETFD, FD_CLOEXEC);

    if (!is_valid()) {
        return false;
    }

    int one = 1;
    /* we want to be able to re-use ports quickly */
    return setsockopt(m_sock, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one)) != -1;
}

bool Socket::bind(const int port) {

    if (!is_valid()) {
        return false;
    }

    m_addr.sin_family = AF_INET;
    m_addr.sin_addr.s_addr = INADDR_ANY;
    m_addr.sin_port = htons (port);

    return ::bind(m_sock, (struct sockaddr *) &m_addr, sizeof(m_addr)) != -1;
}


bool Socket::listen() const {
    return is_valid() && ::listen(m_sock, SOMAXCONN) != -1;
}


bool Socket::accept(Socket *&new_socket) const {
    new_socket = new Socket;
    socklen_t addr_length = sizeof(m_addr);
    new_socket->m_sock = ::accept(m_sock, (sockaddr *) &m_addr, &addr_length);
    fcntl(new_socket->m_sock, F_SETFD, FD_CLOEXEC);

    return new_socket->m_sock > 0;
}


bool Socket::send(const std::string &s) const {
    return ::send(m_sock, s.c_str(), s.size(), MSG_NOSIGNAL) != -1;
}


int Socket::recv(std::string &data, timeval timeout) const {
    if (timeout.tv_sec || timeout.tv_usec) {
        setsockopt(m_sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    }

    char buf[MAXRECV + 1];

    data = "";

    std::memset(buf, 0, MAXRECV + 1);

    int status = (int) ::recv(m_sock, buf, MAXRECV, 0);

    if (status == -1) {
        if (errno != EAGAIN) {
            std::cout << "status == -1   errno == " << errno << "  in Socket::recv\n";
        }
        return 0;
    } else if (status == 0) {
        return 0;
    } else {
        data = buf;
        return status;
    }
}

bool Socket::connect(const std::string &host, const int port, int timeout) {
    if (!is_valid()) return false;

    m_addr.sin_family = AF_INET;
    m_addr.sin_port = htons (port);

    inet_pton(AF_INET, host.c_str(), &m_addr.sin_addr);

    if (errno == EAFNOSUPPORT) return false;

    set_non_blocking(timeout > 0);

    int status = ::connect(m_sock, (sockaddr *) &m_addr, sizeof(m_addr));
    return timeout > 0 ? waitTimeout() : status == 0;
}

bool Socket::waitTimeout() {

    bool ret = false;

    if (errno == EINPROGRESS) {
        FD_ZERO(&fdset);
        FD_SET(m_sock, &fdset);
        tv.tv_sec = 5;
        tv.tv_usec = 0;

        if (select(m_sock + 1, nullptr, &fdset, nullptr, &tv) == 1) {
            int so_error;
            socklen_t len = sizeof so_error;

            getsockopt(m_sock, SOL_SOCKET, SO_ERROR, &so_error, &len);

            if (so_error == 0)
                ret = true;
        }
    } else {
        ret = false;
    }

    set_non_blocking(false);

    return ret;
}

void Socket::set_non_blocking(const bool b) {
    int opts = fcntl(m_sock, F_GETFL);

    if (opts < 0) {
        return;
    }

    if (b)
        opts = (opts | O_NONBLOCK);
    else
        opts = (opts & ~O_NONBLOCK);

    fcntl(m_sock, F_SETFL, opts);
}
