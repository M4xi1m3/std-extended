#include "streams/endian.hpp"

using namespace streams;

endianconv::endian endianconv::native = (*(char*) (int[] ) { 1 } ) ? endian::little : endian::big;
