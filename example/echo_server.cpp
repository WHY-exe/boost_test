#include <spdlog/spdlog.h>
#include <boost/asio.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/stacktrace.hpp>


#include "util/asio_as_result.hpp"
#include "util/exception.hpp"
#include "util/util.hpp"


using tcp = boost::asio::ip::tcp;

boost::asio::awaitable<void> echo(tcp::socket socket, tcp::endpoint peer_ep) {
	try {
		char buffer[1024] = { 0 };
		while (true) {
			util::result<size_t> read_size = co_await socket.async_read_some(
					boost::asio::buffer(buffer), CO_RESULT_TOKEN);
			if (!read_size) {
				if (read_size.error() == boost::asio::error::eof) {
					SPDLOG_INFO("connection close for client {}: {}", peer_ep.address().to_string(), peer_ep.port());
					break;
				}
				SPDLOG_ERROR("read error: {}", read_size.error().message());
				break;
			}
			std::string_view sv_buffer(&buffer[0], read_size.value());
			SPDLOG_INFO("recieve {} from endpoint: {}:{}", sv_buffer, peer_ep.address().to_string(), peer_ep.port());
			util::result<size_t> write_res = co_await socket.async_write_some(
					boost::asio::buffer(sv_buffer), CO_RESULT_TOKEN);
			if (!write_res) {
				SPDLOG_ERROR("write error: {}", write_res.error().message());
				break;
			}
		}
	} catch (const std::exception &e) {
		SPDLOG_ERROR("catch exception: {}", e.what());
	} catch (...) {
		SPDLOG_ERROR("catch unknown exception");
	}
}

boost::asio::awaitable<void> listen() {
	using tcp = boost::asio::ip::tcp;

	try {
		auto executor = co_await boost::asio::this_coro::executor;
		tcp::acceptor			 acceptor(executor, { tcp::v4(), 7800 });
		SPDLOG_INFO("listening on port 7800");
		while (true) {
			tcp::endpoint			  peer_ep;
			util::result<tcp::socket> socket = co_await acceptor.async_accept(peer_ep, CO_RESULT_TOKEN);
			if (!socket) {
				SPDLOG_INFO("accept error: {}", socket.error().message());
				continue;
			}
			SPDLOG_INFO("accept connection from: {}: {}", peer_ep.address().to_string(), peer_ep.port());
			boost::asio::co_spawn(executor, echo(std::move(socket.value()), std::move(peer_ep)),
					boost::asio::detached);
		}
	} catch (const std::exception &e) {
		SPDLOG_ERROR("catch exception: {}", e.what());
	} catch (...) {
		SPDLOG_ERROR("catch unknown exception");
	}
}

int main() {
	try {
		util::init_log(spdlog::level::trace);
		boost::asio::io_context io_ctx;
		boost::asio::signal_set signals(io_ctx, SIGINT, SIGTERM);
		signals.async_wait([&](const boost::system::error_code &, int) { io_ctx.stop(); });
		boost::asio::co_spawn(io_ctx, listen(), boost::asio::detached);
		io_ctx.run();
		return EXIT_SUCCESS;
	} catch (const std::exception &e) {
		const boost::stacktrace::stacktrace *st = boost::get_error_info<util::exception::traced>(e);
		std::stringstream					 ss;
		ss << (st ? *st : boost::stacktrace::stacktrace::from_current_exception());
		SPDLOG_ERROR("Catch exception: {}, trace: \n {}", e.what(), ss.str());
	} catch (...) {
		std::stringstream ss;
		ss << boost::stacktrace::stacktrace::from_current_exception();
		SPDLOG_ERROR("Catch unknown exception ... trace: {}", ss.str());
	}
	return EXIT_FAILURE;
}