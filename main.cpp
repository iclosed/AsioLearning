#include <iostream>
#include <asio.hpp>

int main(){
	std::cout << "Hello, world! Please wait 3 seconds..." << std::endl;
	asio::io_context io;
	asio::steady_timer t(io, asio::chrono::seconds(3));
	t.wait();
	std::cout << "Hello, world again!" << std::endl;
	return 0;
}
