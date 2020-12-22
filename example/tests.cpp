#include <iostream>
#include <fstream>

#include "stde/stde.hpp"

using namespace stde;

int main() {
    net::init();

    stde::streams::tee_ostream tee(std::cout, std::cerr);
    tee << "Hello!" << std::endl;
    /*
     net::server_sock s(net::sock_address("[::]:12345"));
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
     s.close();
     */
    /*
     net::sock s(net::sock_address("127.0.0.1:12345"));
     s.connect();
     std::cout << "Socket on " << s.address() << " connected to " << s.peer_address() << std::endl;

     streams::sock_ostream sout(s);
     sout << "Hello from " << s.address() << std::endl;
     sout.flush();
     s.close();
     */
    net::deinit();
    return 0;
}
