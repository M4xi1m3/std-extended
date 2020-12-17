#include <iostream>
#include <fstream>

#include "stde/net/init.hpp"
#include "stde/net/serversock.hpp"
#include "stde/net/sock.hpp"
#include "stde/net/sockaddress.hpp"
#include "stde/streams/sock.hpp"
#include "stde/conf/properties.hpp"

#ifdef WIN32
#include <winsock2.h>
#endif

#include <exception>

using namespace stde;

int main() {
    net::init();
    /*
     net::server_sock s(net::sock_address("0.0.0.0:12345"));
     s.reuse_address(true);
     s.bind();
     s.listen();

     std::cout << "Server listening on " << s.address() << std::endl;

     net::sock client = s.accept();
     net::sock_address client_addr = client.peer_address();

     streams::sock_ostream client_out(client);

     client_out << "Hello " << client_addr << "\n";

     client_out.flush();
     client.close();
     */

    net::sock s(net::sock_address("127.0.0.1:12345"));
    s.connect();
    std::cout << "Socket on " << s.address() << " connected to " << s.peer_address() << std::endl;

    streams::sock_ostream sout(s);
    sout << "Hello from " << s.address() << std::endl;
    sout.flush();
    s.close();

    net::deinit();
    return 0;
}
