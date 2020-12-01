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

    struct addrinfo hints;
    struct addrinfo *addrinfo;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;     // IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // TCP

    char buf[10];
    snprintf(buf, 10, "%u", address.getPort());

    int status;
    if ((status = getaddrinfo(address.getAddress().c_str(), buf, &hints, &addrinfo)) != 0) {
        freeaddrinfo(addrinfo);
        throw std::system_error(status, std::system_category(), gai_strerror(status));
    }

    m_sockfd = socket(addrinfo->ai_family, addrinfo->ai_socktype, addrinfo->ai_protocol);
    if (m_sockfd == -1) {
        freeaddrinfo(addrinfo);
        throw std::system_error(errno, std::system_category(), strerror(errno));
    }

    if (connect(m_sockfd, addrinfo->ai_addr, sizeof(*addrinfo->ai_addr)) != 0) {
        freeaddrinfo(addrinfo);
        throw std::system_error(errno, std::system_category(), strerror(errno));
    }

    freeaddrinfo(addrinfo);
}

static void __close(int fd) {
    close(fd);
}

void sock::close() {
    if (m_sockfd != -1) {
        __close(m_sockfd);
        m_sockfd = -1;
    }
}

sock::sock(int sockfd) : m_sockfd(sockfd) {

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

    return sock_address(std::string(address), ntohs(addr.sin6_port));
}

sock_address sock::getSockAddress() {
    struct sockaddr_in6 addr;

    socklen_t len;

    char address[INET6_ADDRSTRLEN];
    len = sizeof(addr);
    getsockname(m_sockfd, (struct sockaddr*) &addr, &len);

    inet_ntop(addr.sin6_family, get_in_addr((struct sockaddr*) &addr), address, sizeof address);

    return sock_address(std::string(address), ntohs(addr.sin6_port));
}

sock::~sock() {
    close();
}
