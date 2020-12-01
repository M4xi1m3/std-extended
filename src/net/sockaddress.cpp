#include "sockaddress.h"

#include <arpa/inet.h>

#include <cstring>
#include <stdexcept>
#include <system_error>

using namespace net;

sock_address::sock_address(const std::string& address, uint16_t port, familly familly) : m_address(std::move(address)), m_port(port), m_familly(familly) {
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

sock_address::familly sock_address::getFamilly() const {
    return m_familly;
}

std::string sock_address::getAddress() const {
    return m_address;
}

uint16_t sock_address::getPort() const {
    return m_port;
}

std::ostream& operator<< (std::ostream& stream, const net::sock_address& addr) {
    if (addr.getFamilly() == sock_address::inet6) {
        return stream << "[" << addr.getAddress() << "]:" << addr.getPort();
    } else {
        return stream << addr.getAddress() << ":" << addr.getPort();
    }
}
