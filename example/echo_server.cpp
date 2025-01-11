#include <boost/asio.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>

#include "util/asio_as_result.hpp"
#include "util/log.hpp"

using tcp = boost::asio::ip::tcp;

boost::asio::awaitable<void> echo(tcp::socket socket, tcp::endpoint peer_ep) {
	try {
		char buffer[1024] = { 0 };
		while (true) {
			boost::outcome_v2::result<size_t> read_size = co_await socket.async_read_some(
					boost::asio::buffer(buffer), CO_RESULT_TOKEN);
			if (!read_size) {
				if (read_size.error() == boost::asio::error::eof) {
					LOG_MSG(info) << "connection close for client" << peer_ep.address().to_string() << ":"
								  << peer_ep.port();
					break;
				}
				LOG_MSG(error) << "read error: " << read_size.error().message();
				break;
			}
			std::string_view sv_buffer(&buffer[0], read_size.value());
			LOG_MSG(info) << "recieve " << sv_buffer << " from endpoint: " << peer_ep.address().to_string() << ":" << peer_ep.port();
			boost::outcome_v2::result<size_t> write_res = co_await socket.async_write_some(
					boost::asio::buffer(sv_buffer), CO_RESULT_TOKEN);
			if (!write_res) {
				LOG_MSG(error) << "write error: " << write_res.error().message();
				break;
			}
		}
	} catch (const std::exception &e) {
		LOG_MSG(error) << "catch exception: " << e.what();
	} catch (...) {
		LOG_MSG(error) << "catch unknown exception";
	}
}

boost::asio::awaitable<void> listen() {
	using tcp = boost::asio::ip::tcp;

	try {
		auto executor = co_await boost::asio::this_coro::executor;
		tcp::acceptor			 acceptor(executor, { tcp::v4(), 7800 });
		LOG_MSG(info) << "listening on port 7800";
		while (true) {
			tcp::endpoint						   peer_ep;
			boost::outcome_v2::result<tcp::socket> socket = co_await acceptor.async_accept(peer_ep, CO_RESULT_TOKEN);
			if (!socket) {
				LOG_MSG(error) << "accept error: " << socket.error().message();
				continue;
			}
			LOG_MSG(info) << "accept connection from: " << peer_ep.address().to_string() << ":" << peer_ep.port();
			boost::asio::co_spawn(executor, echo(std::move(socket.value()), std::move(peer_ep)),
					boost::asio::detached);
		}
	} catch (const std::exception &e) {
		LOG_MSG(error) << "catch exception: " << e.what();
	} catch (...) {
		LOG_MSG(error) << "catch unknown exception";
	}
}

int main() {
	try {
		const auto log_ini_stat =
				util::Logger::init_default(util::log_level::trace, false, true);
		if (!log_ini_stat) {
			std::cout << "fail to ini default log sink" << '\n';
		}
		boost::asio::io_context io_ctx;
		boost::asio::signal_set signals(io_ctx, SIGINT, SIGTERM);
		signals.async_wait([&](const boost::system::error_code &, int) { io_ctx.stop(); });
		boost::asio::co_spawn(io_ctx, listen(), boost::asio::detached);
		io_ctx.run();
		return EXIT_SUCCESS;
	} catch (const std::exception &e) {
		LOG_MSG(error) << "catch exception: " << e.what();
	} catch (...) {
		LOG_MSG(error) << "catch unknown exception";
	}
	return EXIT_FAILURE;
}