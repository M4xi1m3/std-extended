#include "sock.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <unistd.h>

#include <cstring>
#include <system_error>

using namespace net;

sock::sock(const sock_address& address) {

    m_sockfd = -1;
    m_addrinfo = nullptr;

    struct addrinfo hints;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = address.getFamilly() == sock_address::inet6 ? AF_INET6 : AF_INET;
    hints.ai_socktype = SOCK_STREAM; // TCP

    char buf[10];
    snprintf(buf, 10, "%u", address.getPort());

    int status;
    if ((status = getaddrinfo(address.getAddress().c_str(), buf, &hints, &m_addrinfo)) != 0) {
        freeaddrinfo(m_addrinfo);
        m_addrinfo = nullptr;
        throw std::system_error(status, std::system_category(), gai_strerror(status));
    }

    m_sockfd = socket(m_addrinfo->ai_family, m_addrinfo->ai_socktype, m_addrinfo->ai_protocol);
    if (m_sockfd == -1) {
        freeaddrinfo(m_addrinfo);
        m_addrinfo = nullptr;
        throw std::system_error(errno, std::system_category(), strerror(errno));
    }
}

static void __connect(int sockfd, struct addrinfo** addrinfo) {

    if (*addrinfo == nullptr) {
        throw std::runtime_error("Socket's addrinfo already freed!");
    }

    if (connect(sockfd, (*addrinfo)->ai_addr, (*addrinfo)->ai_family == AF_INET6 ? sizeof(sockaddr_in6) : sizeof(sockaddr_in)) != 0) {
        freeaddrinfo(*addrinfo);
        *addrinfo = nullptr;
        throw std::system_error(errno, std::system_category(), strerror(errno));
    }

    freeaddrinfo(*addrinfo);
    *addrinfo = nullptr;
}

void sock::connect() {
    __connect(m_sockfd, &m_addrinfo);
}

static void __close(int fd) {
    close(fd);
}

void sock::close() {
    if (m_sockfd != -1) {
        __close(m_sockfd);
        m_sockfd = -1;
    }

    if (m_addrinfo != nullptr) {
        freeaddrinfo(m_addrinfo);
        m_addrinfo = nullptr;
    }
}

sock::sock(int sockfd) : m_sockfd(sockfd), m_addrinfo(nullptr) {

}

void sock::setKeepAlive(bool value) {
    int val = value ? 1 : 0;

    if (setsockopt(m_sockfd, SOL_SOCKET, SO_KEEPALIVE, &val, sizeof(val)) != 0) {
        throw std::system_error(errno, std::system_category(), strerror(errno));
    }
}

bool sock::getKeepAlive() {
    int out = 0;
    socklen_t len = sizeof(out);

    if (getsockopt(m_sockfd, SOL_SOCKET, SO_KEEPALIVE, &out, &len) != 0) {
        throw std::system_error(errno, std::system_category(), strerror(errno));
    }

    return out == 0 ? false : true;
}

void sock::setTcpNoDelay(bool value) {
    int val = value ? 1 : 0;

    if (setsockopt(m_sockfd, IPPROTO_TCP, TCP_NODELAY, &val, sizeof(val)) != 0) {
        throw std::system_error(errno, std::system_category(), strerror(errno));
    }
}

bool sock::getTcpNoDelay() {
    int out = 0;
    socklen_t len = sizeof(out);

    if (getsockopt(m_sockfd, IPPROTO_TCP, TCP_NODELAY, &out, &len) != 0) {
        throw std::system_error(errno, std::system_category(), strerror(errno));
    }

    return out == 0 ? false : true;
}

static void* get_in_addr(struct sockaddr* sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*) sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*) sa)->sin6_addr);
}

sock_address sock::getPeerAddress() {
    struct sockaddr_in6 addr;

    socklen_t len;

    char address[INET6_ADDRSTRLEN];
    len = sizeof(addr);
    getpeername(m_sockfd, (struct sockaddr*) &addr, &len);

    inet_ntop(addr.sin6_family, get_in_addr((struct sockaddr*) &addr), address, sizeof address);

    return sock_address(std::string(address), ntohs(addr.sin6_port), addr.sin6_family == AF_INET6 ? sock_address::inet6 : sock_address::inet4);
}

sock_address sock::getSockAddress() {
    struct sockaddr_in6 addr;

    socklen_t len;

    char address[INET6_ADDRSTRLEN];
    len = sizeof(addr);
    getsockname(m_sockfd, (struct sockaddr*) &addr, &len);

    inet_ntop(addr.sin6_family, get_in_addr((struct sockaddr*) &addr), address, sizeof address);

    return sock_address(std::string(address), ntohs(addr.sin6_port), addr.sin6_family == AF_INET6 ? sock_address::inet6 : sock_address::inet4);
}

sock::~sock() {
    close();
}
