#include <iomanip>
#include <iostream>
#include <asio.hpp>
#include <stdlib.h>
#include <windows.h>

using namespace std::placeholders;
using sec = std::chrono::seconds;
using msec = std::chrono::milliseconds;

void resize_console() {
	HWND console = GetConsoleWindow();
	RECT ConsoleRect;
	GetWindowRect(console, &ConsoleRect);
	MoveWindow(console, ConsoleRect.left, ConsoleRect.top, 300, 120, TRUE);
}

void print_cd(asio::error_code ec, asio::steady_timer* t, int left) {
	if (ec == asio::error::operation_aborted) { return; }
	std::cout << "\r剩余时间: " << std::setw(5) << left << " 喵";
	t->expires_after(sec(1));
	t->async_wait(std::bind(&print_cd, _1, t, --left));
}

int main(int argc, const char* argv[]) {
	resize_console();
	std::string line;
	std::cout << "输入时间(秒): ";
	std::getline(std::cin, line);
	int delay = atoi(line.c_str());
	asio::io_context io;
	asio::steady_timer t_(io);
	asio::steady_timer cd(io, sec(0));
	if (delay == 0) {
		int default_delay = 1800;
		t_.expires_from_now(sec(default_delay));
		t_.async_wait([&](asio::error_code ec){
			cd.cancel();
			LockWorkStation();
			return 0;
		});
		cd.async_wait(std::bind(&print_cd, _1, &cd, default_delay));
	}
	else {
		t_.expires_from_now(sec(delay));
		t_.async_wait([&](asio::error_code ec){
			cd.cancel();
			LockWorkStation();
			return 0;
		});
		cd.async_wait(std::bind(&print_cd, _1, &cd, delay));
	}
	io.run();
	return 0;
}
