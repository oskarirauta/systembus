#pragma once

#include <string>

extern "C" {
#include <libubox/blobmsg_json.h>
#include <libubus.h>
}

enum {
	UBUS_STOPPED = 0,
	UBUS_RUNNING,
	__UBUS_STATE_MAX
};

typedef enum {
	FIELD_ID = 0,
	FIELD_COUNT,
	FIELD_NAME,
	FIELD_VENDOR,
	FIELD_MODEL,
	FIELD_MHZ,
	FIELD_LOAD,
	NUM_FIELDS
} field_type;

extern std::string ubus_socket;
extern struct ubus_context *ctx;
extern struct blob_buf b;

int ubus_create(void);
