#ifndef STREAMS_ENDIAN_H_
#define STREAMS_ENDIAN_H_

namespace streams {

    /**
     * Does conversions between different endianness
     */
    class endianconv {
    public:
        /**
         * Possible endianness
         */
        enum endian {
            little = 0, big = 1
        };

        /**
         * Native endianness, set at runtime
         */
        static endian native;

        /**
         * Converts between endianness for arbitrary type
         *
         * @tparam T    Type to convert
         * @param value Value to convert
         * @param from  Input endianness
         * @param to    Output endianness
         * @return      Converted value
         */
        template<typename T>
        static T convert(T const& value, endian from, endian to) {
            if (from == to) {
                return value;
            } else {
                T ret;

                const char *arr = (const char*) &value;
                char *out = (char*) &ret;

                for (long unsigned int i = 0; i < sizeof(T); i++) {
                    out[sizeof(T) - 1 - i] = arr[i];
                }

                return ret;
            }
        }
    };

} /* namespace streams */

#endif /* STREAMS_ENDIAN_H_ */
