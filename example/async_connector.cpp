#include "util/exception.hpp"
#include "util/log.hpp"

#include <fmt/format.h>
#include <boost/asio.hpp>
#include <boost/stacktrace.hpp>

class AysncConnector {
	using tcp									   = boost::asio::ip::tcp;
	static constexpr std::chrono::seconds time_out = std::chrono::seconds(3);

public:
	AysncConnector() :
			_resolver(_context), _tm_timeout(_context){};

	void async_connect(const std::vector<std::string> &eps) {
		_tm_timeout.expires_after(time_out);
		_tm_timeout.async_wait([this](boost::system::error_code ec) {
			try {
				if (!ec) {
					_resolver.cancel();
					for (auto &s : _sockets) {
						s.close();
					}
					_context.stop();
				}
			} catch (const std::exception &e) {
				LOG_MSG(error) << "catch exception: " << e.what();
			} catch (...) {
				LOG_MSG(error) << "catch unknown exception";
			}
		});
		for (const auto &i : eps) {
			_resolver.async_resolve(i, "80",
					std::bind(&AysncConnector::_handle_resolve, this,
							boost::asio::placeholders::error, boost::asio::placeholders::results, i));
		}
	}

	void run_context() {
		_context.run();
	}

private:
	void _handle_resolve(boost::system::error_code ec, const tcp::resolver::results_type &results, std::string hostname) {
		try {
			if (ec) {
				LOG_MSG(error) << fmt::format("resolve host {} failed: {}", hostname, ec.message());
				return;
			}
			for (const auto &i : results) {
				tcp::socket socket(_context);
				const auto	conn_start_time = std::chrono::system_clock::now();
				socket.async_connect(i, std::bind(&AysncConnector::_handle_connect, this, boost::asio::placeholders::error, conn_start_time, i));
				_sockets.push_back(std::move(socket));
			}
		} catch (const std::exception &e) {
			LOG_MSG(error) << "catch exception: " << e.what();
		} catch (...) {
			LOG_MSG(error) << "catch unknown exception";
		}
	}

	void _handle_connect(boost::system::error_code ec, std::chrono::time_point<std::chrono::system_clock> conn_start_time, const tcp::endpoint &ep) {
		try {
			if (ec) {
				LOG_MSG(error) << fmt::format("connection to {} failed: {}", ep.address().to_string(), ec.message());
				return;
			}
			const auto now = std::chrono::system_clock::now();
			LOG_MSG(info) << "connection takes: " << std::chrono::duration_cast<std::chrono::milliseconds>(now - conn_start_time);
		} catch (const std::exception &e) {
			LOG_MSG(error) << "catch exception: " << e.what();
		} catch (...) {
			LOG_MSG(error) << "catch unknown exception";
		}
	}

private:
	boost::asio::io_context	  _context;
	tcp::resolver			  _resolver;
	std::vector<tcp::socket>  _sockets;
	boost::asio::steady_timer _tm_timeout;
};

int main() {
	const auto ini_stat = util::Logger::init_default(util::log_level::trace, false, true);
	if (!ini_stat) {
		std::cout << "fail to init default logger" << ini_stat.error().message();
		return EXIT_FAILURE;
	}
	try {
		std::vector<std::string> ip_list = {
			// "www.google.com",
			"www.baidu.com",
			"182.61.200.7",
			"182.61.200.6",
		};

		AysncConnector connector;
		connector.async_connect(ip_list);
		connector.run_context();
	} catch (const std::exception &e) {
		LOG_MSG(error) << "Catch exception: " << e.what();
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
}