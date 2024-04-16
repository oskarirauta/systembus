#include <iostream>
#include <unistd.h>

#include "logger.hpp"
#include "cpu/cpu.hpp"
#include "uptime.hpp"
#include "json.hpp"
#include "ubus.hpp"
#include "usage.hpp"

#include "settings.hpp"
#include "ubus_cpu.hpp"
#include "ubus_info.hpp"
#include "ubus_security.hpp"
#include "ubus_bandwidth.hpp"

cpu_t* cpu;
uptime_t* uptime;
kernel_info* kernel;
bandwidth_t* bm;
ubus* srv;

static bool rate_monitoring = false;

// update once per second
static int update_often() {

	if ( logger::loglevel() == logger::debug.id())
		logger::debug << "updating data" << std::endl;
	cpu -> update();
	if ( rate_monitoring )
		bm -> update();

	return 1000;
}

// update every 12 seconds
static int update_rare() {

	if ( logger::loglevel() == logger::debug.id())
		logger::debug << "updating rare data" << std::endl;
	kernel -> update();

	return 12000;
}

int main(int argc, char **argv) {

	usage_t usage = {
		.args = { argc, argv },
		.name = "systembus",
		.version_title = "\nversion ",
		.version = "2.0.0",
		.author_title = "author ",
		.author = "Oskari Rauta",
		.options = {
			{ "socket", { .key = "s", .word = "socket", .desc = "ubus socket path", true, "path" }},
			{ "verbose", { .key = "v", .word = "verbose", .desc = "debug mode" }},
			{ "debug", { .key = "d", .word = "debug", .desc = "debug mode" }},
			{ "quiet", { .key = "q", .word = "quiet", .desc = "quiet mode" }},
			{ "help", { .key = "h", .word = "help", .desc = "usage help" }},
			{ "version", { .word = "version", .desc = "display version" }}
		}
	};

	if ( usage["help"] ) {

		std::cout << usage << "\n\nusage:\n" << usage.help() << "\n" << std::endl;
		return 0;

	} else if ( usage["version"] ) {

		std::cout << usage.version_info() << std::endl;
		return 0;

	} else if ( !usage.args.empty()) {

		try {
			usage.validate();
		} catch ( const std::runtime_error& e ) {

			std::cout << usage << "\n\nusage:\n" << usage.help() << "\n\n" <<
					"error: " << e.what() << std::endl;
			return 1;
		}
	}

	if (( usage["debug"] && usage["quiet"] ) ||
	    ( usage["verbose"] && usage["quiet"] )) {

		std::cout << usage << "\n\nusage:\n" << usage.help() << "\n\n" <<
					"error: options --debug, --verbose and --quiet cannot be mixed" << std::endl;
		return 1;

	} else if ( ::getuid() != 0 && ::geteuid() != 0 ) {

		std::cout << usage << "\n\nerror: systembus must be executed as root" << std::endl;
		return 1;
	}

	if ( usage["debug"] || usage["verbose"] )
		std::cout << std::endl;

	if ( usage["debug"] )
		logger::loglevel(logger::debug);

	if ( usage["verbose"] || usage["debug"] )
		logger::loglevel(usage["debug"] ? logger::debug : logger::verbose);

	else if ( usage["quiet"] )
		logger::loglevel(logger::error);

	if ( !usage["quiet"] )
		std::cout << usage << std::endl;

	logger::max_log_entries = usage["debug"] ? 50 : ( usage["quiet"] ? 5 : 25 );

	rate_monitoring = settings::load().monitoring;

	if ( rate_monitoring )
		logger::debug << "bandwidth monitoring enabled" << std::endl;

	try {
		cpu = new cpu_t;
	} catch ( const std::runtime_error& e ) {
		logger::error << "failed to create cpu object: " << e.what() << std::endl;
		return 1;
	}

	try {
		uptime = new uptime_t;
	} catch ( const std::runtime_error& e ) {
		delete cpu;
		logger::error << "failed to create uptime object: " << e.what() << std::endl;
		return 1;
	}

	try {
		kernel = new kernel_info;
	} catch ( const std::runtime_error& e ) {
		delete cpu;
		delete uptime;
		logger::error << "failed to retrieve kernel information: " << e.what() << std::endl;
		return 1;
	}

	try {
		bm = new bandwidth_t;
	} catch ( const std::runtime_error& e ) {
		delete cpu;
		delete uptime;
		delete kernel;
		logger::error << "failed to construct bandwidth monitoring: " << e.what() << std::endl;
		return 1;
	}

	logger::debug << "connecting with ubus" << std::endl;

	try {
		srv = new ubus(usage["socket"] ? usage["socket"].value : "");
	} catch ( ubus::exception& e ) {
		logger::error << "error while creating ubus context:\n" << e.what() << std::endl;
		delete cpu;
		delete uptime;
		delete kernel;
		return e.code();
	}

	logger::debug << "adding object system.cpu" << std::endl;

	srv -> add_object("system.cpu", {
		{ .name = "get", .cb = ubus_cpu_get, .hints = {{ "key", JSON::TYPE::STRING }, { "id", JSON::TYPE::INT }}},
		{ .name = "count", .cb = ubus_cpu_count },
		{ .name = "load", .cb = ubus_cpu_load },
		{ .name = "temp", .cb = ubus_cpu_temp },
		{ .name = "all", .cb = ubus_cpu_all },
		{ .name = "list", .cb = ubus_cpu_list }
	});

	logger::debug << "adding object system.info" << std::endl;

	srv -> add_object("system.info", {
		{ .name = "uptime", .cb = ubus_info_uptime },
		{ .name = "systembus_loaded", .cb = ubus_info_loaded },
		{ .name = "release_name", .cb = ubus_info_release },
		{ .name = "boot_variant", .cb = ubus_info_variant },
		{ .name = "list", .cb = ubus_info_list }
	});

	logger::debug << "adding object system.security" << std::endl;

	srv -> add_object("system.security", {
		{ .name = "list", .cb = ubus_security_list },
		{ .name = "selinux", .cb = ubus_security_selinux },
		{ .name = "apparmor", .cb = ubus_security_apparmor },
		{ .name = "seccomp", .cb = ubus_security_seccomp }
	});

	if ( rate_monitoring ) {

		logger::debug << "adding object network.usage" << std::endl;

		srv -> add_object("network.usage", {
			{ .name = "get", .cb = ubus_bandwidth_get },
			{ .name = "list", .cb = ubus_bandwidth_list }
		});

	} else delete bm;

	logger::debug << "adding 2 tasks for updating data on the background" << std::endl;

	uloop::task::add(update_often, 100);
	uloop::task::add(update_rare, 200);

	logger::debug << "starting service" << std::endl;

	uloop::run();

	delete srv;
	delete cpu;
	delete uptime;
	delete kernel;

	if ( rate_monitoring )
		delete bm;

	std::cout << "exiting" << std::endl;

	return 0;
}
