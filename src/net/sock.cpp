#include "net/sock.hpp"

#include <cstring>
#include <system_error>

#ifdef WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#define ERROR_CODE WSAGetLastError()
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <unistd.h>
#define ERROR_CODE errno
#endif

using namespace stde::net;

sock::sock(const sock_address& address) {
    m_sockfd = SOCK_INVALID;
    m_addrinfo = nullptr;

    struct addrinfo hints;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = address.family() == sock_address::ip_family::inet6 ? AF_INET6 : AF_INET;
    hints.ai_socktype = SOCK_STREAM; // TCP

    char buf[10];
    snprintf(buf, 10, "%u", address.port());

    int status;
    if ((status = getaddrinfo(address.address().c_str(), buf, &hints, &m_addrinfo)) != 0) {
        m_addrinfo = nullptr;
        throw std::system_error(status, std::system_category(), gai_strerror(status));
    }

    m_sockfd = socket(m_addrinfo->ai_family, m_addrinfo->ai_socktype, m_addrinfo->ai_protocol);
    if (m_sockfd == SOCK_INVALID) {
        freeaddrinfo(m_addrinfo);
        m_addrinfo = nullptr;
        throw std::system_error(ERROR_CODE, std::system_category(), "socket");
    }
}

static void __connect(int sockfd, struct addrinfo** addrinfo) {

    if (*addrinfo == nullptr) {
        throw std::runtime_error("Socket's addrinfo already freed!");
    }

    if (connect(sockfd, (*addrinfo)->ai_addr, (*addrinfo)->ai_family == AF_INET6 ? sizeof(sockaddr_in6) : sizeof(sockaddr_in)) != 0) {
        freeaddrinfo(*addrinfo);
        *addrinfo = nullptr;
        throw std::system_error(ERROR_CODE, std::system_category(), "connect");
    }

    freeaddrinfo(*addrinfo);
    *addrinfo = nullptr;
}

void sock::connect() {
    __connect(m_sockfd, &m_addrinfo);
}

static void __close(int fd) {
#ifdef WIN32
    closesocket(fd);
#else
    close(fd);
#endif
}

void sock::close() {
    if (m_sockfd != SOCK_INVALID) {
        __close(m_sockfd);
        m_sockfd = SOCK_INVALID;
    }

    if (m_addrinfo != nullptr) {
        freeaddrinfo(m_addrinfo);
        m_addrinfo = nullptr;
    }
}

sock::sock(SOCK_SOCKET_T sockfd) : m_sockfd(sockfd), m_addrinfo(nullptr) {

}

void sock::keep_alive(bool value) {
#ifdef WIN32
    const char val = value ? 1 : 0;
#else
    int val = value ? 1 : 0;
#endif

    if (setsockopt(m_sockfd, SOL_SOCKET, SO_KEEPALIVE, &val, sizeof(val)) != 0) {
        throw std::system_error(ERROR_CODE, std::system_category(), "setsockopt");
    }
}

bool sock::keep_alive() {
#ifdef WIN32
    char out = 0;
#else
    int out = 0;
#endif

    socklen_t len = sizeof(out);

    if (getsockopt(m_sockfd, SOL_SOCKET, SO_KEEPALIVE, &out, &len) != 0) {
        throw std::system_error(ERROR_CODE, std::system_category(), "getsockopt");
    }

    return out == 0 ? false : true;
}

void sock::tcp_no_delay(bool value) {
#ifdef WIN32
    const char val = value ? 1 : 0;
#else
    int val = value ? 1 : 0;
#endif

    if (setsockopt(m_sockfd, IPPROTO_TCP, TCP_NODELAY, &val, sizeof(val)) != 0) {
        throw std::system_error(ERROR_CODE, std::system_category(), "setsockopt");
    }
}

bool sock::tcp_no_delay() {
#ifdef WIN32
    char out = 0;
#else
    int out = 0;
#endif

    socklen_t len = sizeof(out);

    if (getsockopt(m_sockfd, IPPROTO_TCP, TCP_NODELAY, &out, &len) != 0) {
        throw std::system_error(ERROR_CODE, std::system_category(), "getsockopt");
    }

    return out == 0 ? false : true;
}

sock_address sock::peer_address() {
    struct sockaddr_in6 addr;

    socklen_t len;

    char address[INET6_ADDRSTRLEN];
    len = sizeof(addr);
    if (getpeername(m_sockfd, (struct sockaddr*)&addr, &len) == SOCK_INVALID) {
        throw std::system_error(ERROR_CODE, std::system_category(), "getpeername");
    }

    getnameinfo((struct sockaddr*) &addr, sizeof(struct sockaddr), address, sizeof address, nullptr, 0, NI_NUMERICHOST | NI_NUMERICSERV);

    return sock_address(std::string(address), ntohs(addr.sin6_port), addr.sin6_family == AF_INET6 ? sock_address::ip_family::inet6 : sock_address::ip_family::inet4);
}

sock_address sock::address() {
    struct sockaddr_in6 addr;

    socklen_t len;

    char address[INET6_ADDRSTRLEN];
    len = sizeof(addr);
    getsockname(m_sockfd, (struct sockaddr*) &addr, &len);

    getnameinfo((struct sockaddr*) &addr, sizeof(struct sockaddr), address, sizeof address, nullptr, 0, NI_NUMERICHOST | NI_NUMERICSERV);

    return sock_address(std::string(address), ntohs(addr.sin6_port), addr.sin6_family == AF_INET6 ? sock_address::ip_family::inet6 : sock_address::ip_family::inet4);
}

sock::~sock() {
    close();
}
