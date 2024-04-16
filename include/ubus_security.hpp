#pragma once

#include <string>
#include "kernel_info.hpp"

extern kernel_info* kernel;

int ubus_security_list(const std::string& method, const JSON& req, JSON& res);
int ubus_security_selinux(const std::string& method, const JSON& req, JSON& res);
int ubus_security_apparmor(const std::string& method, const JSON& req, JSON& res);
int ubus_security_seccomp(const std::string& method, const JSON& req, JSON& res);

