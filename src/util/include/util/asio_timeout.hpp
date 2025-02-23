#pragma once
#include <boost/asio.hpp>
struct timeout_provider;

// that's our completion token with the timeout attached
template <typename Token>
struct with_timeout {
	timeout_provider *provider;
	Token			  token;
};

// this is the timeout source
struct timeout_provider {
	timeout_provider(boost::asio::any_io_executor exec,
			std::chrono::milliseconds			  tt_total,
			std::chrono::milliseconds			  tt_partial,
			std::chrono::milliseconds			  tt_terminal) :
			tim{ exec, std::chrono::steady_clock::time_point::max() },
			_tt_total(tt_total),
			_tt_partial(tt_partial),
			_tt_terminal(tt_terminal),
			emit_all(false) {}

	timeout_provider(boost::asio::any_io_executor exec,
			std::chrono::milliseconds			  tt_total) :
			tim{ exec, std::chrono::steady_clock::time_point::max() },
			_tt_total(tt_total),
			emit_all(true) {}

	boost::asio::steady_timer tim;

	std::chrono::milliseconds _tt_total;
	std::chrono::milliseconds _tt_partial;
	std::chrono::milliseconds _tt_terminal;

	bool emit_all;

	boost::asio::cancellation_slot	 parent;
	boost::asio::cancellation_signal timeout;

	boost::asio::cancellation_type last_fired{ boost::asio::cancellation_type::none };

	~timeout_provider() {
		if (parent.is_connected()) {
			parent.clear();
		}
	}

	// to use it
	template <typename Token>
	auto operator()(Token &&token) {
		return with_timeout<std::decay_t<Token>>{
			this, std::forward<Token>(token)
		};
	}

	// set up the timer and get ready to trigger
	void arm() {
		last_fired = boost::asio::cancellation_type::none;
		tim.expires_after(_tt_total);
		if (parent.is_connected()) {
			parent.assign([this](boost::asio::cancellation_type ct) { timeout.emit(ct); });
		}
		tim.async_wait(
				[this](boost::system::error_code ec) {
					if (!ec) {
						emit_all ? fire_total() : fire_all();
					}
				});
	}

	void fire_total() {
		timeout.emit(last_fired = boost::asio::cancellation_type::total);
		tim.expires_after(_tt_partial);
		tim.async_wait(
				[this](boost::system::error_code ec) {
					if (!ec) {
						fire_partial();
					}
				});
	}

	void fire_partial() {
		timeout.emit(last_fired = boost::asio::cancellation_type::partial);
		tim.expires_after(_tt_terminal);
		tim.async_wait(
				[this](boost::system::error_code ec) {
					if (!ec) {
						fire_terminal();
					}
				});
	}

	void fire_terminal() {
		timeout.emit(last_fired = boost::asio::cancellation_type::terminal);
	}

	void fire_all() {
		timeout.emit(last_fired = boost::asio::cancellation_type::all);
	}
};

// the completion handler
// that's our completion token with the timeout attached
template <typename Handler>
struct with_timeout_binder {
	timeout_provider *provider;
	Handler			  handler;

	template <typename... Args>
	void operator()(Args &&...args) {
		//cancel the time, we're done!
		provider->tim.cancel();
		std::move(handler)(std::forward<Args>(args)...);
	}
};

namespace boost::asio {

// This is the class to specialize when implementing a completion token.
template <typename InnerToken, typename... Signatures>
struct async_result<with_timeout<InnerToken>, Signatures...> {
	using return_type = typename async_result<InnerToken, Signatures...>::return_type;

	// this wrapper goes around the inner initiation, because we need to capture their cancellation slot
	template <typename Initiation>
	struct init_wrapper {
		Initiation		  initiation;
		timeout_provider *provider;

		// the forwards to the initiation and lets us access the actual handler.
		template <typename Handler, typename... Args>
		void operator()(
				Handler &&handler,
				Args &&...args) {
			auto sl = boost::asio::get_associated_cancellation_slot(handler);
			if (sl.is_connected()) {
				provider->parent = sl;
			}
			provider->arm();
			std::move(initiation)(
					with_timeout_binder<std::decay_t<Handler>>{
							provider,
							std::forward<Handler>(handler) },
					std::forward<Args>(args)...);
		}
	};

	// the actual initiation
	template <typename Initiation, typename RawToken,
			typename... Args>
	static auto initiate(Initiation &&init,
			RawToken				&&token,
			Args &&...args) -> return_type {
		return async_result<InnerToken, Signatures...>::initiate(
				init_wrapper<std::decay_t<Initiation>>(std::forward<Initiation>(init), token.provider),
				std::move(token.token),
				std::forward<Args>(args)...);
	}
};

// forward the other associators, such as allocator & executor
template <template <typename, typename> class Associator,
		typename T, typename DefaultCandidate>
struct associator<Associator,
		with_timeout_binder<T>,
		DefaultCandidate> {
	typedef typename Associator<T, DefaultCandidate>::type type;

	static type get(const with_timeout_binder<T> &b,
			const DefaultCandidate				 &c = DefaultCandidate()) noexcept {
		return Associator<T, DefaultCandidate>::get(b.handler, c);
	}
};

// set the slot explicitly
template <typename T, typename CancellationSlot1>
struct associated_cancellation_slot<
		with_timeout_binder<T>,
		CancellationSlot1> {
	typedef boost::asio::cancellation_slot type;

	static type get(const with_timeout_binder<T> &b,
			const CancellationSlot1 & = CancellationSlot1()) noexcept {
		return b.provider->timeout.slot();
	}
};

} //namespace boost::asio
