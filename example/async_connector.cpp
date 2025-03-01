#include "util/exception.hpp"
#include <boost/asio.hpp>
#include <boost/stacktrace.hpp>
#include <spdlog/spdlog.h>
#include "util/util.hpp"

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
				SPDLOG_ERROR("catch exception: {}", e.what());
			} catch (...) {
				SPDLOG_ERROR("catch unknown exception");
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
				SPDLOG_ERROR("resolve host {} failed: {}", hostname, ec.message());
				return;
			}
			for (const auto &i : results) {
				tcp::socket socket(_context);
				const auto	conn_start_time = std::chrono::system_clock::now();
				socket.async_connect(i, std::bind(&AysncConnector::_handle_connect, this, boost::asio::placeholders::error, conn_start_time, i));
				_sockets.push_back(std::move(socket));
			}
		} catch (const std::exception &e) {
			SPDLOG_ERROR("catch exception: {}", e.what());
		} catch (...) {
			SPDLOG_ERROR("catch unknown exception");
		}
	}

	void _handle_connect(boost::system::error_code ec, std::chrono::time_point<std::chrono::system_clock> conn_start_time, const tcp::endpoint &ep) {
		try {
			if (ec) {
				SPDLOG_ERROR("connection to {} failed: {}", ep.address().to_string(), ec.message());
				return;
			}
			const auto now = std::chrono::system_clock::now();
			SPDLOG_ERROR("connection takes: {}", std::chrono::duration_cast<std::chrono::milliseconds>(now - conn_start_time).count());
		} catch (const std::exception &e) {
			SPDLOG_ERROR("catch exception: {}", e.what());
		} catch (...) {
			SPDLOG_ERROR("catch unknown exception");
		}
	}

private:
	boost::asio::io_context	  _context;
	tcp::resolver			  _resolver;
	std::vector<tcp::socket>  _sockets;
	boost::asio::steady_timer _tm_timeout;
};

int main() {
	try {
		util::init_log(spdlog::level::trace);
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
		const boost::stacktrace::stacktrace *st = boost::get_error_info<util::exception::traced>(e);
		std::stringstream ss;
		ss << (st ? *st : boost::stacktrace::stacktrace::from_current_exception());
		SPDLOG_ERROR("Catch exception: {}, trace: \n {}", e.what(), ss.str());
	} catch (...) {
		std::stringstream ss;
		ss << boost::stacktrace::stacktrace::from_current_exception();
		SPDLOG_ERROR("Catch unknown exception ... trace: {}", ss.str());
	}
}