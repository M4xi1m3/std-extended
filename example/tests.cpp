#include <iostream>
#include "net/serversock.hpp"
#include "net/sock.hpp"
#include "net/sockaddress.hpp"
#include "streams/sock.hpp"

int main() {
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
