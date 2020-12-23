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

#ifdef __MINGW32__
// This is needed because mingw doesn't include it.
static const char* inet_ntop(int af, const void* src, char* dst, socklen_t size) {
    struct sockaddr_storage ss;
    unsigned long s = size;

    ZeroMemory(&ss, sizeof(ss));
    ss.ss_family = af;

    switch (af) {
    case AF_INET:
        ((struct sockaddr_in*)&ss)->sin_addr = *(struct in_addr*)src;
        break;
    case AF_INET6:
        ((struct sockaddr_in6*)&ss)->sin6_addr = *(struct in6_addr*)src;
        break;
    default:
        return NULL;
    }
    /* cannot direclty use &size because of strict aliasing rules */
    return (WSAAddressToString((struct sockaddr*)&ss, sizeof(ss), NULL, dst, &s) == 0) ? dst : NULL;
}

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

sock::sock(sock&& other) : m_sockfd(other.m_sockfd), m_addrinfo(other.m_addrinfo) {
    // Dirty hack to avoid closing of the socket by a destructor call.
    other.m_sockfd = SOCK_INVALID;
    other.m_addrinfo = nullptr;
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

static void __shutdown(SOCK_SOCKET_T fd) {
#ifdef WIN32
    shutdown(fd, SD_BOTH);
#else
    shutdown(fd, SHUT_RDWR);
#endif
}

void sock::shutdown() {
    if (m_sockfd != SOCK_INVALID) {
        __shutdown(m_sockfd);
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

static void* get_in_addr(struct sockaddr* sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*) sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*) sa)->sin6_addr);
}

sock_address sock::peer_address() {
    struct sockaddr_in6 addr;

    socklen_t len;

    char address[INET6_ADDRSTRLEN];
    len = sizeof(addr);
    if (getpeername(m_sockfd, (struct sockaddr*) &addr, &len) == SOCK_INVALID) {
        throw std::system_error(ERROR_CODE, std::system_category(), "getpeername");
    }

    inet_ntop(addr.sin6_family, get_in_addr((struct sockaddr*) &addr), address, sizeof address);

    return sock_address(std::string(address), ntohs(addr.sin6_port),
            addr.sin6_family == AF_INET6 ? sock_address::ip_family::inet6 : sock_address::ip_family::inet4);
}

sock_address sock::address() {
    struct sockaddr_in6 addr;

    socklen_t len;

    char address[INET6_ADDRSTRLEN];
    len = sizeof(addr);
    getsockname(m_sockfd, (struct sockaddr*) &addr, &len);

    inet_ntop(addr.sin6_family, get_in_addr((struct sockaddr*) &addr), address, sizeof address);

    return sock_address(std::string(address), ntohs(addr.sin6_port),
            addr.sin6_family == AF_INET6 ? sock_address::ip_family::inet6 : sock_address::ip_family::inet4);
}

sock::~sock() {
    close();
}
