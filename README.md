# systembus
systembus is a tool to retrieve some statistics over ubus.
It is written in C++ except for ubus parts which use externed C.

# statistics
 - cpu statistics including cpu load(s)
 - details of enabled security modules, support includes:
	- seccomp
	- apparmor
	- selinux (if variant enabled)
 - retrieval of custom release name

# use cases
systembus was created to assist LuCi to show nicer details on
cpu load(s). Afterwards I added rest of functions.

# requirements
openwrt system or other system with ubus, libraries for using
ubus, jsoncpp and possible libselinux if selinux variant is chose.

# License
MIT

