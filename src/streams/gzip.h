#ifndef STREAMS_GZIP_H_
#define STREAMS_GZIP_H_

#include <cstring>
#include <fstream>

#include <zlib.h>

#include "exceptions.h"

#define STREAMS_CHUNK_SIZE 1024

namespace streams {

    /**
     * gzip streambuf
     */
    class gzip_streambuf: public std::streambuf {
    public:
        /**
         * Constructor
         *
         * @param buffer    Buffer to read from/write to
         */
        gzip_streambuf(std::streambuf* buffer);
        virtual ~gzip_streambuf();
        virtual std::streambuf::int_type underflow();
        virtual std::streambuf::int_type overflow(std::streambuf::int_type value);
        virtual int sync();

    private:
        std::streambuf *m_buffer;
        char *m_decInBuffer;
        char *m_decOutBuffer;
        z_stream m_decompressStream;
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
        gzip_istream(std::istream& stream) : std::istream(new gzip_streambuf(stream.rdbuf())) {
        }

        virtual ~gzip_istream() {
            delete rdbuf();
        }
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
        gzip_ostream(std::ostream& stream) : std::ostream(new gzip_streambuf(stream.rdbuf())) {
        }

        virtual ~gzip_ostream() {
            delete rdbuf();
        }
    };

} /* namespace streams */

#endif /* STREAMS_GZIP_H_ */
