#include "sock.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

using namespace streams;

sock_streambuf::sock_streambuf(const net::sock& socket) : m_socket(socket) {
    m_inBuffer = new char[STREAMS_SOCK_BUFFER_SIZE];
    m_outBuffer = new char[STREAMS_SOCK_BUFFER_SIZE];

    setg(0, 0, 0);
    setp(m_outBuffer, m_outBuffer + STREAMS_SOCK_BUFFER_SIZE);
}

int sock_streambuf::sync() {
    std::streambuf::int_type result = this->overflow(traits_type::eof());
    return traits_type::eq_int_type(result, traits_type::eof()) ? -1 : 0;
}

std::streambuf::int_type sock_streambuf::underflow() {

    int out = recv(m_socket.m_sockfd, m_inBuffer, STREAMS_SOCK_BUFFER_SIZE, 0);

    if (out <= 0) {
        return traits_type::eof();
    }

    setg(m_inBuffer, m_inBuffer, m_inBuffer + out);
    return traits_type::to_int_type(*gptr());
}

std::streambuf::int_type sock_streambuf::overflow(std::streambuf::int_type value) {

    int write_len = pptr() - pbase();

    if (write_len) {
        int tmp = write_len;
        while(tmp > 0) {
            int ret = send(m_socket.m_sockfd, m_outBuffer, write_len, 0);

            if (ret == -1) {
                return traits_type::eof();
            } else {
                tmp -= ret;
            }
        }
    }

    setp(m_outBuffer, m_outBuffer + STREAMS_SOCK_BUFFER_SIZE);
    if (!traits_type::eq_int_type(value, traits_type::eof()))
        sputc(value);
    return traits_type::not_eof(value);
}

sock_streambuf::~sock_streambuf() {
    sync();

    delete[] m_inBuffer;
    delete[] m_outBuffer;
}
