#include "conf/properties.hpp"

#include <vector>
#include <iostream>

using namespace stde::conf;

properties::properties() {
}

properties::~properties() {
}

/**
 * Decodes escaped string.
 * @param in    Escaped string
 * @return      String with escaping taken into account
 */
static std::string load_convert(const std::string& in) {
    if (in.find("\\") == in.npos)
        return in;

    char c;
    std::string out = "";

    for (size_t i = 0; i < in.length();) {
        c = in[i++];

        if (c == '\\') {
            c = in[i++];

            switch (c) {
                case 't':
                    c = '\t';
                    break;
                case 'r':
                    c = '\r';
                    break;
                case 'n':
                    c = '\n';
                    break;
                case 'f':
                    c = '\f';
                    break;
                default:
                    break;
            }

            out += c;
        } else {
            out += c;
        }
    }

    return out;
}

static std::string save_convert(const std::string& in, bool escapeSpace) {
    std::string out;

    for (size_t i = 0; i < in.length(); i++) {
        char c = in[i];
        if (c == ' ' && (i == 0 || escapeSpace)) {
            out += "\\ ";
        } else if (c == '\t') {
            out += "\\t";
        } else if (c == '\n') {
            out += "\\n";
        } else if (c == '\r') {
            out += "\\r";
        } else if (c == '\f') {
            out += "\\f";
        } else if (c == '=' || c == ':' || c == '!' || c == '#' || c == '\\') {
            out += '\\';
            out += c;
        } else {
            out += c;
        }
    }

    return out;
}

/**
 * Left-trims a string
 * @param str   String to left-trim
 * @return      Left-trimmed string
 */
static std::string ltrim(const std::string& str) {
    size_t pos = str.find_first_not_of(" \t\f");
    if (pos == str.npos)
        return str;
    else
        return str.substr(pos, str.length() - pos);
}

/**
 * Reads a logical line from in
 * @param in    Input stream to read from
 * @param out   String to write to
 * @return      -1 if EOF, 0 if comment / blank, 1 if actual line.
 */
static int readline(std::istream& in, std::string& out) {
    out = "";

    if (in.eof())
        return -1;
    int backslash_count = 0;
    std::string line;
    do {
        std::getline(in, line);
        out += ltrim(line);
        if (out.length() == 0)
            return 0;
        if (out.find_first_of("#!") == 0)
            return 0;
        if (out.find_last_of("\\") != out.length() - 1) {
            return 1;
        } else {
            backslash_count = out.length() - out.find_last_not_of("\\") - 1;
            if (backslash_count % 2 == 1)
                out = out.substr(0, out.length() - 1);
        }
    } while (backslash_count % 2 == 1);

    return 1;
}

void properties::load(std::istream& input) {
    std::string l = "";
    int t;
    do {
        t = readline(input, l);
        if (t == 1) {

            char c = 0;
            int keylen = 0;
            int limit = l.length();
            int valuestart = limit;
            bool hassep = false;
            bool precedingbackslash = false;

            while (keylen < limit) {
                c = l[keylen];

                if ((c == '=' || c == ':') && !precedingbackslash) {
                    valuestart = keylen + 1;
                    hassep = true;
                    break;
                } else if ((c == ' ' || c == '\t' || c == '\f') && !precedingbackslash) {
                    valuestart = keylen + 1;
                    break;
                }
                if (c == '\\') {
                    precedingbackslash = !precedingbackslash;
                } else {
                    precedingbackslash = false;
                }
                keylen++;
            }
            while (valuestart < limit) {
                c = l[valuestart];
                if (c != ' ' && c != '\t' && c != '\f') {
                    if (!hassep && (c == '=' || c == ':')) {
                        hassep = true;
                    } else {
                        break;
                    }
                }
                valuestart++;
            }
            std::string key(load_convert(l.substr(0, keylen)));
            std::string value(load_convert(l.substr(valuestart, limit - valuestart)));
            (*this)[key] = value;
        }
    } while (t != -1);
}

void properties::save(std::ostream& output) {
    for (std::pair<std::string, std::string> const &x : *this) {
        output << save_convert(x.first, true) << "=" << save_convert(x.second, false) << "\n";
    }
}
