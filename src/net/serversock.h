#ifndef NET_SERVERSOCK_H_
#define NET_SERVERSOCK_H_

#include <string>
#include <netdb.h>

#include "sock.h"

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
         * @param port
         */
        server_sock(const std::string& address, const std::string& port);
        virtual ~server_sock();

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
         * Enable/disable the SO_REUSEADDR socket option
         * @param value Value for SO_REUSEADDR
         */
        void setReuseAddress(bool value);

        /**
         * Get value of the SO_REUSEADDR socket option
         * @return  The value of SO_REUSEADDR
         */
        bool getReuseAddress();

    private:
        int m_sockfd;
        struct addrinfo *m_addrinfo;
    };

} /* namespace net */

#endif /* NET_SERVERSOCK_H_ */
