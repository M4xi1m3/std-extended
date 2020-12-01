#include "data.h"
#include "exceptions.h"

#include <iostream>

using namespace streams;

data_istream::data_istream(std::istream& stream, endianconv::endian endianness) : std::istream(stream.rdbuf()), m_endianness(endianness) {
}

bool data_istream::readBoolean() {
    uint8_t out = 0;
    if (readsome((char*) &out, 1) != 1)
        throw eof_exception("Trying to read after EOF.");
    return out != 0;
}

int8_t data_istream::readByte() {
    int8_t out = 0;
    if (readsome((char*) &out, 1) != 1)
        throw eof_exception("Trying to read after EOF.");
    return out;
}

uint8_t data_istream::readUnsignedByte() {
    uint8_t out = 0;
    if (readsome((char*) &out, 1) != 1)
        throw eof_exception("Trying to read after EOF.");
    return out;
}

int16_t data_istream::readShort() {
    int16_t out = 0;
    if (readsome((char*) &out, 2) != 2)
        throw eof_exception("Trying to read after EOF.");
    return streams::endianconv::convert(out, m_endianness, streams::endianconv::native);
}

uint16_t data_istream::readUnsignedShort() {
    uint16_t out = 0;
    if (readsome((char*) &out, 2) != 2)
        throw eof_exception("Trying to read after EOF.");
    return streams::endianconv::convert(out, m_endianness, streams::endianconv::native);
}

int32_t data_istream::readInt() {
    int32_t out = 0;
    if (readsome((char*) &out, 4) != 4)
        throw eof_exception("Trying to read after EOF.");
    return streams::endianconv::convert(out, m_endianness, streams::endianconv::native);
}

uint32_t data_istream::readUnsignedInt() {
    uint32_t out = 0;
    if (readsome((char*) &out, 4) != 4)
        throw eof_exception("Trying to read after EOF.");
    return streams::endianconv::convert(out, m_endianness, streams::endianconv::native);
}

int64_t data_istream::readLong() {
    int64_t out = 0;
    if (readsome((char*) &out, 8) != 8)
        throw eof_exception("Trying to read after EOF.");
    return streams::endianconv::convert(out, m_endianness, streams::endianconv::native);
}

uint64_t data_istream::readUnsignedLong() {
    int64_t out = 0;
    if (readsome((char*) &out, 8) != 8)
        throw eof_exception("Trying to read after EOF.");
    return streams::endianconv::convert(out, m_endianness, streams::endianconv::native);
}

float data_istream::readFloat() {
    uint32_t out = 0;
    if (readsome((char*) &out, 4) != 4)
        throw eof_exception("Trying to read after EOF.");
    out = streams::endianconv::convert(out, m_endianness, streams::endianconv::native);
    float *tmp = reinterpret_cast<float*>(&out);
    return *tmp;
}

double data_istream::readDouble() {
    uint64_t out = 0;
    if (readsome((char*) &out, 8) != 8)
        throw eof_exception("Trying to read after EOF.");
    out = streams::endianconv::convert(out, m_endianness, streams::endianconv::native);
    double *tmp = reinterpret_cast<double*>(&out);
    return *tmp;
}

std::string data_istream::readUTF() {
    uint16_t length = readUnsignedShort();

    char *buff = new char[length];

    if (readsome(buff, length) != length)
        throw eof_exception("Trying to read after EOF.");

    std::string out(buff, buff + length);

    delete[] buff;

    return out;
}
