#include "mutex.hpp"
#include "counter.hpp"

namespace Counter {

	Store::Store(Counter::Store *copy) {

		this -> security = copy -> security;
		this -> info = copy -> info;
		this -> app_settings = copy -> app_settings;
	}

	Counter::Store store;

	bool cycle(counter_type counter) {

		std::lock_guard<std::mutex> guard(mutex.counter);

		switch ( counter ) {
			case COUNTER_SECURITY:
				store.security++;
				if ( store.security >= Counter::Cycles::security ) {
					store.security = 0;
					return true;
				}
				break;
			case COUNTER_INFO:
				store.info++;
				if ( store.info >= Counter::Cycles::info ) {
					store.info = 0;
					return true;
				}
				break;
			case COUNTER_APP_SETTINGS:
				store.app_settings++;
				if ( store.app_settings >= Counter::Cycles::app_settings ) {
					store.security = 0;
					return true;
				}
				break;
		}
		return false;
	}

	int get(counter_type counter) {

		int ret = -1;

		std::lock_guard<std::mutex> guard(mutex.counter);

		switch ( counter ) {
			case COUNTER_SECURITY:
				ret = store.security;
				break;
			case COUNTER_INFO:
				ret = store.info;
				break;
			case COUNTER_APP_SETTINGS:
				ret = store.app_settings;
				break;
		}

		return ret;
	}

}
