#ifndef NET_SOCKADDRESS_H_
#define NET_SOCKADDRESS_H_

#include <string>
#include <netinet/in.h>
#include <ostream>

namespace net {

    class sock_address {
    public:
        /**
         * Represents an address family, either IPv4 or IPv6
         */
        enum family {
            inet4, inet6
        };

        /**
         * Creates a socket address from a domain and a service
         * @param address   Domain or ip address
         * @param service   Service, either textual or a port
         */
        sock_address(const std::string& address, const std::string& service);

        /**
         * Create a socket address from an IP, a port and a family
         * @param address   IP of the socket address
         * @param port      Port of the socket address
         * @param family    Family of the created address
         */
        sock_address(const std::string& address, uint16_t port, family family);
        virtual ~sock_address();

        /**
         * Get the IP of the socket address
         * @return  The IP
         */
        std::string getAddress() const;

        /**
         * Get the port of the socket address
         * @return  The port
         */
        uint16_t getPort() const;

        /**
         * Get the family of the socket address
         * @return  The family
         */
        family getFamilly() const;

    private:
        struct sockaddr_in6 m_sockaddr;
        std::string m_address;
        uint16_t m_port;
        family m_familly;
    };

} /* namespace net */

/**
 * Prints a socket address on a stream
 * For IPv4, it prints <ip>:<<port>
 * For IPv6, it prints [<ip>]:<port>
 *
 * @param stream    Stream to print to
 * @param addr      Socket address to print
 * @return          stream
 */
std::ostream& operator<<(std::ostream& stream, const net::sock_address& addr);

#endif /* NET_SOCKADDRESS_H_ */
