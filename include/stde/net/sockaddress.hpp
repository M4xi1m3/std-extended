#ifndef NET_SOCKADDRESS_HPP_
#define NET_SOCKADDRESS_HPP_

#ifdef WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#else
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#endif

#include <string>
#include <ostream>

namespace stde {
    namespace net {

        /**
         * Represents a socket address
         */
        class sock_address {
        public:
            /**
             * Represents an address family, either IPv4 or IPv6
             */
            enum class ip_family {
                inet4, inet6, undef
            };

            /**
             * Creates a socket address from a single string, formatted as address:port, or [address]:port for IPv6
             *
             * Parsing this string is done in two different steps :
             *  - Separating the address and port part (finding the last ":" and splitting)
             *  - Removing the [] arround the IPv6 (if present)
             *
             * @param address
             */
            sock_address(const std::string& address);

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
            sock_address(const std::string& address, uint16_t port, ip_family family);
            virtual ~sock_address();

            /**
             * Get the IP of the socket address
             * @return  The IP
             */
            inline std::string address() const {
                return m_address;
            }

            /**
             * Get the port of the socket address
             * @return  The port
             */
            inline uint16_t port() const {
                return m_port;
            }

            /**
             * Get the family of the socket address
             * @return  The family
             */
            inline ip_family family() const {
                return m_family;
            }

        private:
            void doGetaddrInfo(const std::string& address, const std::string& service, ip_family family);
            std::string m_address;
            uint16_t m_port;
            ip_family m_family;
            struct sockaddr_in6 m_sockaddr;
        };
    }
}

/**
 * Prints a socket address on a stream
 * For IPv4, it prints <ip>:<<port>
 * For IPv6, it prints [<ip>]:<port>
 *
 * @param stream    Stream to print to
 * @param addr      Socket address to print
 * @return          stream
 */
std::ostream& operator<<(std::ostream& stream, const stde::net::sock_address& addr);

#endif
