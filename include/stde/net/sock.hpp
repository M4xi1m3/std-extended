#ifndef NET_SOCK_HPP_
#define NET_SOCK_HPP_

#include <string>
#include <netdb.h>

#include "sockaddress.hpp"

namespace stde::streams {
    class sock_streambuf;
}

namespace stde::net {

    class server_sock;

    /**
     * Represents a TCP socket
     */
    class sock {
        friend class streams::sock_streambuf;
        friend class net::server_sock;

    public:
        /**
         * Create a socket from address and port, forcing it into SOCK_STREAM.
         * After this, the socket is ready to connect. If the socket couldn't be created, an exception is raised.
         *
         * @param address   Address to connect to
         */
        sock(const sock_address& address);

        /**
         * Close the socket
         */
        void close();

        /**
         * Connects the socket.
         */
        void connect();

        /**
         * Enable/disable the SO_KEEPALIVE socket option
         * @param value Value for SO_KEEPALIVE
         */
        void keep_alive(bool value);

        /**
         * Get value of the SO_KEEPALIVE socket option
         * @return  The value of SO_KEEPALIVE
         */
        bool keep_alive();

        /**
         * Enable/disable the SO_REUSEADDR socket option
         * @param value Value for SO_REUSEADDR
         */
        void reuse_address(bool value);

        /**
         * Get value of the SO_REUSEADDR socket option
         * @return  The value of SO_REUSEADDR
         */
        bool reuse_address();

        /**
         * Enable/disable the TCP_NODELAY socket option
         * @param value Value for TCP_NODELAY
         */
        void tcp_no_delay(bool value);

        /**
         * Get value of the TCP_NODELAY socket option
         * @return  The value of TCP_NODELAY
         */
        bool tcp_no_delay();

        /**
         * Get address of the peer we're connected to.
         * @return  Address of the peer
         */
        sock_address peer_address();

        /**
         * Get address of the socket
         * @return  Address of the socket
         */
        sock_address address();

        virtual ~sock();
    private:
        sock(int sockfd);
        int m_sockfd;
        struct addrinfo *m_addrinfo;
    };

} /* namespace net */

#endif /* NET_SOCK_HPP_ */
