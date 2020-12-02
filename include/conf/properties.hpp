#ifndef CONF_PROPERTIES_HPP_
#define CONF_PROPERTIES_HPP_

#include <map>
#include <string>
#include <iostream>

namespace stde::conf {

    /**
     * Read and write java-like properties file.
     *
     * This format is similar to the java one except:
     *  - It uses UTF-8 instead of ISO-8891-1
     *  - Lines can only end in \n
     *  - Since it's in UTF-8, the \u escape sequence doesn't exist.
     */
    class properties : std::map<std::string, std::string> {
    public:
        properties();
        virtual ~properties();

        /**
         * Loads properties from an input stream.
         * @param input Stream to load from
         */
        void load(std::istream& input);

        /**
         * Saves properties to an output stream.
         * @param output    Stream to save to
         */
        void save(std::ostream& output);
    };

} /* namespace stde::conf */

#endif /* CONF_PROPERTIES_HPP_ */
