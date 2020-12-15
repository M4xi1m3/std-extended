#include "streams/endian.hpp"
#include <cstdint>

using namespace stde::streams;

static endianconv::endian get_platform_endianness() {
    union {
        uint32_t i;
        char c[4];
    } bint = { 0x01020304 };

    return bint.c[0] == 1 ? endianconv::endian::big : endianconv::endian::little;
}

endianconv::endian endianconv::native = get_platform_endianness();
