#pragma once

#include <string>
#include <map>

class kcmdline {
	public:
		std::string path(void);
		std::string cmdline(void);
		std::map<std::string, std::string> pairs(void);
		bool io_error(void);

		kcmdline(std::string path = "/proc/cmdline");
	private:
		std::string _path;
		std::string _cmdline;
		std::map<std::string, std::string> _pairs;
		bool _io_error = false;

		void parse(void);
};
