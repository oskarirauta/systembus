#pragma once

#include <string>
#include <vector>

#include "ubus.hpp"
#include "cpu_tcks.hpp"

class cpu_node {
	public:
		int id, mhz;
		std::string name, vendor, model;
		float load;
		struct ubus_object ubus;

		cpu_node(int id, std::string name, std::string vendor, std::string model, int mhz, float load);
		void update(float load);
};

class cpu_t {
	private:
		tck_t *new_tck;
		tck_t *tcks[2];
		int index;

		void read_cpustat(tck_t *stat);
		void calculate_cpuload(tck_t *prev, tck_t *curr);

		uint64_t idle_ticks(tck_t *stat);
		uint64_t total_ticks(tck_t *stat);

	public:
		int delay;
		uint8_t count;
		float load;

		std::vector<cpu_node> nodes;

		cpu_t();
		void update(void);
		int indexOf(std::string name);
};

extern cpu_t *cpu_data;

void update_cpu(void);
void cpu_info(bool minimal = false);
