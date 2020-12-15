#ifndef STREAMS_DATA_HPP_
#define STREAMS_DATA_HPP_

#include <cstring>
#include <fstream>

#include "endian.hpp"

namespace stde {
    namespace streams {

        /**
         * Stream used to deserialize data
         */
        class data_istream: public std::istream {
        public:
            /**
             * Constructor
             * @param stream        Stream to read from
             * @param endianness    Endianness if the input
             */
            data_istream(const std::istream& stream, endianconv::endian endianness);

            /**
             * Reads a boolean from the stream
             * @return  The read boolean
             */
            bool read_bool();

            /**
             * Reads a byte from the stream
             * @return  The read byte
             */
            int8_t read_byte();

            /**
             * Reads an unsigned byte from the stream
             * @return  The read unsigned byte
             */
            uint8_t read_ubyte();

            /**
             * Reads a short from the stream
             * @return  The read short
             */
            int16_t read_short();

            /**
             * Reads an unsigned short from the stream
             * @return  The read short
             */
            uint16_t read_ushort();

            /**
             * Reads an int from the stream
             * @return  The read int
             */
            int32_t read_int();

            /**
             * Reads an unsigned int from the stream
             * @return  The read unsigned int
             */
            uint32_t read_uint();

            /**
             * Reads a long from the stream
             * @return  The read long
             */
            int64_t read_long();

            /**
             * Reads an unsigned long from the stream
             * @return  The read unsigned long
             */
            uint64_t read_ulong();

            /**
             * Reads a float from the stream
             * @return  The read float
             */
            float read_float();

            /**
             * Reads an double from the stream
             * @return  The read double
             */
            double read_double();

            /**
             * Reads a string from the stream
             *
             * Read as a short, containing the length, then the content, not null terminated.
             *
             * @return  The read string
             */
            std::string read_string();
        private:
            endianconv::endian m_endianness;
        };

        /**
         * Stream used to serialize data
         */
        class data_ostream: public std::ostream {
        public:
            /**
             * Constructor
             * @param stream        Stream to write to
             * @param endianness    Endianness of the output
             */
            data_ostream(const std::ostream& stream, endianconv::endian endianness);

            /**
             * Writes a boolean
             * @param value The boolean to write
             */
            void write_bool(bool value);

            /**
             * Writes a byte
             * @param value The byte to write
             */
            void write_byte(int8_t value);

            /**
             * Writes an unsigned byte
             * @param value The unsigned byte to write
             */
            void write_ubyte(uint8_t value);

            /**
             * Writes a short
             * @param value The short to write
             */
            void write_short(int16_t value);

            /**
             * Writes an unsigned short
             * @param value The unsigned short to write
             */
            void write_ushort(uint16_t value);

            /**
             * Writes an int
             * @param value The int to write
             */
            void write_int(int32_t value);

            /**
             * Writes an unsigned int
             * @param value The int to write
             */
            void write_uint(uint32_t value);

            /**
             * Writes a long
             * @param value The long to write
             */
            void write_long(int64_t value);

            /**
             * Writes an unsigned long
             * @param value The unsigned long to write
             */
            void write_ulong(uint64_t value);

            /**
             * Writes a float
             * @param value The float to write
             */
            void write_float(float value);

            /**
             * Writes a double
             * @param value The double to write
             */
            void write_double(double value);

            /**
             * Writes a string
             *
             * Written as a short containing the length then the data, without null terminator.
             *
             * @param value The string to write
             */
            void write_string(const std::string& value);
        private:
            endianconv::endian m_endianness;
        };
    }
}

#endif
