[![License:MIT](https://img.shields.io/badge/License-MIT-blue?style=plastic)](LICENSE)
### systembus
systembus is a tool to retrieve some statistics over ubus.
It is written in C++. Mostly designed for my own use, but can be found useful for others
as well.

### <sub>notes</sub>
[ubus-bm](https://github.com/oskarirauta/ubus-bm) is now integrated to systembus (can be enabled
by adding boolean option `bandwidth_monitoring` to `/etc/config/system`
[ubus-bm](https://github.com/oskarirauta/ubus-bm) is now deprecated and archived and no longer developed.

## <sub>statistics</sub>
 - cpu statistics including cpu load(s) and temperatures
 - details of enabled security modules, support includes:
	- seccomp
	- apparmor
	- selinux (if variant enabled)
 - retrieval of custom release name
 - retrieval of boot.variant (from kernel command-line env.boot_variant variable)
 - uptime details
 - bandwidth monitoring

## <sub>use cases</sub>
systembus was created to assist LuCi to show nicer details on
cpu load(s). Afterwards I added more functionality.

## <sub>requirements</sub>
openwrt system or other system with ubus, libraries for using
ubus, uci and libubox - possibly libselinux if selinux support
is built-in.

## <sub>depends</sub>
systembus build uses some submodules, remember to clone with
--recursive flag, or download source tarball that already contains
necessary submodules.

List of needed submodules:

 - [bandwidth_cpp](https://github.com/oskarirauta/bandwidth_cpp)
 - [common_cpp](https://github.com/oskarirauta/common_cpp)
 - [cpu_cpp](https://github.com/oskarirauta/cpu_cpp)
 - [json_cpp](https://github.com/oskarirauta/json_cpp)
 - [kernel_cpp](https://github.com/oskarirauta/kernel_cpp)
 - [logger_cpp](https://github.com/oskarirauta/logger_cpp)
 - [throws_cpp](https://github.com/oskarirauta/throws_cpp)
 - [ubus_cpp](https://github.com/oskarirauta/ubus_cpp)
 - [uptime_cpp](https://github.com/oskarirauta/uptime_cpp)
 - [usage_cpp](https://github.com/oskarirauta/usage_cpp)
