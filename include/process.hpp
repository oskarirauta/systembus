#pragma once

#include <vector>

// Based on
// https://github.com/fho/code_snippets/blob/master/c/getusage.c

namespace Process {

	struct Stat {
		long unsigned int utime_ticks, stime_ticks;
		long int cutime_ticks, cstime_ticks;
		long unsigned int vsize, rss;
		long unsigned int cpu_total_time;
		std::string name;
		pid_t pid;
		bool failed;

		Stat(const pid_t pid, bool update = true);
		Stat(void);
		void update(void);
		double usage(const struct Process::Stat *prev);
	};

	std::vector<pid_t> children(pid_t pid);

}
