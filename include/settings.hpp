#include <string>

namespace settings {

	struct config {
		std::string release_name;
		bool monitoring;
	};

	settings::config load();
}
