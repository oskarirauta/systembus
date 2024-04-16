#pragma once

#include <string>
#include "uptime.hpp"

extern uptime_t* uptime;

int ubus_info_list(const std::string& method, const JSON& req, JSON& res);
int ubus_info_loaded(const std::string& method, const JSON& req, JSON& res);
int ubus_info_release(const std::string& method, const JSON& req, JSON& res);
int ubus_info_variant(const std::string& method, const JSON& req, JSON& res);
int ubus_info_uptime(const std::string& method, const JSON& req, JSON& res);
