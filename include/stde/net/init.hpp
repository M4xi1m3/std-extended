#ifndef STDE_NET_INIT_HPP_
#define STDE_NET_INIT_HPP_

namespace stde {
    namespace net {
        /**
         * Initializes the network.
         *
         * On Windows, it calls WSAStartup, asking for version 2.2.
         * On Linux, it calls signal to ignore SIGPIPE.
         */
        void init();

        /**
         * De-initializes the network.
         *
         * On Windows, it calls WSACleanup.
         * On Linux, is restores the default signal handler for SIGPIPE.
         */
        void deinit();
    }
}

#endif
