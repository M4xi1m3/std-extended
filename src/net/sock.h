#ifndef NET_SOCK_H_
#define NET_SOCK_H_

#include <string>

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
         * Create a socket from address and port, forcing it into SOCK_STREAM, using either IPv4 or IPv6, and calls connect.
         * After this, the socket is ready and connected. If the socket couldn't be created or failed to connect, an exception is raised.
         *
         * @param address   Address to connect to
         * @param port      Port to connect to
         */
        sock(const std::string& address, const std::string& port);
        void close();
        virtual ~sock();
    private:
        sock(int sockfd);
        int m_sockfd;
    };

} /* namespace net */

#endif /* NET_SOCK_H_ */
