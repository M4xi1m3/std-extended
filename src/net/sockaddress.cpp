#include "net/sockaddress.hpp"

#include <cstring>
#include <stdexcept>
#include <system_error>

#ifdef WIN32
#include <ws2tcpip.h>
#define ERROR_CODE WSAGetLastError()
#else
#define ERROR_CODE errno
#endif

#ifdef __MINGW32__
// This is needed because mingw doesn't include it.
static int inet_pton(int af, const char* src, void* dst) {
    struct sockaddr_storage ss;
    int size = sizeof(ss);
    char src_copy[INET6_ADDRSTRLEN + 1];

    ZeroMemory(&ss, sizeof(ss));
    /* stupid non-const API */
    strncpy(src_copy, src, INET6_ADDRSTRLEN + 1);
    src_copy[INET6_ADDRSTRLEN] = 0;

    if (WSAStringToAddress(src_copy, af, NULL, (struct sockaddr*) &ss, &size) == 0) {
        switch (af) {
            case AF_INET:
                *(struct in_addr*) dst = ((struct sockaddr_in*) &ss)->sin_addr;
                return 1;
            case AF_INET6:
                *(struct in6_addr*) dst = ((struct sockaddr_in6*) &ss)->sin6_addr;
                return 1;
        }
    }
    return 0;
}

static const char* inet_ntop(int af, const void* src, char* dst, socklen_t size) {
    struct sockaddr_storage ss;
    unsigned long s = size;

    ZeroMemory(&ss, sizeof(ss));
    ss.ss_family = af;

    switch (af) {
        case AF_INET:
            ((struct sockaddr_in*) &ss)->sin_addr = *(struct in_addr*) src;
            break;
        case AF_INET6:
            ((struct sockaddr_in6*) &ss)->sin6_addr = *(struct in6_addr*) src;
            break;
        default:
            return NULL;
    }
    /* cannot direclty use &size because of strict aliasing rules */
    return (WSAAddressToString((struct sockaddr*) &ss, sizeof(ss), NULL, dst, &s) == 0) ? dst : NULL;
}

#endif

using namespace stde::net;

sock_address::sock_address(const std::string& address) {
    m_port = 0;
    m_family = ip_family::undef;
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
        m_family = ip_family::inet6;
    }

    // Call getaddrinfo to do domain name resolution and stuff
    doGetaddrInfo(addr_part, service_part, m_family);
}

static void* get_in_addr(struct sockaddr* sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*) sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*) sa)->sin6_addr);
}

void sock_address::doGetaddrInfo(const std::string& address, const std::string& service, ip_family family) {
    struct addrinfo *info = nullptr;

    struct addrinfo hints;

    memset(&hints, 0, sizeof hints);
    switch (family) {
        case ip_family::inet4:
            hints.ai_family = AF_INET;
            break;
        case ip_family::inet6:
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

    m_family = info->ai_family == AF_INET6 ? ip_family::inet6 : ip_family::inet4;
    m_port = ntohs(((sockaddr_in6*) info->ai_addr)->sin6_port);

    char tmpaddress[INET6_ADDRSTRLEN];

    inet_ntop(info->ai_family, get_in_addr((struct sockaddr*) info->ai_addr), tmpaddress, sizeof tmpaddress);

    m_address = std::string(tmpaddress);

    freeaddrinfo(info);
}

sock_address::sock_address(const std::string& address, const std::string& service) {
    doGetaddrInfo(address, service, ip_family::undef);
}

sock_address::sock_address(const std::string& address, uint16_t port, ip_family family) : m_address(address), m_port(port), m_family(family) {

    int out = inet_pton(family == ip_family::inet6 ? AF_INET6 : AF_INET, address.c_str(), &m_sockaddr);
    // int out = 1;

    if (out == 0) {
        throw std::invalid_argument("address doesn't represent a valid address in the specified family");
    } else if (out == -1) {
        throw std::system_error(ERROR_CODE, std::system_category(), "inet_pton");
    }
}

sock_address::~sock_address() {
}

std::ostream& operator<<(std::ostream& stream, const stde::net::sock_address& addr) {
    if (addr.family() == sock_address::ip_family::inet6) {
        return stream << "[" << addr.address() << "]:" << addr.port();
    } else {
        return stream << addr.address() << ":" << addr.port();
    }
}
