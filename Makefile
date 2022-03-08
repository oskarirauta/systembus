all: systembus

CXX?=g++
CXXFLAGS?=--std=c++17
#-ggdb

OBJS:= \
	objs/process.o objs/childproc.o \
	objs/cpu.o objs/security.o objs/info.o \
	objs/ubus_cpu.o objs/ubus_security.o objs/ubus_info.o \
	objs/ubus.o \
	objs/counter.o objs/mutex.o objs/loop.o \
	objs/signal.o objs/app.o objs/main.o

SHARED_OBJS:=objs/common.o objs/log.o

LIBS:=

# for cross-building:
UBUS_LIBS:=-lubox -lblobmsg_json -lubus -luci
# for native building:
#UBUS_LIBS:=/usr/lib/libubox.a /usr/lib/libblobmsg_json.a /usr/lib/libuci.a /usr/lib/libubus.a

JSON_LIBS:=-ljsoncpp

INCLUDES:=-I./include -I.

ifdef selinux
LIBS+= -lselinux
endif

objs/common.o: shared/common.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<;

objs/app.o: shared/app.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<;

objs/mutex.o: shared/mutex.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<;

objs/signal.o: signal.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<;

objs/counter.o: shared/counter.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<;

objs/log.o: shared/log.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<;

objs/cpu.o: system/cpu.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<;

objs/security.o: system/security.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<;

objs/info.o: system/info.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<;

objs/process.o: system/process.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<;

objs/childproc.o: system/childproc.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<;

objs/ubus_cpu.o: ubus/ubus_cpu.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<;

objs/ubus_security.o: ubus/ubus_security.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<;

objs/ubus_info.o: ubus/ubus_info.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<;

objs/ubus.o: ubus/ubus.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<;

objs/loop.o: loop.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<;

objs/main.o: main.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<;

systembus: $(SHARED_OBJS) $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(SHARED_OBJS) $(OBJS) $(LIBS) $(UBUS_LIBS) $(JSON_LIBS) -o $@;

clean:
	rm -f objs/** systembus
