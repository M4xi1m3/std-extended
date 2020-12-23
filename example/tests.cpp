#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>

#include "stde/stde.hpp"
#include "stde/log/log.hpp"

using namespace stde;

int main() {
    net::init();
    log::log l = log::log::get("main");

    net::server_sock s(net::sock_address("0.0.0.0:12346"));
    s.blocking(false);
    s.reuse_address(true);
    s.bind();
    s.listen();

    l << log::level::info << "Server listening on " << s.address() << "." << std::endl;

    while (1) {
        try {
            net::sock client = s.accept();
            net::sock_address client_addr = client.peer_address();

            l << log::level::info << "Accepted connection from " << client_addr << "." << std::endl;

            streams::sock_ostream client_out(client);
            client.shutdown();

            client_out << "Hello " << client_addr << "\n";

            client_out.flush();
            client.close();
            s.close();

            l << log::level::info << "Closed connection with " << client_addr << "." << std::endl;
            break;
        } catch (const std::system_error &e) {
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        l << log::level::debug << "Waiting for connection..." << std::endl;
    }

    s.close();

    log::log::clean();
    net::deinit();
    return 0;
}
