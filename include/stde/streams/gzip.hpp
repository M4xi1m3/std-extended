#ifndef STREAMS_GZIP_HPP_
#define STREAMS_GZIP_HPP_

#include <cstring>
#include <fstream>

#include <zlib.h>

#include "exceptions.hpp"

#define STREAMS_CHUNK_SIZE 8192

namespace stde::streams {

    /**
     * gzip streambuf
     */
    class gzip_streambuf: public std::streambuf {
    public:
        /**
         * Header type to be used for the stream
         *
         * zlib uses RFC1950
         * gzip uses RFC1952
         * unknown lets zlib detect the header type (only applicable to input)
         */
        enum header_types : int {
            zlib = 2, gzip = 1, unknown = 0
        };

        /**
         * Constructor
         *
         * @param buffer    Buffer to read from/write to
         */
        gzip_streambuf(std::streambuf* buffer, header_types type);
        virtual ~gzip_streambuf();
        virtual std::streambuf::int_type underflow();
        virtual std::streambuf::int_type overflow(std::streambuf::int_type value);
        virtual int sync();

        inline header_types type() const {
            switch (m_decompressHeader.done) {
                case 1:
                    return gzip;
                case -1:
                    return zlib;
                default:
                    return unknown;
            }
        }

    private:
        std::streambuf *m_buffer;
        char *m_decInBuffer;
        char *m_decOutBuffer;
        z_stream m_decompressStream;
        gz_header m_decompressHeader;

        char *m_cmpInBuffer;
        char *m_cmpOutBuffer;
        z_stream m_compressStream;
    };

    /**
     * gzip input stream
     *
     * reads and inflate a compressed stream
     */
    class gzip_istream: public std::istream {
    public:
        /**
         * Constructor
         *
         * @param stream    Stream to read and inflate from
         */
        gzip_istream(const std::istream& stream, gzip_streambuf::header_types type = gzip_streambuf::unknown) : std::istream(
                m_sb = new gzip_streambuf(stream.rdbuf(), type)) {
        }

        inline gzip_streambuf::header_types type() const {
            return m_sb->type();
        }

        virtual ~gzip_istream() {
            delete rdbuf();
        }
    private:
        gzip_streambuf *m_sb;
    };

    /**
     * gzip output stream
     *
     * writes and deflates an uncompressed stream
     */
    class gzip_ostream: public std::ostream {
    public:
        /**
         * Constructor
         *
         * @param stream    Stream to write deflated data to
         */
        gzip_ostream(const std::ostream& stream, gzip_streambuf::header_types type = gzip_streambuf::gzip) : std::ostream(
                m_sb = new gzip_streambuf(stream.rdbuf(), type)), m_type(type) {
        }

        inline gzip_streambuf::header_types type() const {
            return m_type;
        }

        virtual ~gzip_ostream() {
            delete rdbuf();
        }
    private:
        gzip_streambuf::header_types m_type;
        gzip_streambuf *m_sb;
    };

} /* namespace streams */

#endif /* STREAMS_GZIP_HPP_ */
