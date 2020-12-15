#ifndef STREAMS_EXCEPTIONS_HPP_
#define STREAMS_EXCEPTIONS_HPP_

#include <exception>

namespace stde {
    namespace streams {

        /**
         * Stream-related exception
         */
        class stream_exception: public std::exception {
        public:
            /**
             * Constructor
             *
             * @param message   Message associated with the exception.
             */
            stream_exception(const std::string& message) : std::exception(), m_message(message) {
            }
            virtual const char* what() const noexcept {
                return m_message.c_str();
            }
        private:
            std::string m_message;
        };

        /**
         * Thrown when trying to read from a stream which has reached its end
         */
        class eof_exception: public stream_exception {
        public:
            /**
             * Constructor
             *
             * @param message   Message associated with the exception.
             */
            eof_exception(const std::string& message) : stream_exception(message) {
            }
        };

        /**
         * Gzip-related exception. Thrown when failing to initialize zlib or when zlib returns an error while inflating/deflating
         */
        class gzip_exception: public stream_exception {
        public:
            /**
             * Constructor
             *
             * @param message   Message associated with the exception.
             */
            gzip_exception(const std::string& message) : stream_exception(message) {
            }
        };
    }
}

#endif
