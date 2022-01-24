// Timer

#include <iostream>
#include <queue>
#include <unordered_set>
#include <asio.hpp>

class MTimer {
private:
	asio::steady_timer t_;
public:
	MTimer(asio::io_context& io) : t_(io, std::chrono::milliseconds(500)) {
		t_.async_wait(std::bind(&MTimer::start, this, std::placeholders::_1));
	}
	void start(asio::error_code ec) {
		if (ec == asio::error::operation_aborted) {
			std::cout << "The timer is cancelled\n";
			return;
		}
		std::cout << "timer fires..\r\n";
		t_.expires_after(std::chrono::milliseconds(500));
		t_.async_wait(std::bind(&MTimer::start, this, std::placeholders::_1));
	}
	asio::steady_timer& get_timer() { return t_; }
};

int main() {
	asio::io_context io_context;
	MTimer mt(io_context);
	
	asio::steady_timer watchdog(io_context, std::chrono::seconds(5));
	watchdog.async_wait([&](asio::error_code ec){ mt.get_timer().cancel(); });
	io_context.run();
	return 0;
}
