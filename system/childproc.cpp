#include <string>
#include <vector>
#include <fstream>
#include <filesystem>

#include "common.hpp"
#include "process.hpp"

#include <iostream>

std::vector<pid_t> Process::children(pid_t pid) {

	std::vector<pid_t> results;

	for ( const auto & entry : std::filesystem::directory_iterator("/proc")) {

		if ( !entry.is_directory() || !common::is_number(entry.path().filename()))
			continue;

		std::ifstream statfd((std::string)entry.path() + "/status");
		if ( !statfd.is_open())
			continue;

		std::string line;
		std::vector<std::string> words;
		pid_t cpid = -1;
		pid_t ppid = -1;

		while ( std::getline(statfd, line)) {

			words = common::split(common::to_lower(line), ':', " \t\r\n");

			if ( words[0] == "pid" && common::is_number(words[1]))
				cpid = stoi(words[1]);
			else if ( words[0] == "ppid" && common::is_number(words[1]))
				ppid = stoi(words[1]);

			if ( cpid != -1 && ppid != -1 )
				break;
		}		

		statfd.close();

		if ( ppid == pid )
			results.push_back(cpid);
	}

	return results;

}
