all: world

CXX?=g++
CXXFLAGS?=--std=c++17 -Wall -fPIC -g
LDFLAGS?=-L/lib -L/usr/lib

INCLUDES+= -I./include

OBJS:= \
	objs/ubus_cpu.o \
	objs/ubus_info.o \
	objs/ubus_security.o \
	objs/ubus_bandwidth.o \
	objs/settings.o \
	objs/main.o

include common/Makefile.inc
include throws/Makefile.inc
include logger/Makefile.inc
include usage/Makefile.inc
include json/Makefile.inc
include cpu/Makefile.inc
include uptime/Makefile.inc
include kernel/Makefile.inc
include bandwidth/Makefile.inc
include ubus/Makefile.inc

world: systembus

$(shell mkdir -p objs)

objs/ubus_cpu.o: src/ubus_cpu.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<;

objs/ubus_info.o: src/ubus_info.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<;

objs/ubus_security.o: src/ubus_security.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<;

objs/ubus_bandwidth.o: src/ubus_bandwidth.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<;

objs/settings.o: src/settings.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<;

objs/main.o: main.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<;

systembus: $(COMMON_OBJS) $(THROWS_OBJS) $(LOGGER_OBJS) $(USAGE_OBJS) $(UPTIME_OBJS) $(JSON_OBJS) $(CPU_OBJS) $(KERNEL_OBJS) $(UBUS_OBJS) $(BANDWIDTH_OBJS) $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@;

.PHONY: clean
clean:
	@rm -rf objs systembus
