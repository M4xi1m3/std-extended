#ifndef NET_SOCK_H_
#define NET_SOCK_H_

#include <string>
#include <netdb.h>

#include "sockaddress.h"

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
        void setKeepAlive(bool value);

        /**
         * Get value of the SO_KEEPALIVE socket option
         * @return  The value of SO_KEEPALIVE
         */
        bool getKeepAlive();

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

        /**
         * Enable/disable the TCP_NODELAY socket option
         * @param value Value for TCP_NODELAY
         */
        void setTcpNoDelay(bool value);

        /**
         * Get value of the TCP_NODELAY socket option
         * @return  The value of TCP_NODELAY
         */
        bool getTcpNoDelay();

        /**
         * Get address of the peer we're connected to.
         * @return  Address of the peer
         */
        sock_address getPeerAddress();

        /**
         * Get address of the socket
         * @return  Address of the socket
         */
        sock_address getSockAddress();

        virtual ~sock();
    private:
        sock(int sockfd);
        int m_sockfd;
        struct addrinfo *m_addrinfo;
    };

} /* namespace net */

#endif /* NET_SOCK_H_ */
