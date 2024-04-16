#include <sstream>

extern "C" {
#include <uci.h>
}

#include "common.hpp"
#include "logger.hpp"
#include "json.hpp"
#include "settings.hpp"
#include "ubus.hpp"
#include "ubus_security.hpp"
#include "ubus_info.hpp"

int ubus_info_uptime(const std::string& method, const JSON& req, JSON& res) {

	logger::debug << "info::uptime called" << std::endl;

	std::string key = !req.empty() ? ( req.contains("key") ? req["key"] : "" ) : "";

	if ( key.empty()) {

		auto data = uptime -> data();
		res["uptime"] = ( data.days > 0 ? ( std::to_string(data.days) + " days " ) : "" ) +
					std::to_string(data.hours) + " hours " +
					std::to_string(data.minutes) + " minutes";
		res["days"] = data.days;
		res["hours"] = data.hours;
		res["minutes"] = data.minutes;

	} else {

		std::string key = req.contains("key") ? req["key"] : "";

		if ( key.empty()) {

			auto data = uptime -> data();
			res["uptime"] = ( data.days > 0 ? ( std::to_string(data.days) + " days " ) : "" ) +
					std::to_string(data.hours) + " hours " +
					std::to_string(data.minutes) + " minutes";

		} else if ( key == "days" )
			res["days"] = (size_t)uptime -> days();
		else if ( key == "hours" )
			res["hours"] = (size_t)uptime -> hours();
		else if ( key == "minutes" )
			res["minutes"] = (size_t)uptime -> minutes();
		else if ( key == "seconds" )
			res["seconds"] = (size_t)uptime -> seconds();
		else res["error"] = "key " + key + " is not supported, supported keys are days, hours, minutes and seconds";
	}

        return 0;
}

int ubus_info_loaded(const std::string& method, const JSON& req, JSON& res) {

	logger::debug << "info::loaded called" << std::endl;

	res["systembus_loaded"] = true;
	return 0;
}

int ubus_info_release(const std::string& method, const JSON& req, JSON& res) {

	logger::debug << "info::release called" << std::endl;

	res["release_name"] = settings::load().release_name;
	return 0;
}

int ubus_info_variant(const std::string& method, const JSON& req, JSON& res) {

	logger::debug << "info::variant called" << std::endl;

	res["variant"] = "";
	for ( const auto& p : kernel -> pairs()) {
		if ( common::to_lower(p.first) == "env.boot_variant" ) {
			res["variant"] = p.second;
			break;
		}
	}

	return 0;
}

int ubus_info_list(const std::string& method, const JSON& req, JSON& res) {

	logger::debug << "info::list called" << std::endl;

	auto data = uptime -> data();
	res["uptime"] = ( data.days > 0 ? ( std::to_string(data.days) + " days " ) : "" ) +
				std::to_string(data.hours) + " hours " +
				std::to_string(data.minutes) + " minutes";
	res["systembus_loaded"] = true;
	res["release_name"] = settings::load().release_name;
	res["variant"] = "";
	for ( const auto& p : kernel -> pairs()) {
		if ( common::to_lower(p.first) == "env.boot_variant" ) {
			res["variant"] = p.second;
			break;
		}
	}

	return 0;
}
