// Basic Client

#include <iostream>
#include <asio.hpp>

namespace ip = asio::ip;
using tcp = asio::ip::tcp;
using error_code = asio::error_code;

int main()
{
    asio::io_context io_context;
    tcp::socket socket(io_context);
    ip::address address = ip::make_address("127.0.0.1");
    tcp::endpoint endpoint(address, 15001);

    error_code ec;
    socket.connect(endpoint, ec);

    if (!ec) {
        std::cout << "The connection has been established!";
    }
    else {
        std::cerr << "Something went wrong :(";
    }

    return 0;
}
