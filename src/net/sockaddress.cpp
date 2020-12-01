#include "sockaddress.h"

using namespace net;

sock_address::sock_address(const std::string& address, uint16_t port) : m_address(std::move(address)), m_port(port) {
    // TODO Auto-generated constructor stub

}

sock_address::~sock_address() {
    // TODO Auto-generated destructor stub
}

std::string net::sock_address::getAddress() const {
    return m_address;
}

uint16_t net::sock_address::getPort() const {
    return m_port;
}
