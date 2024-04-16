#include "common.hpp"
#include "logger.hpp"
#include "json.hpp"
#include "ubus.hpp"
#include "ubus_cpu.hpp"

int ubus_cpu_get(const std::string& method, const JSON& req, JSON& res) {

	logger::debug << "cpu::get called" << std::endl;

	cpu_t::node_t *node = nullptr;
	std::string key = "";
	bool fail = false;

	if ( req.empty() || !req.contains("key")) {

		logger::verbose << "incorrect args for cpu::get" << std::endl;
		res["error"] = "cannot call systembus.cpu get without fields key and possibly id/number";
		return 0;
	}

	if (( req.contains("key") && req["key"].convertible_to(JSON::STRING)) ||
	    ( req.contains("name") && req["name"].convertible_to(JSON::STRING))) {

		key = req.contains("key") && req["key"].convertible_to(JSON::STRING) ?
					common::to_lower(req["key"]) : common::to_lower(req["name"]);
	}

	if ( !key.empty() && (( req.contains("number") && req["number"].convertible_to(JSON::INT)) ||
		   ( req.contains("id") && req["id"].convertible_to(JSON::INT)))) {

		int n = req.contains("number") && req["number"].convertible_to(JSON::INT) ? req["number"] : req["id"];

		try {
			node = &(*cpu)[n];
		} catch (...) {

			logger::verbose << "incorrect cpu number " << n << " for cpu::get, out of bounds" << std::endl;
			res["error"] = "cpu number " + std::to_string(n) + " out of bounds";
			return 0;
		}
	}

	if ( !key.empty() && node == nullptr ) {

		if ( key == "load" )
			res["load"] = (size_t)cpu -> load();
		else if ( key == "temp" || key == "temperature" )
			res["temp"] = (size_t)cpu -> temp();
		else if ( key == "tempmax" || key == "temp_max" || key == "max_temp" || key == "maxtemp" || key == "maxtemperature" )
			res["temp_max"] = (size_t)cpu -> temp_max();
		else {

			try {
				res[key] = (*cpu)[key];
			} catch (...) {
				fail = true;
			}
		}

	} else if ( !key.empty() && node != nullptr ) {

		if ( key == "load" )
			res["load"] = (size_t)node -> load();
		else if ( key == "temp" || key == "temperature" )
			res["temp"] = (size_t)node -> temp();
		else if ( key == "tempmax" || key == "temp_max" || key == "max_temp" || key == "maxtemp" || key == "maxtemperature" )
			res["temp_max"] = (size_t)node -> temp_max();
		else {

			try {
				res[key] = (*node)[key];
			} catch (...) {
				fail = "true";
			}
		}

	} else fail = "true";

	if ( fail ) {

		logger::verbose << "cpu::get call failed, missing fields key and/or id/number" << std::endl;
		res = JSON::Object();
		if ( key.empty())
			res["error"] = "cannot call systembus.cpu get without fields key and possibly id/number";
		else res["error"] = "key " + std::string(res["key"]) + " is not supported";
	}

	return 0;
}

int ubus_cpu_count(const std::string& method, const JSON& req, JSON& res) {

	logger::debug << "cpu::count called" << std::endl;

	res["cores"] = (size_t)cpu -> cores();
	res["count"] = (size_t)cpu -> size();

	return 0;
}

int ubus_cpu_load(const std::string& method, const JSON& req, JSON& res) {

	logger::debug << "cpu::load called" << std::endl;

	res["load"] = (size_t)cpu -> load();
	return 0;
}

int ubus_cpu_temp(const std::string& method, const JSON& req, JSON& res) {

	logger::debug << "cpu::temp called" << std::endl;

	res["temperature"] = (size_t)cpu -> temp();
	res["temperature_max"] = (size_t)cpu -> temp_max();
	return 0;
}

int ubus_cpu_all(const std::string& method, const JSON& req, JSON& res) {

	logger::debug << "cpu::all called" << std::endl;

	res["count"] = (size_t)cpu -> size();
	res["cores"] = (size_t)cpu -> cores();
	res["load"] = (size_t)cpu -> load();
	res["temperature"] = (size_t)cpu -> temp();
	res["temperature_max"] = (size_t)cpu -> temp_max();

	for ( size_t i = 0; i < cpu -> size(); i++ ) {

		cpu_t::node_t* node = &((*cpu)[i]);
		JSON obj;
		obj["id"] = (size_t)node -> number();
		obj["core"] = (size_t)node -> core();
		obj["load"] = (size_t)node -> load();
		obj["temperature"] = (size_t)node -> temp();
		obj["temperature_max"] = (size_t)node -> temp_max();

		res[node -> id()] = obj;
	}

	return 0;
}

int ubus_cpu_list(const std::string& method, const JSON& req, JSON& res) {

	logger::debug << "cpu::list called" << std::endl;

	res["cpu"] = (size_t)cpu -> load();

	for ( size_t i = 0; i < cpu -> size(); i++ ) {
		cpu_t::node_t* node = &((*cpu)[i]);
		res[node -> id()] = (size_t)node -> load();
	}

	return 0;
}
