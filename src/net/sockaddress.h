#ifndef NET_SOCKADDRESS_H_
#define NET_SOCKADDRESS_H_

#include <string>

namespace net {

    class sock_address {
    public:
        sock_address(const std::string& address, uint16_t port);
        virtual ~sock_address();
        std::string getAddress() const;
        uint16_t getPort() const;

    private:
        std::string m_address;
        uint16_t m_port;
    };

} /* namespace net */

#endif /* NET_SOCKADDRESS_H_ */
