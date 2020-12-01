#include "serversock.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <cstring>
#include <system_error>

using namespace net;

server_sock::server_sock(const std::string& address, const std::string& port) {
    struct addrinfo hints;
    m_addrinfo = nullptr;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;     // IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // TCP

    int status;
    if ((status = getaddrinfo(address.c_str(), port.c_str(), &hints, &m_addrinfo)) != 0) {
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

void __listen(int sockfd) {
    if (listen(sockfd, 0) != 0) {
        throw std::system_error(errno, std::system_category(), strerror(errno));
    }
}

void server_sock::listen() {
    __listen(m_sockfd);
}

void __bind(int sockfd, struct addrinfo **addrinfo) {

    if (*addrinfo == nullptr) {
        throw std::runtime_error("Socket's addrinfo already freed!");
    }

    if (bind(sockfd, (*addrinfo)->ai_addr, sizeof(*(*addrinfo)->ai_addr)) != 0) {
        freeaddrinfo(*addrinfo);
        *addrinfo = nullptr;
        throw std::system_error(errno, std::system_category(), strerror(errno));
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
        throw std::system_error(errno, std::system_category(), strerror(errno));
    }

    sock client(client_sockfd);
    return client;
}

void server_sock::setReuseAddress(bool value) {
    int val = value ? 1 : 0;

    if (setsockopt(m_sockfd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) != 0) {
        throw std::system_error(errno, std::system_category(), strerror(errno));
    }
}

bool server_sock::getReuseAddress() {
    int out = 0;
    socklen_t len = sizeof(out);

    if (getsockopt(m_sockfd, SOL_SOCKET, SO_REUSEADDR, &out, &len) != 0) {
        throw std::system_error(errno, std::system_category(), strerror(errno));
    }

    return out == 0 ? false : true;
}

server_sock::~server_sock() {
    close(m_sockfd);
}

