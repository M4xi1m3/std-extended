#include "net/serversock.hpp"

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <cstring>
#include <system_error>

using namespace stde::net;

server_sock::server_sock(const sock_address& address) {
    m_sockfd = -1;

    struct addrinfo hints;
    m_addrinfo = nullptr;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = address.getFamilly() == sock_address::inet6 ? AF_INET6 : AF_INET;
    hints.ai_socktype = SOCK_STREAM; // TCP
    hints.ai_flags = AI_PASSIVE;     // Local address

    char buf[10];
    snprintf(buf, 10, "%u", address.getPort());

    int status;
    if ((status = getaddrinfo(address.getAddress().c_str(), buf, &hints, &m_addrinfo)) != 0) {
        m_addrinfo = nullptr;
        throw std::system_error(status, std::system_category(), gai_strerror(status));
    }

    m_sockfd = socket(m_addrinfo->ai_family, m_addrinfo->ai_socktype, m_addrinfo->ai_protocol);
    if (m_sockfd == -1) {
        freeaddrinfo(m_addrinfo);
        m_addrinfo = nullptr;
        throw std::system_error(errno, std::system_category(), "socket");
    }
}

static void __listen(int sockfd) {
    if (listen(sockfd, 0) != 0) {
        throw std::system_error(errno, std::system_category(), "listen");
    }
}

void server_sock::listen() {
    __listen(m_sockfd);
}

static void __bind(int sockfd, struct addrinfo** addrinfo) {

    if (*addrinfo == nullptr) {
        throw std::runtime_error("Socket's addrinfo already freed!");
    }

    if (bind(sockfd, (*addrinfo)->ai_addr, (*addrinfo)->ai_family == AF_INET6 ? sizeof(sockaddr_in6) : sizeof(sockaddr_in)) != 0) {
        freeaddrinfo(*addrinfo);
        *addrinfo = nullptr;
        throw std::system_error(errno, std::system_category(), "bind");
    }

    freeaddrinfo(*addrinfo);
    *addrinfo = nullptr;
}

void server_sock::bind() {
    __bind(m_sockfd, &m_addrinfo);
}

int __accept(int sockfd) {
    return accept(sockfd, nullptr, nullptr);
}

sock server_sock::accept() {
    int client_sockfd = __accept(m_sockfd);

    if (client_sockfd == -1) {
        throw std::system_error(errno, std::system_category(), "accept");
    }

    sock client(client_sockfd);
    return client;
}

void server_sock::setReuseAddress(bool value) {
    int val = value ? 1 : 0;

    if (setsockopt(m_sockfd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) != 0) {
        throw std::system_error(errno, std::system_category(), "setsockopt");
    }
}

bool server_sock::getReuseAddress() {
    int out = 0;
    socklen_t len = sizeof(out);

    if (getsockopt(m_sockfd, SOL_SOCKET, SO_REUSEADDR, &out, &len) != 0) {
        throw std::system_error(errno, std::system_category(), "getsockopt");
    }

    return out == 0 ? false : true;
}

static void __close(int fd) {
    close(fd);
}

void server_sock::close() {
    if (m_sockfd != -1) {
        __close(m_sockfd);
        m_sockfd = -1;
    }

    if (m_addrinfo != nullptr) {
        freeaddrinfo(m_addrinfo);
        m_addrinfo = nullptr;
    }
}

static void* get_in_addr(struct sockaddr* sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*) sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*) sa)->sin6_addr);
}

sock_address server_sock::getAddress() {
    struct sockaddr_in6 addr;

    socklen_t len;

    char address[INET6_ADDRSTRLEN];
    len = sizeof(addr);
    getsockname(m_sockfd, (struct sockaddr*) &addr, &len);

    inet_ntop(addr.sin6_family, get_in_addr((struct sockaddr*) &addr), address, sizeof address);

    return sock_address(std::string(address), ntohs(addr.sin6_port), addr.sin6_family == AF_INET6 ? sock_address::inet6 : sock_address::inet4);
}

server_sock::~server_sock() {
    close();
}

