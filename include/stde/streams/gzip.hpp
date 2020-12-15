#ifndef STREAMS_GZIP_HPP_
#define STREAMS_GZIP_HPP_

#include <cstring>
#include <fstream>

#include "exceptions.hpp"

#define STREAMS_CHUNK_SIZE 8192

namespace stde {
    namespace streams {

        /**
         * gzip streambuf
         */
        class gzip_streambuf: public std::streambuf {
            class impl;
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

            header_types type() const;

        private:
            std::streambuf *m_buffer;

            impl* m_imp;
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
    }
}

#endif
