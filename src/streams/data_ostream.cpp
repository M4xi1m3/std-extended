#include <iostream>
#include "streams/data.hpp"
#include "streams/exceptions.hpp"

using namespace stde::streams;

data_ostream::data_ostream(std::ostream& stream, endianconv::endian endianness) : std::ostream(stream.rdbuf()), m_endianness(endianness) {
}

void data_ostream::writeBoolean(bool value) {
    int8_t val = value ? 1 : 0;
    write((char*) &val, 1);
}

void data_ostream::writeByte(int8_t value) {
    int8_t val = value;
    write((char*) &val, 1);
}

void data_ostream::writeUnsignedByte(uint8_t value) {
    uint8_t val = value;
    write((char*) &val, 1);
}

void data_ostream::writeShort(int16_t value) {
    int16_t val = streams::endianconv::convert(value, streams::endianconv::native, m_endianness);
    write((char*) &val, 2);
}

void data_ostream::writeUnsignedShort(uint16_t value) {
    uint16_t val = streams::endianconv::convert(value, streams::endianconv::native, m_endianness);
    write((char*) &val, 2);
}

void data_ostream::writeInt(int32_t value) {
    int32_t val = streams::endianconv::convert(value, streams::endianconv::native, m_endianness);
    write((char*) &val, 4);
}

void data_ostream::writeUnsignedInt(uint32_t value) {
    uint32_t val = streams::endianconv::convert(value, streams::endianconv::native, m_endianness);
    write((char*) &val, 4);
}

void data_ostream::writeLong(int64_t value) {
    int64_t val = streams::endianconv::convert(value, streams::endianconv::native, m_endianness);
    write((char*) &val, 8);
}

void data_ostream::writeUnsignedLong(uint64_t value) {
    uint64_t val = streams::endianconv::convert(value, streams::endianconv::native, m_endianness);
    write((char*) &val, 8);
}

void data_ostream::writeFloat(float value) {
    uint32_t *tmp = reinterpret_cast<uint32_t*>(&value);
    uint32_t val = streams::endianconv::convert(*tmp, streams::endianconv::native, m_endianness);
    write((char*) &val, 4);
}

void data_ostream::writeDouble(double value) {
    uint64_t *tmp = reinterpret_cast<uint64_t*>(&value);
    uint64_t val = streams::endianconv::convert(*tmp, streams::endianconv::native, m_endianness);
    write((char*) &val, 8);
}

void data_ostream::writeUTF(std::string value) {
    writeShort(value.length());
    write(value.c_str(), value.length());
}
