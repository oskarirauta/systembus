#pragma once

#include <string>

extern "C" {
#include <uci.h>
}

class info_t {
	public:
		std::string release_name;
		std::string boot_variant = "";

		info_t(void);
		void update(void);

	private:

		std::string parse_release_name(struct uci_package *p);
		std::string get_release_name(void);
		void update_boot_variant(void);
};

extern info_t *info_data;

void update_info(void);
