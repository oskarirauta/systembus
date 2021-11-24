#pragma once

typedef enum {
	COUNTER_SECURITY = 0,
	COUNTER_INFO,
	COUNTER_APP_SETTINGS
} counter_type;

namespace Counter {

	struct Cycles {
		static const int security = 30;
		static const int info = 20;
		static const int app_settings = 35;
	};

	struct Store {

		public:
			int security = 0;
			int info = 0;
			int app_settings = 0;

			Store(void) {};
			Store(Counter::Store *copy);
	};

	extern Counter::Store store;

	bool cycle(counter_type counter);
	int get(counter_type counter);
}
