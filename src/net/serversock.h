#ifndef NET_SERVERSOCK_H_
#define NET_SERVERSOCK_H_

#include <string>

#include "sock.h"

namespace net {

    class server_sock {
    public:
        server_sock(const std::string& address, const std::string& port);
        virtual ~server_sock();

        sock accept();
    private:
        int m_sockfd;
    };

} /* namespace net */

#endif /* NET_SERVERSOCK_H_ */
