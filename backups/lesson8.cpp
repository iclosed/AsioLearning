
// TCP Chat Room

#include <iostream>
#include <queue>
#include <unordered_set>
#include <asio.hpp>


class Session : public std::enable_shared_from_this<Session> {
private:
	asio::streambuf streambuf;
	asio::ip::tcp::socket socket;
	std::function<void(std::string, std::shared_ptr<Session>)> on_msg;
	std::function<void()> on_error;
	std::queue<std::string> write_q;

public:
	Session(asio::io_context& io): socket(io) {
		std::cout << "new session created." << std::endl;
	}
	~Session() { std::cout << "session destroyed." << std::endl; }

	asio::ip::tcp::socket& Socket() { return socket; }
	void start(std::function<void(std::string, std::shared_ptr<Session>)>&& msg_handle, std::function<void()>&& ec_handle) {
		on_msg = std::move(msg_handle);
		on_error = std::move(ec_handle);
		async_read();
	}
	void async_read() {
		//post(std::string("> "));
		asio::async_read_until(socket, streambuf, '\n', [&, p = shared_from_this()](std::error_code ec, std::size_t len) {
			if (!ec) {
				std::stringstream msg;
				msg << socket.remote_endpoint(ec) << ": " << std::istream(&p->streambuf).rdbuf();
				streambuf.consume(len);
				on_msg(msg.str(), p);
				async_read();
			}
			else {
				socket.close(ec);
				on_error();
			}
		});
	}

	void async_write() {
		asio::async_write(socket, asio::buffer(write_q.front()), [&](asio::error_code ec, size_t len) {
			std::cout << "async write to client..." << std::endl;
			if (!ec) {
				write_q.pop();
				if (!write_q.empty()) {
					async_write();
				}
			}
			else {
				socket.close(ec);
				on_error();
			}
		});
	}

	void post(std::string const& msg) {
		bool idle = write_q.empty();
		write_q.push(msg);
		if (idle) { async_write(); }
	}
};

class Server {
private:
	asio::io_context& m_io_context;
	asio::ip::tcp::acceptor m_acceptor;
	std::unordered_set<std::shared_ptr<Session>> clients;
public:
	Server(asio::io_context& io_context, asio::ip::tcp::endpoint ep): m_io_context(io_context), m_acceptor(io_context, ep) {}
	void async_accept() {
		std::cout << "Server async accept looping..." << std::endl;
		auto client = std::make_shared<Session>(m_io_context);
		m_acceptor.async_accept(client->Socket(), [client, this](std::error_code error) {
			std::cout << "local endpoint is:" << client->Socket().local_endpoint() << std::endl;
			std::cout << "remote endpoint is:" << client->Socket().remote_endpoint() << std::endl;
			clients.insert(client);  // maintain the client

			//client->post("Welcome to chat room!\r\n");
			std::stringstream notice;
			notice << "Welcome <" << client->Socket().remote_endpoint() << "> to the chat room.\r\n";
			broadcast(notice.str(), NULL);
			client->start(
				std::bind(&Server::broadcast, this, std::placeholders::_1, std::placeholders::_2),
				[&, weak = std::weak_ptr<Session>(client)](){
					auto shared = weak.lock();
					if (shared && clients.erase(shared)) {
						std::cout << "erase the client." << std::endl;
					}
				}
			);
			async_accept();
		});
	}
	void broadcast(std::string const& msg, std::shared_ptr<Session> src_client) {
		for (auto& c : clients) {
			if (c == src_client) { continue; }
			std::cout << c << std::endl;
			c->post(msg);
		}
	}
};

int main() {
	asio::io_context io_context;
	Server srv(io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 15001));
	srv.async_accept();
	io_context.run();
	return 0;
}
