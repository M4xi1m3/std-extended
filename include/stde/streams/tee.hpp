#ifndef STDE_STREAMS_TEE_HPP_
#define STDE_STREAMS_TEE_HPP_

#include <streambuf>
#include <ostream>

namespace stde {
    namespace streams {
        class tee_streambuf: public std::streambuf {
        public:
            tee_streambuf(std::streambuf* sb1, std::streambuf* sb2);

            virtual ~tee_streambuf();
        private:
            virtual int overflow(int c);
            virtual int sync();
        private:
            std::streambuf *m_sb1;
            std::streambuf *m_sb2;

        };

        class tee_ostream: public std::ostream {
        public:
            /**
             * Constructor
             *
             * @param stream    Stream to write deflated data to
             */
            tee_ostream(const std::ostream& stream, const std::ostream& stream2) : std::ostream(m_sb = new tee_streambuf(stream.rdbuf(), stream2.rdbuf())) {
            }
        private:
            tee_streambuf *m_sb;
        };

    }
}

#endif
