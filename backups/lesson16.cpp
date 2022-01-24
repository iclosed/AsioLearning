// Hostname Resolver

#include <iostream>
#include <asio.hpp>

namespace ip = asio::ip;
using tcp = ip::tcp;
using error_code = asio::error_code;

int main()
{
    asio::io_context io_context;
    error_code error;
    tcp::resolver resolver(io_context);

    tcp::resolver::results_type results = resolver.resolve("google.com", "80", error);

    for (tcp::endpoint const& endpoint : results) {
        std::cout << endpoint << "\n";
    }

    return 0;
}