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
    struct addrinfo *addrinfo;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;     // IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // TCP

    int status;
    if ((status = getaddrinfo(address.c_str(), port.c_str(), &hints, &addrinfo)) != 0) {
        freeaddrinfo(addrinfo);
        throw std::system_error(status, std::system_category(), gai_strerror(status));
    }

    m_sockfd = socket(addrinfo->ai_family, addrinfo->ai_socktype, addrinfo->ai_protocol);
    if (m_sockfd == -1) {
        freeaddrinfo(addrinfo);
        throw std::system_error(errno, std::system_category(), strerror(errno));
    }

    if (bind(m_sockfd, addrinfo->ai_addr, sizeof(*addrinfo->ai_addr)) != 0) {
        freeaddrinfo(addrinfo);
        throw std::system_error(errno, std::system_category(), strerror(errno));
    }

    freeaddrinfo(addrinfo);

    if (listen(m_sockfd, 0) != 0) {
        throw std::system_error(errno, std::system_category(), strerror(errno));
    }
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

server_sock::~server_sock() {
    close(m_sockfd);
}

