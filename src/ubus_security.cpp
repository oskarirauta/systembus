#include "common.hpp"
#include "logger.hpp"
#include "json.hpp"
#include "ubus.hpp"
#include "ubus_security.hpp"

int ubus_security_list(const std::string& method, const JSON& req, JSON& res) {

	logger::debug << "security::list called" << std::endl;

	JSON selinux = JSON::Object();
	selinux["enabled"] = kernel -> security -> selinux -> enabled();
	selinux["mode"] = kernel -> security -> selinux -> mode();

	JSON apparmor = JSON::Object();
	apparmor["enabled"] = kernel -> security -> apparmor -> enabled();
	apparmor["profiles"] = kernel -> security -> apparmor -> profile_count();

	JSON seccomp = JSON::Object();
	JSON seccomp_actions = JSON::Array();
	seccomp["enabled"] = kernel -> security -> seccomp -> enabled();

	for ( const std::string& action : kernel -> security -> seccomp -> actions())
		seccomp_actions.append(action);

	seccomp["actions"] = seccomp_actions;

	res["selinux"] = selinux;
	res["apparmor"] = apparmor;
	res["seccomp"] = seccomp;

	return 0;
}

int ubus_security_selinux(const std::string& method, const JSON& req, JSON& res) {

	logger::debug << "security::selinux called" << std::endl;

	res["enabled"] = kernel -> security -> selinux -> enabled();
	res["mode"] = kernel -> security -> selinux -> mode();
	return 0;
}

int ubus_security_apparmor(const std::string& method, const JSON& req, JSON& res) {

	logger::debug << "security::apparmor called" << std::endl;

	res["enabled"] = kernel -> security -> apparmor -> enabled();
	res["profiles"] = kernel -> security -> apparmor -> profile_count();
	return 0;
}

int ubus_security_seccomp(const std::string& method, const JSON& req, JSON& res) {

	logger::debug << "security::seccomp called" << std::endl;

	JSON actions = JSON::Array();

	for ( const std::string& action : kernel -> security -> seccomp -> actions())
		actions.append(action);

	res["enabled"] = kernel -> security -> seccomp -> enabled();
	res["actions"] = actions;
	return 0;
}
