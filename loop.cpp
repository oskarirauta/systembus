#include <thread>

#include "counter.hpp"
#include "cpu.hpp"
#include "security.hpp"
#include "info.hpp"
#include "mutex.hpp"
#include "ubus.hpp"
#include "loop.hpp"

bool Loop::sig_exit(void) {

	std::lock_guard<std::mutex> guard(this -> sig_mutex);
	return this -> _sig_exit;
}

bool Loop::running(void) {

	std::lock_guard<std::mutex> guard(this -> sig_mutex);
	return this -> _running;
}

int Loop::delay(void) {

	std::lock_guard<std::mutex> guard(this -> sig_mutex);
	return this -> _delay;
}

void Loop::set_sig_exit(bool state) {

	std::lock_guard<std::mutex> guard(this -> sig_mutex);
	this -> _sig_exit = state;
}

void Loop::set_running(bool state) {

	std::lock_guard<std::mutex> guard(this -> sig_mutex);
	this -> _running = state;
}

void Loop::set_delay(int delay) {

	std::lock_guard<std::mutex> guard(this -> sig_mutex);
	this -> _delay = delay;
}

void Loop::sleep(int ms) {

	std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

void Loop::run(void) {

	if ( this -> running())
		return;

	bool minimalSleep = false;

	this -> set_running(true);
	int __delay = this -> delay();

	this -> sleep((int)(__delay * 0.5));

	while ( !this -> sig_exit()) {

		update_cpu(); // Update cpu on each cycle

		if ( Counter::cycle(COUNTER_SECURITY))
			update_security();
		else if ( Counter::cycle(COUNTER_INFO))
			update_info();
		else if ( Counter::cycle(COUNTER_APP_SETTINGS))
			__delay = this -> delay();

		this -> sleep(__delay);
	}

	this -> set_running(false);

}

Loop main_loop;

void run_main_loop(void) {
	main_loop.run();
}
