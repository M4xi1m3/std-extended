#ifndef NET_SERVERSOCK_HPP_
#define NET_SERVERSOCK_HPP_

#include <string>

#ifdef WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#define SERVERSOCK_SOCKET_T SOCKET
#define SERVERSOCK_INVALID SOCKET_ERROR
#else
#include <netdb.h>
#define SERVERSOCK_SOCKET_T int
#define SERVERSOCK_INVALID -1
#endif

#include "sock.hpp"
#include "sockaddress.hpp"

namespace stde {
    namespace net {

        /**
         * Represents a TCP server socket
         */
        class server_sock {
        public:
            /**
             * Create a socket from address and port, forcing it into SOCK_STREAM, using either IPv4 or IPv6.
             * After this, the socket is not bound and isn't listening
             *
             * @param address
             */
            server_sock(const sock_address& address);

            /**
             * Move constructor
             *
             * @param other Socket to move
             */
            server_sock(server_sock&& other);

            server_sock(server_sock& other) = delete;
            server_sock& operator=(const server_sock&) = delete;
            /**
             * Bind the server socket
             */
            void bind();

            /**
             * Start listening
             */
            void listen();

            /**
             * Accept a client
             * @return  The client socket
             */
            sock accept();

            /**
             * Closes the socket
             */
            void close();

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
             * Set socket to be blocking or not
             * @param value If the socket is blocking or not
             */
            void blocking(bool value);

            /**
             * Check if socket is blocking or not
             * @return  If the socket is blocking or not
             */
            bool blocking();

            /**
             * Get address of the socket
             * @return  Address of the socket
             */
            sock_address address();

            virtual ~server_sock();
        private:
            SERVERSOCK_SOCKET_T m_sockfd;
            struct addrinfo *m_addrinfo;
#ifdef WIN32
            bool m_blocking = false;
#endif
        };
    }
}

#endif
