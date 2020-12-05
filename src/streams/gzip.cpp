#include "streams/gzip.hpp"

#include <assert.h>
#include "streams/exceptions.hpp"

using namespace stde::streams;

#include <iostream>

gzip_streambuf::gzip_streambuf(std::streambuf* buffer, header_types type) : m_buffer(buffer) {
    m_decInBuffer = new char[STREAMS_CHUNK_SIZE];
    m_decOutBuffer = new char[STREAMS_CHUNK_SIZE];
    setg(0, 0, 0);

    m_decompressStream.zalloc = Z_NULL;
    m_decompressStream.zfree = Z_NULL;
    m_decompressStream.opaque = Z_NULL;
    m_decompressStream.avail_in = 0;

    int res;
    int windowBits = MAX_WBITS + (type == gzip ? 16 : (type == zlib ? 0 : 32));

    if ((res = inflateInit2(&m_decompressStream, windowBits)) != Z_OK) {
        std::string message = "";
        message += "Couldn't initialize inflate stream: ";
        message += res;
        message += " ";
        message += m_decompressStream.msg;
        throw streams::gzip_exception(message);
    }

    m_decompressHeader.done = 0;
    inflateGetHeader(&m_decompressStream, &m_decompressHeader);

    m_cmpInBuffer = new char[STREAMS_CHUNK_SIZE];
    m_cmpOutBuffer = new char[STREAMS_CHUNK_SIZE];
    setp(m_cmpInBuffer, m_cmpInBuffer + STREAMS_CHUNK_SIZE);

    m_compressStream.zalloc = Z_NULL;
    m_compressStream.zfree = Z_NULL;
    m_compressStream.opaque = Z_NULL;

    windowBits = MAX_WBITS + (type == zlib ? 0 : 16);
    if ((res = deflateInit2(&m_compressStream, Z_DEFAULT_COMPRESSION, Z_DEFLATED, 16+MAX_WBITS, 8, Z_DEFAULT_STRATEGY)) != Z_OK) {
        std::string message = "";
        message += "Couldn't initialize deflate stream: ";
        message += res;
        message += " ";
        message += m_compressStream.msg;
        throw streams::gzip_exception(message);
    }
}

gzip_streambuf::~gzip_streambuf() {
    sync();

    inflateEnd(&m_decompressStream);
    deflateEnd(&m_compressStream);

    delete[] m_decInBuffer;
    delete[] m_decOutBuffer;

    delete[] m_cmpInBuffer;
    delete[] m_cmpOutBuffer;
}

std::streambuf::int_type gzip_streambuf::underflow() {
    // Provide more data to zlib if the output is empty.
    if (m_decompressStream.avail_in == 0) {
        // Read line from original buffer
        std::streamsize read = m_buffer->sgetn(m_decInBuffer, STREAMS_CHUNK_SIZE);
        if (!read) {
            inflateEnd(&m_decompressStream);
            return traits_type::eof();
        }

        m_decompressStream.avail_in = read;
        m_decompressStream.next_in = (unsigned char*) m_decInBuffer;
    }

    // Set output buffer
    m_decompressStream.avail_out = STREAMS_CHUNK_SIZE;
    m_decompressStream.next_out = (unsigned char*) m_decOutBuffer;

    // Do decompression
    int ret = inflate(&m_decompressStream, Z_NO_FLUSH);
    assert(ret != Z_STREAM_ERROR);

    // Check return.
    switch (ret) {
        case Z_NEED_DICT:
            ret = Z_DATA_ERROR;
            /* no break */
        case Z_DATA_ERROR:
        case Z_MEM_ERROR:
            inflateEnd(&m_decompressStream);
            std::string message = "";
            message += "Error while inflating: ";
            message += std::to_string(ret);
            message += " ";
            message += m_decompressStream.msg;

            throw streams::gzip_exception(message);
    }

    setg(m_decOutBuffer, m_decOutBuffer, m_decOutBuffer + STREAMS_CHUNK_SIZE - m_decompressStream.avail_out);
    return traits_type::to_int_type(*gptr());
}

std::streambuf::int_type gzip_streambuf::overflow(std::streambuf::int_type value) {
    int write = pptr() - pbase();

    if (write) {
        // Set input
        m_compressStream.avail_in = write;
        m_compressStream.next_in = (unsigned char*) m_cmpInBuffer;

        int flush = traits_type::eq_int_type(value, traits_type::eof()) ? Z_FINISH : Z_NO_FLUSH;

        do {
            // Set output
            m_compressStream.avail_out = STREAMS_CHUNK_SIZE;
            m_compressStream.next_out = (unsigned char*) m_cmpOutBuffer;

            int ret = deflate(&m_compressStream, flush);
            assert(ret != Z_STREAM_ERROR);

            std::streamsize written = m_buffer->sputn(m_cmpOutBuffer, STREAMS_CHUNK_SIZE - m_compressStream.avail_out);
            if (written != STREAMS_CHUNK_SIZE - m_compressStream.avail_out)
                return traits_type::eof();

        } while (m_compressStream.avail_out == 0);
    }

    setp(m_cmpInBuffer, m_cmpInBuffer + STREAMS_CHUNK_SIZE);
    if (!traits_type::eq_int_type(value, traits_type::eof()))
        sputc(value);
    return traits_type::not_eof(value);
}

int gzip_streambuf::sync() {
    std::streambuf::int_type result = this->overflow(traits_type::eof());
    m_buffer->pubsync();
    return traits_type::eq_int_type(result, traits_type::eof()) ? -1 : 0;
}
