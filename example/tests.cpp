#include <iostream>
#include <fstream>

#include "stde/net/serversock.hpp"
#include "stde/net/sock.hpp"
#include "stde/net/sockaddress.hpp"
#include "stde/streams/sock.hpp"
#include "stde/conf/properties.hpp"


using namespace stde;

int main() {
    /*
    std::ifstream f("tmp.properties");
    conf::properties p;
    p.load(f);
    p.save(std::cout);
    */


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


    /*
     std::cout << net::sock_address("[::1]:12345") << std::endl;
     net::sock s(net::sock_address("[::1]:12345"));
     s.connect();

     streams::sock_ostream out(s);
     out << "Hello world!\n";
     out.flush();
     s.close();
     */

    return 0;
}
