#include <iostream>
#include "net/serversock.hpp"
#include "net/sock.hpp"
#include "net/sockaddress.hpp"
#include "streams/sock.hpp"

#include <fstream>
#include "conf/properties.hpp"

using namespace stde;

int main() {
    std::ifstream f("tmp.properties");
    conf::properties p;
    p.load(f);
    p.save(std::cout);

    /*
     net::server_sock s(net::sock_address("0.0.0.0:12345"));
     s.setReuseAddress(true);
     s.bind();
     s.listen();

     std::cout << "Server listening on " << s.getAddress() << std::endl;

     net::sock client = s.accept();
     net::sock_address client_addr = client.getPeerAddress();

     streams::sock_ostream client_out(client);

     client_out << "Hello " << client_addr << "\n";

     client_out.flush();
     client.close();
     */

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
