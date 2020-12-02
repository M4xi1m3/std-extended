#ifndef STREAMS_DATA_HPP_
#define STREAMS_DATA_HPP_

#include <cstring>
#include <fstream>

#include "endian.hpp"

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
        data_istream(std::istream& stream, endianconv::endian endianness);

        /**
         * Reads a boolean from the stream
         * @return  The read boolean
         */
        bool readBoolean();

        /**
         * Reads a byte from the stream
         * @return  The read byte
         */
        int8_t readByte();

        /**
         * Reads an unsigned byte from the stream
         * @return  The read unsigned byte
         */
        uint8_t readUnsignedByte();

        /**
         * Reads a short from the stream
         * @return  The read short
         */
        int16_t readShort();

        /**
         * Reads an unsigned short from the stream
         * @return  The read short
         */
        uint16_t readUnsignedShort();

        /**
         * Reads an int from the stream
         * @return  The read int
         */
        int32_t readInt();

        /**
         * Reads an unsigned int from the stream
         * @return  The read unsigned int
         */
        uint32_t readUnsignedInt();

        /**
         * Reads a long from the stream
         * @return  The read long
         */
        int64_t readLong();

        /**
         * Reads an unsigned long from the stream
         * @return  The read unsigned long
         */
        uint64_t readUnsignedLong();

        /**
         * Reads a float from the stream
         * @return  The read float
         */
        float readFloat();

        /**
         * Reads an double from the stream
         * @return  The read double
         */
        double readDouble();

        /**
         * Reads a string from the stream
         * @return  The read string
         */
        std::string readUTF();
    private:
        endianconv::endian m_endianness;
    };

    /**
     * Stream used to serialiez data
     */
    class data_ostream: public std::ostream {
    public:
        /**
         * Constructor
         * @param stream        Stream to write to
         * @param endianness    Endianness of the output
         */
        data_ostream(std::ostream& stream, endianconv::endian endianness);

        /**
         * Writes a boolean
         * @param value The boolean to write
         */
        void writeBoolean(bool value);

        /**
         * Writes a byte
         * @param value The byte to write
         */
        void writeByte(int8_t value);

        /**
         * Writes an unsigned byte
         * @param value The unsigned byte to write
         */
        void writeUnsignedByte(uint8_t value);

        /**
         * Writes a short
         * @param value The short to write
         */
        void writeShort(int16_t value);

        /**
         * Writes an unsigned short
         * @param value The unsigned short to write
         */
        void writeUnsignedShort(uint16_t value);

        /**
         * Writes an int
         * @param value The int to write
         */
        void writeInt(int32_t value);

        /**
         * Writes an unsigned int
         * @param value The int to write
         */
        void writeUnsignedInt(uint32_t value);

        /**
         * Writes a long
         * @param value The long to write
         */
        void writeLong(int64_t value);

        /**
         * Writes an unsigned long
         * @param value The unsigned long to write
         */
        void writeUnsignedLong(uint64_t value);

        /**
         * Writes a float
         * @param value The float to write
         */
        void writeFloat(float value);

        /**
         * Writes a double
         * @param value The double to write
         */
        void writeDouble(double value);

        /**
         * Writes a string
         * @param value The string to write
         */
        void writeUTF(std::string value);
    private:
        endianconv::endian m_endianness;
    };

} /* namespace streams */

#endif /* STREAMS_DATA_HPP_ */
