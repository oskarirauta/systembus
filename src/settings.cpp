#include "settings.hpp"

extern "C" {
#include <uci.h>
#include <uci_blob.h>
#include <libubox/blobmsg_json.h>
}

static blob_buf b;

enum {
	CFG_RELEASE,
	CFG_MONITORING,
	__CFG_MAX
};

static const struct blobmsg_policy config_policy[__CFG_MAX] = {
	[CFG_RELEASE] = { .name = "openwrt_release", .type = BLOBMSG_TYPE_STRING },
	[CFG_MONITORING] = { .name = "bandwidth_monitoring", .type = BLOBMSG_TYPE_BOOL },
};

static const struct uci_blob_param_list config_attr_list = {
	.n_params = __CFG_MAX,
	.params = config_policy,
};

static void parse_options(uci_context *ctx, uci_package *p, settings::config& cfg) {

	//bool release_found = false;
	bool monitoring_found = false;

	uci_element *e;
	uci_foreach_element(&p -> sections, e) {

		uci_section *s = uci_to_section(e);
		if ( std::string(s -> type) != "system" )
			continue;

		blob_attr *tb[__CFG_MAX] = { 0 };
		blob_buf_init(&b, 0);
		uci_to_blob(&b, s, &config_attr_list);
		blobmsg_parse(config_policy, __CFG_MAX, tb, blob_data(b.head), blob_len(b. head));

		if ( blob_attr* attr = tb[CFG_RELEASE] ) {

			cfg.release_name = blobmsg_get_string(attr);
			//release_found = true;
		}

		if ( tb[CFG_MONITORING] ) {

			cfg.monitoring = (int)blobmsg_get_u8(tb[CFG_MONITORING]) == 0 ? false : true;
			monitoring_found = true;
		}

		blob_buf_free(&b);

		// uci refuses to store empty string..
		/*
		if ( !release_found ) {

			uci_ptr ptr = {
				.p = s -> package,
				.s = s,
				.o = nullptr,
				.option = config_policy[CFG_RELEASE].name,
				.value = 0
			};

			if ( uci_lookup_ptr(ctx, &ptr, NULL, false); !ptr.o ) {
				uci_set(ctx, &ptr);
				uci_save(ctx, s -> package);
			}
		}
		*/
		if ( !monitoring_found ) {

			uci_ptr ptr = {
				.p = s -> package,
				.s = s,
				.o = nullptr,
				.option = config_policy[CFG_MONITORING].name,
				.value = "0"
			};

			if ( uci_lookup_ptr(ctx, &ptr, NULL, false); !ptr.o ) {
				uci_set(ctx, &ptr);
				uci_save(ctx, s -> package);
			}
		}

		if ( /*!release_found ||*/ !monitoring_found )
			uci_commit(ctx, &s -> package, false);

		break;
	}

}

settings::config settings::load() {

	uci_context *uci_ctx;
	settings::config cfg = { .release_name = "", .monitoring = false };

	if ( uci_ctx = uci_alloc_context(); !uci_ctx )
		return cfg;

	uci_package *p = nullptr;
	if ( uci_load(uci_ctx, "system", &p); !p ) {
		uci_free_context(uci_ctx);
		return cfg;
	}

	parse_options(uci_ctx, p, cfg);
	uci_free_context(uci_ctx);
	return cfg;
}
