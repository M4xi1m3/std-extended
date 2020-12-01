#ifndef NET_SOCKADDRESS_H_
#define NET_SOCKADDRESS_H_

#include <string>
#include <netinet/in.h>
#include <ostream>

namespace net {

    class sock_address {
    public:
        enum familly {
            inet4, inet6
        };

        sock_address(const std::string& address, uint16_t port, familly familly);
        virtual ~sock_address();
        std::string getAddress() const;
        uint16_t getPort() const;
        familly getFamilly() const;

    private:
        struct sockaddr_in6 m_sockaddr;
        std::string m_address;
        uint16_t m_port;
        familly m_familly;
    };

} /* namespace net */


std::ostream& operator<<(std::ostream& stream, const net::sock_address& addr);

#endif /* NET_SOCKADDRESS_H_ */
