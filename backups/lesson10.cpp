// Prevent io_context from being done

#include <iostream>
#include <queue>
#include <unordered_set>
#include <asio.hpp>


int main() {
	asio::io_context io_context;

	asio::executor_work_guard<asio::io_context::executor_type> work_guard(io_context.get_executor());
	std::thread watchdog([&] {
		std::this_thread::sleep_for(std::chrono::seconds(3));
		std::cout << "thread watchdog, force stop." << std::endl;
		//io_context.stop();
		work_guard.reset();
	});
	io_context.run();
	std::cout << "Jobs done." << std::endl;
	return 0;
}
