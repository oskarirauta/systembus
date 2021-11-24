#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unistd.h>

#include "common.hpp"
#include "log.hpp"
#include "process.hpp"

// Based on
// https://github.com/fho/code_snippets/blob/master/c/getusage.c

Process::Stat::Stat(const pid_t pid, bool update) {

	this -> pid = pid;
	this -> failed = true;

	if ( update ) this -> update();
}

Process::Stat::Stat(void) {

	this -> pid = 0;
	this -> failed = true;
}

void Process::Stat::update(void) {

	this -> failed = true;

	if ( this -> pid == 0 ) { // Failure
		log::vverbose << "error: cannot get stats for pid 0" << std::endl;
		return;
	}

	std::ifstream pstatfd("/proc/" + std::to_string(pid) + "/stat");

	if ( !pstatfd.is_open()) { // Failure
		log::debug << "error: process pid " << std::to_string(pid) << " does not exist." << std::endl;
		log::vverbose << "error: failed to open /proc/" << std::to_string(pid) << "/stat" << std::endl;
		return;
	}

	std::string line;

	if ( !std::getline(pstatfd, line)) { // Failure
		log::vverbose << "error: failed to read from /proc/" << std::to_string(pid) << "/stat" << std::endl;
		pstatfd.close();
		return;
	}

	pstatfd.close();

	std::vector<std::string> words = common::split(common::to_lower(line), ' ');
	if ( words.empty() || words.size() < 24 || words[1].size() < 3 ) { // Failure
		log::vverbose << "error: failed to parse /proc/" << std::to_string(pid) << "/stat" << std::endl;
		return;
	}

	this -> name = words[1].substr(1, words[1].size() - 2);

	try {
		this -> utime_ticks = std::stoul(words[13]);
	} catch(std::exception &e) { // Failure
		log::vverbose << "error: failed to read pid's utime ticks[13] = " << words[13] << std::endl;
		return;
	}

	try {
		this -> stime_ticks = std::stoul(words[14]);
	} catch(std::exception &e) { // Failure
		log::vverbose << "error: failed to read pid's stime ticks[14] = " << words[14] << std::endl;
		return;
	}

	try {
		this -> cutime_ticks = std::stol(words[15]);
	} catch(std::exception &e) { // Failure
		log::vverbose << "error: failed to read pid's cutime ticks[15] = " << words[15] << std::endl;
		return;
	}

	try {
		this -> cstime_ticks = std::stol(words[16]);
	} catch(std::exception &e) { // Failure
		log::vverbose << "error: failed to read pid's cstime ticks[16] = " << words[16] << std::endl;
		return;
	}

	try {
		this -> vsize = std::stoul(words[22]);
	} catch(std::exception &e) { // Failure
		log::vverbose << "error: failed to read pid's vsize[22] = " << words[22] << std::endl;
		return;
	}

	try {
		this -> rss = std::stoul(words[23]) * getpagesize();
	} catch(std::exception &e) { // Failure
		log::vverbose << "error: failed to read pid's rss[23] = " << words[23] << std::endl;
		return;
	}

	pstatfd.close();

	std::ifstream statfd("/proc/stat");
	if ( !statfd.is_open()) { // Failure
		log::vverbose << "error: failed to open /proc/stat" << std::endl;
		return;
	}

	if ( !std::getline(statfd, line)) { // Failure
		log::vverbose << "error: failed to read from /proc/stat" << std::endl;
		statfd.close();
		return;
	}

	statfd.close();

	words = common::split(common::to_lower(line), ' ');
	if ( words.empty() || words.size() < 11 ) { // Failure
		log::vverbose << "error: failed to parse /proc/stat" << std::endl;
		return;
	}

	long unsigned int cpu_time;
	this -> cpu_total_time = 0;

	for ( int i = 0; i < 10; i++ ) {
		try {
			cpu_time = std::stoul(words[i + 1]);
		} catch(std::exception &e) { // Failure
			log::vverbose << "error: failed to read value " << i + 1 << " = " << words[i + 1] << " from /proc/stat" << std::endl;
			return;
		}
		this -> cpu_total_time += cpu_time;
	}

	statfd.close();
	this -> failed = false;
}

double Process::Stat::usage(const struct Process::Stat *prev) {

	if ( this -> failed || prev -> failed ) return -1;

	const long unsigned int total_time_diff = this -> cpu_total_time - prev -> cpu_total_time;
	long int v = ( this -> utime_ticks + this -> cutime_ticks ) -
			( prev -> utime_ticks + prev -> cutime_ticks);
	return 100 * ( v / (double) total_time_diff );
}
