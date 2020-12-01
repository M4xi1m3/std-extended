#include "sockaddress.h"

#include <arpa/inet.h>

#include <cstring>
#include <stdexcept>
#include <system_error>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

using namespace net;

static void* get_in_addr(struct sockaddr* sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*) sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*) sa)->sin6_addr);
}

sock_address::sock_address(const std::string& address, const std::string& service) {

    struct addrinfo *info;

    struct addrinfo hints;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;

    int status;
    if ((status = getaddrinfo(address.c_str(), service.c_str(), &hints, &info)) != 0) {
        freeaddrinfo(info);
        throw std::system_error(status, std::system_category(), gai_strerror(status));
    }

    m_familly = info->ai_family == AF_INET6 ? inet6 : inet4;
    m_port = ntohs(((sockaddr_in6*)info->ai_addr)->sin6_port);

    char tmpaddress[INET6_ADDRSTRLEN];
    inet_ntop(info->ai_family, get_in_addr((struct sockaddr*) info->ai_addr), tmpaddress, sizeof tmpaddress);

    m_address = std::string(tmpaddress);

    freeaddrinfo(info);
}

sock_address::sock_address(const std::string& address, uint16_t port, family familly) : m_address(std::move(address)), m_port(port), m_familly(familly) {
    int out = inet_pton(familly == inet6 ? AF_INET6 : AF_INET, address.c_str(), &m_sockaddr);

    if (out == 0) {
        throw std::invalid_argument("address doesn't represent a valid address in the specified family");
    } else if (out == -1) {
        throw std::system_error(errno, std::system_category(), "inet_pton");
    }
}

sock_address::~sock_address() {
    // TODO Auto-generated destructor stub
}

sock_address::family sock_address::getFamilly() const {
    return m_familly;
}

std::string sock_address::getAddress() const {
    return m_address;
}

uint16_t sock_address::getPort() const {
    return m_port;
}

std::ostream& operator<<(std::ostream& stream, const net::sock_address& addr) {
    if (addr.getFamilly() == sock_address::inet6) {
        return stream << "[" << addr.getAddress() << "]:" << addr.getPort();
    } else {
        return stream << addr.getAddress() << ":" << addr.getPort();
    }
}
