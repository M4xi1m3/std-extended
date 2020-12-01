#include "net/sock.h"
#include "net/serversock.h"
#include "net/sockaddress.h"
#include "streams/sock.h"

#include <iostream>

int main() {
/*
    net::server_sock s(net::sock_address("::1", 12345, net::sock_address::inet6));
    s.setReuseAddress(true);
    s.bind();
    s.listen();

    std::cout << "Server listening on " << s.getAddress() << std::endl;

    net::sock client = s.accept();
    net::sock_address client_addr = client.getPeerAddress();

    streams::sock_ostream client_out(client);

    client_out << "Hello " << client_addr;

    client_out.flush();
    client.close();
*/
    net::sock s(net::sock_address("::1", 12345, net::sock_address::inet6));
    s.connect();

    streams::sock_ostream out(s);
    out << "Hello world!\n";
    out.flush();
    s.close();


    return 0;
}
