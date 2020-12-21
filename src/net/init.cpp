#include "net/init.hpp"

#ifdef WIN32
#include <system_error>
#include <winsock2.h>
#ifndef errno
#define errno WSAGetLastError()
#endif
#else
#include <csignal>
#endif


namespace stde {
    namespace net {
        void init() {
#ifdef WIN32
            WSADATA wsaData;

            if (WSAStartup(0x202, &wsaData) != 0)
                throw std::system_error(errno, std::system_category(), "Error with WSAStartup");
#else
            // Don't make SIGPIPE stop the application.
            signal(SIGPIPE, SIG_IGN);
#endif
        }

        void deinit() {
#ifdef WIN32
            WSACleanup();
#else
            // Don't make SIGPIPE stop the application.
            signal(SIGPIPE, SIG_DFL);
#endif
        }
    }
}
