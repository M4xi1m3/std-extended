#include "sock.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <cstring>
#include <system_error>

using namespace net;

sock::sock(const std::string& address, const std::string& port) {

    m_sockfd = -1;

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

    if (connect(m_sockfd, addrinfo->ai_addr, sizeof(*addrinfo->ai_addr)) != 0) {
        freeaddrinfo(addrinfo);
        throw std::system_error(errno, std::system_category(), strerror(errno));
    }

    freeaddrinfo(addrinfo);
}

void __close(int fd) {
    close(fd);
}

void sock::close() {
    if (m_sockfd != -1)
        __close(m_sockfd);
}

sock::sock(int sockfd) : m_sockfd(sockfd) {

}

sock::~sock() {
    close();
}
