#ifndef STREAMS_SOCK_HPP_
#define STREAMS_SOCK_HPP_

#include <cstring>
#include <fstream>

#include "../net/sock.hpp"

#define STREAMS_SOCK_BUFFER_SIZE 1024

namespace stde::streams {

    /**
     * Socket streambuf
     */
    class sock_streambuf: public std::streambuf {
    public:
        /**
         * Constructor
         *
         * @param socket    Socket to read from / write to
         */
        sock_streambuf(const net::sock& socket);
        virtual std::streambuf::int_type underflow();
        virtual std::streambuf::int_type overflow(std::streambuf::int_type value);
        virtual int sync();
        virtual ~sock_streambuf();
    private:
        const net::sock& m_socket;
        char *m_inBuffer;
        char *m_outBuffer;
    };

    /**
     * socket input stream
     */
    class sock_istream: public std::istream {
    public:
        /**
         * Constructor
         *
         * @param sock  Socket to read from
         */
        sock_istream(const net::sock& socket) : std::istream(new sock_streambuf(socket)) {
        }

        virtual ~sock_istream() {
            delete rdbuf();
        }
    };

    /**
     * socket output stream
     */
    class sock_ostream: public std::ostream {
    public:
        /**
         * Constructor
         *
         * @param sock  Socket to write to
         */
        sock_ostream(const net::sock& socket) : std::ostream(new sock_streambuf(socket)) {
        }

        virtual ~sock_ostream() {
            delete rdbuf();
        }
    };

} /* namespace streams */

#endif /* STREAMS_SOCK_HPP_ */
