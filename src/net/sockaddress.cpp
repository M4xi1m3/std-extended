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

sock_address::sock_address(const std::string& address) {
    m_port = 0;
    m_family = undef;
    m_address = "";

    size_t split;
    std::string addr_part;
    std::string service_part;

    // Check for ]: for ipv6 formatted as [ip]:port
    split = address.rfind("]:");
    if (split == address.npos) {
        // Not fount, ipv4
        // Find : the port separator
        split = address.rfind(":");

        // Not found
        if (split == address.npos) {
            throw std::invalid_argument("wrongly formatted address");
        }

        addr_part = std::string(address.substr(0, split));
        service_part = std::string(address.substr(split + 1));
    } else {
        // Check if ipv6 starts with [
        if (address[0] != '[') {
            throw std::invalid_argument("wrongly formatted address");
        }

        addr_part = std::string(address.substr(1, split - 1));
        service_part = std::string(address.substr(split + 2));
        m_family = inet6;
    }

    // Call getaddrinfo to do domain name resolution and stuff
    doGetaddrInfo(addr_part, service_part, m_family);
}

void sock_address::doGetaddrInfo(const std::string& address, const std::string& service, family family) {

    struct addrinfo *info = nullptr;

    struct addrinfo hints;

    memset(&hints, 0, sizeof hints);
    switch (family) {
        case inet4:
            hints.ai_family = AF_INET;
            break;
        case inet6:
            hints.ai_family = AF_INET6;
            break;
        default:
            hints.ai_family = AF_UNSPEC;
            break;
    }

    int status;
    if ((status = getaddrinfo(address.c_str(), service.c_str(), &hints, &info)) != 0) {
        info = nullptr;
        throw std::system_error(status, std::system_category(), gai_strerror(status));
    }

    m_family = info->ai_family == AF_INET6 ? inet6 : inet4;
    m_port = ntohs(((sockaddr_in6*) info->ai_addr)->sin6_port);

    char tmpaddress[INET6_ADDRSTRLEN];
    inet_ntop(info->ai_family, get_in_addr((struct sockaddr*) info->ai_addr), tmpaddress, sizeof tmpaddress);

    m_address = std::string(tmpaddress);

    freeaddrinfo(info);
}

sock_address::sock_address(const std::string& address, const std::string& service) {
    doGetaddrInfo(address, service, undef);
}

sock_address::sock_address(const std::string& address, uint16_t port, family familly) : m_address(std::move(address)), m_port(port), m_family(familly) {
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

std::ostream& operator<<(std::ostream& stream, const net::sock_address& addr) {
    if (addr.getFamilly() == sock_address::inet6) {
        return stream << "[" << addr.getAddress() << "]:" << addr.getPort();
    } else {
        return stream << addr.getAddress() << ":" << addr.getPort();
    }
}
