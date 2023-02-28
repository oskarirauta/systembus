#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <exception>

#include "common.hpp"
#include "mutex.hpp"
#include "log.hpp"
#include "cpu.hpp"

#define MAX_TEMP_VALUES 3

bool cpu_t::read_cputemp() {

	bool success = false;
	std::filesystem::path basepath = std::string("/sys/class/thermal");
	if ( !std::filesystem::is_directory(basepath))
		return false;

	std::cout << "iterating in " << basepath << std::endl;

	for (auto const& dir_entry : std::filesystem::directory_iterator{basepath}) {

		if ( dir_entry.path().string().rfind(basepath.string() + "/thermal_", 0) != 0 )
			continue;

		std::filesystem::path typepath = std::string(dir_entry.path().string() + "/type");
		std::filesystem::path temppath = std::string(dir_entry.path().string() + "/temp");

		if ( !std::filesystem::exists(typepath) || !std::filesystem::exists(temppath) ||
			!std::filesystem::is_regular_file(typepath) || !std::filesystem::is_regular_file(temppath))
			continue;

		std::fstream typefile, tempfile;
		typefile.open(typepath.string(), std::ios::in);
		if ( !typefile.is_open()) continue;
		tempfile.open(temppath.string(), std::ios::in);
		if ( !tempfile.is_open()) {
			typefile.close();
			continue;
		}

		std::string z_type, z_temp;
		if ( getline(typefile, z_type) && getline(tempfile, z_temp) && z_type == "x86_pkg_temp" ) {

			int cpu_temp = -1;
			try {
				cpu_temp = stoi(z_temp);
			} catch(std::exception &err) {
				cpu_temp = -1;
			}

			if ( cpu_temp > -1 )
				cpu_temp = cpu_temp * 0.001;
				this -> temp_arr.push_back(cpu_temp);
				if ( this -> temp_arr.size() > MAX_TEMP_VALUES )
					temp_arr.erase(temp_arr.begin());
				success = true;
		}

		typefile.close();
		tempfile.close();

		// We support only single cpu so exit loop here..
		if ( success )
			break;
	}

	return success;
}

std::string cpu_t::cpu_temp(void) {

	long int total = 0;
	char cnt = 0;

	if ( this -> temp_arr.empty())
		return "0";

	for ( int temp : this -> temp_arr ) {
		cnt++;
		total += temp;
	}

	return std::to_string(cnt > 0 ? ( total / cnt ) : total);
}
