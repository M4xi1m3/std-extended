#include "net/serversock.hpp"

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
#include <unistd.h>
#include <fcntl.h>
#define ERROR_CODE errno
#endif

#include <cstring>
#include <system_error>

using namespace stde::net;

server_sock::server_sock(const sock_address& address) {
    m_sockfd = SERVERSOCK_INVALID;
#ifdef WIN32
    m_blocking = false;
#endif


    struct addrinfo hints;
    m_addrinfo = nullptr;

    memset(&hints, 0, sizeof hints);

    if (address.family() == sock_address::ip_family::undef) {
        throw std::runtime_error("Invalid address familly!");
    }

    hints.ai_family = address.family() == sock_address::ip_family::inet6 ? AF_INET6 : AF_INET;
    hints.ai_socktype = SOCK_STREAM; // TCP
    hints.ai_flags = AI_PASSIVE;     // Local address

    char buf[10];
    snprintf(buf, 10, "%u", address.port());

    int status;
    if ((status = getaddrinfo(address.address().c_str(), buf, &hints, &m_addrinfo)) != 0) {
        m_addrinfo = nullptr;
        throw std::system_error(status, std::system_category(), gai_strerror(status));
    }

    m_sockfd = socket(m_addrinfo->ai_family, m_addrinfo->ai_socktype, m_addrinfo->ai_protocol);
    if (m_sockfd == SERVERSOCK_INVALID) {
        freeaddrinfo(m_addrinfo);
        m_addrinfo = nullptr;
        throw std::system_error(ERROR_CODE, std::system_category(), "socket");
    }
}

server_sock::server_sock(server_sock&& other) : m_sockfd(other.m_sockfd), m_addrinfo(other.m_addrinfo) {
    // Dirty hack to avoid closing of the socket by a destructor call.
    other.m_sockfd = SOCK_INVALID;
    other.m_addrinfo = nullptr;
#ifdef WIN32
    m_blocking = other.m_blocking;
#endif
}

static void __listen(SERVERSOCK_SOCKET_T sockfd) {
    if (listen(sockfd, 0) != 0) {
        throw std::system_error(ERROR_CODE, std::system_category(), "listen");
    }
}

void server_sock::listen() {
    __listen(m_sockfd);
}

static void __bind(SERVERSOCK_SOCKET_T sockfd, struct addrinfo** addrinfo) {

    if (*addrinfo == nullptr) {
        throw std::runtime_error("Socket's addrinfo already freed!");
    }

    if (bind(sockfd, (*addrinfo)->ai_addr, (*addrinfo)->ai_addrlen) == SERVERSOCK_INVALID) {
        freeaddrinfo(*addrinfo);
        *addrinfo = nullptr;
        throw std::system_error(ERROR_CODE, std::system_category(), "bind");
    }

    freeaddrinfo(*addrinfo);
    *addrinfo = nullptr;
}

void server_sock::bind() {
    __bind(m_sockfd, &m_addrinfo);
}

SERVERSOCK_SOCKET_T __accept(SERVERSOCK_SOCKET_T sockfd) {
    return accept(sockfd, nullptr, nullptr);
}

sock server_sock::accept() {
    SERVERSOCK_SOCKET_T client_sockfd = __accept(m_sockfd);

    if (client_sockfd == SERVERSOCK_INVALID) {
        throw std::system_error(ERROR_CODE, std::system_category(), "accept");
    }

    sock client(client_sockfd);
    return client;
}

void server_sock::reuse_address(bool value) {
#ifdef WIN32
    const char val = value ? 1 : 0;
#else
    int val = value ? 1 : 0;
#endif

    if (setsockopt(m_sockfd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) != 0) {
        throw std::system_error(ERROR_CODE, std::system_category(), "setsockopt");
    }
}

bool server_sock::reuse_address() {
#ifdef WIN32
    char out = 0;
#else
    int out = 0;
#endif

    socklen_t len = sizeof(out);

    if (getsockopt(m_sockfd, SOL_SOCKET, SO_REUSEADDR, &out, &len) != 0) {
        throw std::system_error(ERROR_CODE, std::system_category(), "getsockopt");
    }

    return out == 0 ? false : true;
}

static void __close(SERVERSOCK_SOCKET_T fd) {
#ifdef WIN32
    closesocket(fd);
#else
    close(fd);
#endif
}

void server_sock::close() {
    if (m_sockfd != SERVERSOCK_INVALID) {
        __close(m_sockfd);
        m_sockfd = SERVERSOCK_INVALID;
    }

    if (m_addrinfo != nullptr) {
        freeaddrinfo(m_addrinfo);
        m_addrinfo = nullptr;
    }
}

sock_address server_sock::address() {
    struct sockaddr_in6 addr;

    socklen_t len;

    char address[INET6_ADDRSTRLEN];
    len = sizeof(addr);
    getsockname(m_sockfd, (struct sockaddr*) &addr, &len);

    getnameinfo((sockaddr*) &addr, len, address, sizeof address, nullptr, 0, NI_NUMERICHOST | NI_NUMERICSERV);

    return sock_address(std::string(address), ntohs(addr.sin6_port),
            addr.sin6_family == AF_INET6 ? sock_address::ip_family::inet6 : sock_address::ip_family::inet4);
}

void server_sock::blocking(bool value) {
#ifdef WIN32
    /// @note windows sockets are created in blocking mode by default
    // currently on windows, there is no easy way to obtain the socket's current blocking mode since WSAIsBlocking was deprecated
    u_long flags = value ? 0 : 1;
    ioctlsocket(m_sockfd, FIONBIO, &flags);
    m_blocking = value;
#else
    const int flags = fcntl(m_sockfd, F_GETFL, 0);
    fcntl(m_sockfd, F_SETFL, value ? flags ^ O_NONBLOCK : flags | O_NONBLOCK);
#endif

}

bool server_sock::blocking() {
#ifdef WIN32
    return m_blocking;
#else

    const int flags = fcntl(m_sockfd, F_GETFL, 0);
    return !(flags & O_NONBLOCK);
#endif
}

server_sock::~server_sock() {
    close();
}

