#ifndef NET_SOCK_HPP_
#define NET_SOCK_HPP_

#include <string>

#ifdef WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#define SOCK_SOCKET_T SOCKET
#define SOCK_INVALID SOCKET_ERROR
#else
#include <netdb.h>
#define SOCK_SOCKET_T int
#define SOCK_INVALID -1
#endif

#include "sockaddress.hpp"

namespace stde {
    namespace streams {
        class sock_streambuf;
    }

    namespace net {

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
             * Move constructor
             *
             * @param other Socket to move
             */
            sock(sock&& other);

            sock(sock& other) = delete;
            sock& operator=(const sock&) = delete;

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
            sock(SOCK_SOCKET_T sockfd);
            SOCK_SOCKET_T m_sockfd;
            struct addrinfo *m_addrinfo;
        };
    }
}

#endif
