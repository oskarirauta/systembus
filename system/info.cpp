#include <fstream>
#include <string>

#include "common.hpp"
#include "mutex.hpp"
#include "info.hpp"

info_t::info_t(void) {

	this -> update();
}

void info_t::update(void) {

	this -> release_name = this -> get_release_name();
	this -> update_boot_variant();
}

void info_t::update_boot_variant(void) {

	if ( !this -> variant_found ) {

		this -> boot_variant = this -> get_boot_variant();
		if ( this -> boot_variant == "error: boot_entry variable missing from /proc/cmdline" ) {
			this -> variant_found = true;
		} else if ( this -> boot_variant.length() >= 6 && common::to_lower(boot_variant).substr(0,6) == "error:" )
			this -> variant_found = false;
		} else { this -> variant_found = true; }
	}
}

std::string info_t::parse_release_name(struct uci_package *p) {

	struct uci_element *e;

	uci_foreach_element(&p -> sections, e) {

		struct uci_section *s = uci_to_section(e);

		if ( common::c_tostr(s -> type) != "system" )
			continue;

		struct uci_element *n;
		uci_foreach_element(&s -> options, n) {

			struct uci_option *o = uci_to_option(n);

			if ( common::c_tostr(n -> name) != "openwrt_release" ||
				o -> type != UCI_TYPE_STRING ) continue;

			return common::c_tostr(o -> v.string);
		}
	}

	return "";
}

std::string info_t::get_release_name(void) {

	struct uci_context *uci_ctx = uci_alloc_context();

	if ( !uci_ctx )
		return "";

	struct uci_package *p = NULL;
	uci_load(uci_ctx, "system", &p);

	if ( !p )
		return "";

	std::string ret = parse_release_name(p);

	uci_free_context(uci_ctx);
	return ret;
}

std::string info_t::get_boot_variant(void) {

	std::ifstream fd("/proc/boot_entry");

	if ( !fd.good())
		return "boot_entry.ko module not loaded";

	std::string res;
	
	while ( std::getline(fd, res)) {
		if ( !res.empty()) {
			break;
		}
	}

	fd.close();

	if ( res.empty()) {
		res == "unknown error";
	} else {
		res = common::trim(ret, "\r\n");
	}

	if ( res == "kernel command line is missing boot_entry variable" ||
		res == "kernel command line is empty???" ||
		res == "failed to read /proc/cmdline - is procfs mounted?" ||
		res == "unknown error" ) {
		res = "error: " + ( res == "kernel command line is missing boot_entry variable" ? "boot_entry variable missing from /proc/cmdline" : (
			res == "kernel command line is empty???" ? "parsing failed" : " can't read /proc/boot_entry"));
	}

	return res;
}

info_t *info_data;

void update_info(void) {

	std::lock_guard<std::mutex> guard(mutex.info);
	info_data -> update();
}
