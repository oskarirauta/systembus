#pragma once

#include <mutex>
#include <thread>

typedef void (*die_handler_func)(int);

struct MutexStore {

	public:

		std::mutex counter;
		std::mutex cpu, security, info;

		MutexStore(die_handler_func die_handler);
};

extern MutexStore mutex;
