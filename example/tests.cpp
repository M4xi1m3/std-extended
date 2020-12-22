#include <iostream>
#include <fstream>

#include "stde/stde.hpp"
#include "stde/log/log.hpp"

using namespace stde;

int main() {
    net::init();
    log::log l = log::log::get("main");

    net::server_sock s(net::sock_address("[::]:12345"));
    s.reuse_address(true);
    s.bind();
    s.listen();

    l << log::level::info << "Server listening on " << s.address() << "." << std::endl;

    net::sock client = s.accept();
    net::sock_address client_addr = client.peer_address();

    l << log::level::info << "Accepted connection from  " << client_addr << "." << std::endl;

    streams::sock_ostream client_out(client);

    client_out << "Hello " << client_addr << "\n";

    client_out.flush();
    client.close();
    s.close();

    l << log::level::info << "Closed connection from  " << client_addr << "." << std::endl;

    log::log::clean();
    net::deinit();
    return 0;
}
