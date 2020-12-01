#include "net/sock.h"
#include "net/serversock.h"
#include "streams/sock.h"

int main() {

    net::server_sock s("127.0.0.1", "12345");
    s.setReuseAddress(true);
    s.bind();
    s.listen();

    while(1) {
        net::sock client = s.accept();

        streams::sock_ostream client_out(client);

        client_out << "Hello\n";
        client_out.flush();
        client.close();
    }

    return 0;
}
