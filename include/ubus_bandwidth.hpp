#pragma once

#include <string>
#include "bandwidth.hpp"

extern bandwidth_t* bm;

int ubus_bandwidth_get(const std::string& method, const JSON& req, JSON& res);
int ubus_bandwidth_list(const std::string& method, const JSON& req, JSON& res);
