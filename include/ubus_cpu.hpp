#pragma once

#include <string>
#include "json.hpp"
#include "cpu/cpu.hpp"

extern cpu_t* cpu;

int ubus_cpu_get(const std::string& method, const JSON& req, JSON& res);
int ubus_cpu_count(const std::string& method, const JSON& req, JSON& res);
int ubus_cpu_load(const std::string& method, const JSON& req, JSON& res);
int ubus_cpu_temp(const std::string& method, const JSON& req, JSON& res);
int ubus_cpu_all(const std::string& method, const JSON& req, JSON& res);
int ubus_cpu_list(const std::string& method, const JSON& req, JSON& res);
