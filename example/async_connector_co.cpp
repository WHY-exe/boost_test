#include <fmt/format.h>
#include <boost/asio.hpp>
#include <boost/stacktrace.hpp>

#include "util/asio_as_result.hpp"
#include "util/asio_timeout.hpp"
#include "util/exception.hpp"
#include "util/log.hpp"

using tcp = boost::asio::ip::tcp;

boost::asio::awaitable<void> async_connect(tcp::endpoint ep) {
	try {
		auto executor = co_await boost::asio::this_coro::executor;

		tcp::socket socket(executor);
		const auto	conn_time = std::chrono::system_clock::now();

		timeout_provider timeout{ executor, std::chrono::seconds(3) };
		auto conn_res = co_await socket.async_connect(ep, timeout(CO_RESULT_TOKEN));

		const auto now = std::chrono::system_clock::now();
		if (conn_res) {
			LOG_MSG(info) << fmt::format("connection to {}:{} takes {}ms", ep.address().to_string(), ep.port(),
					std::chrono::duration_cast<std::chrono::milliseconds>(now - conn_time).count());
			co_return;
		}

		LOG_MSG(error) << fmt::format("error connected to {}:{}: {}", ep.address().to_string(), ep.port(),
				conn_res.error().message());
	} catch (const std::exception &e) {
		LOG_MSG(error) << fmt::format("catch exception: {}", e.what());
	} catch (...) {
		LOG_MSG(error) << "Catch unknown exception ...";
		LOG_MSG(error) << "stacktrace:\n"
					   << boost::stacktrace::stacktrace::from_current_exception();
	}
}

int main() {
	try {
		const auto ini_stat = util::Logger::init_default(util::log_level::trace, false, true);
		if (!ini_stat) {
			std::cout << "fail to init default logger" << ini_stat.error().message();
			return EXIT_FAILURE;
		}
		boost::asio::io_context		   io_ctx;
		boost::asio::ip::tcp::resolver resolver(io_ctx);
		const std::string_view		   sv[] = {
			"www.baidu.com",
			"182.61.200.6",
			"103.235.47.188",
			"182.61.200.7",
			"103.235.47.96",
			"31.13.73.9",
		};
		std::vector<tcp::endpoint> eps;
		for (const auto i : sv) {
			const auto eps_in = resolver.resolve(i, "http");
			std::copy(eps_in.begin(), eps_in.end(), std::back_inserter(eps));
		}

		for (const auto &ep : eps) {
			boost::asio::co_spawn(io_ctx, async_connect(ep), boost::asio::detached);
		}
		io_ctx.run();
	} catch (const std::exception &e) {
		LOG_MSG(error) << fmt::format("catch exception: {}", e.what());
		const boost::stacktrace::stacktrace *st = boost::get_error_info<util::exception::traced>(e);
		if (st) {
			LOG_MSG(error) << "trace: \n"
						   << *st;
		} else {
			LOG_MSG(error) << "stacktrace:\n"
						   << boost::stacktrace::stacktrace::from_current_exception();
		}
	} catch (...) {
		LOG_MSG(error) << "Catch unknown exception ...";
		LOG_MSG(error) << "stacktrace:\n"
					   << boost::stacktrace::stacktrace::from_current_exception();
	}
	return 0;
}