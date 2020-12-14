#include <iostream>
#include "streams/data.hpp"
#include "streams/exceptions.hpp"

using namespace stde::streams;

data_istream::data_istream(const std::istream& stream, endianconv::endian endianness) : std::istream(stream.rdbuf()), m_endianness(endianness) {
}

bool data_istream::read_bool() {
    uint8_t out = 0;
    read((char*) &out, 1);

    if (eof())
        throw eof_exception("Trying to read after EOF.");
    return out != 0;
}

int8_t data_istream::read_byte() {
    int8_t out = 0;
    read((char*) &out, 1);

    if (eof())
        throw eof_exception("Trying to read after EOF.");
    return out;
}

uint8_t data_istream::read_ubyte() {
    uint8_t out = 0;
    read((char*) &out, 1);

    if (eof())
        throw eof_exception("Trying to read after EOF.");
    return out;
}

int16_t data_istream::read_short() {
    int16_t out = 0;
    read((char*) &out, 2);

    if (eof())
        throw eof_exception("Trying to read after EOF.");
    return streams::endianconv::convert(out, m_endianness, streams::endianconv::native);
}

uint16_t data_istream::read_ushort() {
    uint16_t out = 0;
    read((char*) &out, 2);

    if (eof())
        throw eof_exception("Trying to read after EOF.");
    return streams::endianconv::convert(out, m_endianness, streams::endianconv::native);
}

int32_t data_istream::read_int() {
    int32_t out = 0;
    read((char*) &out, 4);

    if (eof())
        throw eof_exception("Trying to read after EOF.");
    return streams::endianconv::convert(out, m_endianness, streams::endianconv::native);
}

uint32_t data_istream::read_uint() {
    uint32_t out = 0;
    read((char*) &out, 4);

    if (eof())
        throw eof_exception("Trying to read after EOF.");
    return streams::endianconv::convert(out, m_endianness, streams::endianconv::native);
}

int64_t data_istream::read_long() {
    int64_t out = 0;
    read((char*) &out, 8);

    if (eof())
        throw eof_exception("Trying to read after EOF.");
    return streams::endianconv::convert(out, m_endianness, streams::endianconv::native);
}

uint64_t data_istream::read_ulong() {
    int64_t out = 0;
    read((char*) &out, 8);

    if (eof())
        throw eof_exception("Trying to read after EOF.");
    return streams::endianconv::convert(out, m_endianness, streams::endianconv::native);
}

float data_istream::read_float() {
    uint32_t out = 0;
    read((char*) &out, 4);

    if (eof())
        throw eof_exception("Trying to read after EOF.");
    out = streams::endianconv::convert(out, m_endianness, streams::endianconv::native);
    float *tmp = reinterpret_cast<float*>(&out);
    return *tmp;
}

double data_istream::read_double() {
    uint64_t out = 0;
    read((char*) &out, 8);

    if (eof())
        throw eof_exception("Trying to read after EOF.");
    out = streams::endianconv::convert(out, m_endianness, streams::endianconv::native);
    double *tmp = reinterpret_cast<double*>(&out);
    return *tmp;
}

std::string data_istream::read_string() {
    uint16_t length = read_ushort();

    char *buff = new char[length];

    read((char*) buff, length);

    if (eof())
        throw eof_exception("Trying to read after EOF.");

    std::string out(buff, buff + length);

    delete[] buff;

    return out;
}
