#include "streams/tee.hpp"

using namespace stde::streams;

tee_streambuf::tee_streambuf(std::streambuf* sb1, std::streambuf* sb2) : m_sb1(sb1), m_sb2(sb2) {

}

tee_streambuf::~tee_streambuf() {
}

int tee_streambuf::overflow(int c) {
    if (c == EOF) {
        return !EOF;
    } else {
        int const r1 = m_sb1->sputc(c);
        int const r2 = m_sb2->sputc(c);
        return r1 == EOF || r2 == EOF ? EOF : c;
    }
}

int tee_streambuf::sync() {
    int const r1 = m_sb1->pubsync();
    int const r2 = m_sb2->pubsync();
    return r1 == 0 && r2 == 0 ? 0 : -1;
}
