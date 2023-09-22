# export PROJECT_DIR:=C:/Users/Andrew/Documents/VSCode/autonomous
# export TOOLCHAIN_DIR:=C:/Users/Andrew/Documents/compilers/SysGCC/bin
export CC:=arm-linux-gnueabihf-gcc
export CXX:=arm-linux-gnueabihf-g++
export LD:=arm-linux-gnueabihf-ld
CFLAGS:=
CXXFLAGS:=
CPPFLAGS:=-I"include" -std=c++11 -Llib
LDFLAGS:=
LDLIBS:=
export BUILD_DIR:=build

BACKEND_SRC:=${wildcard ./src/backend/*.cpp}
BACKEND_BUILD:=${patsubst ./src/backend/%.cpp,build/%.o,${BACKEND_SRC}}

# CC -- the C compiler to use
# CXX -- the C++ compiler to use
# LD -- the linker to use
# CFLAGS -- compilation flag for C source files
# CXXFLAGS -- compilation flags for C++ source files
# CPPFLAGS -- flags for the c-preprocessor (typically include file paths and symbols defined on the command line), used by C and C++
# LDFLAGS -- linker flags
# LDLIBS -- libraries to link

.PHONY: all clean adiru auto_com motor_controller

all: bin/hello bin/pressure bin/adiru bin/auto_com bin/motor_controller bin/gps_test bin/mag_test

build:
	mkdir build

${BACKEND_BUILD}: ${BUILD_DIR}/%.o: ./src/backend/%.cpp
	$(CXX) ${CPPFLAGS} -c $^ -o $@

${patsubst ./src/util/%.cpp,build/%.o,${wildcard ./src/util/*.cpp}}: ${BUILD_DIR}/%.o: ./src/util/%.cpp
	$(CXX) ${CPPFLAGS} -c $^ -o $@

${patsubst ./src/math/%.cpp,build/%.o,${wildcard ./src/math/*.cpp}}: ${BUILD_DIR}/%.o: ./src/math/%.cpp
	$(CXX) ${CPPFLAGS} -c $^ -o $@

build/fmt.o: src/format.cc
	$(CXX) ${CPPFLAGS} -c $^ -o $@

adiru: bin/adiru
bin/adiru: build/fmt.o build/log.o src/adiru.cpp build/i2c.o build/bmp390.o build/timer.o build/filter.o build/mpu6050.o lib/libpaho-mqtt3a.so lib/libpaho-mqttpp3.so build/math.o
	${CXX} ${CPPFLAGS} -pthread $^ -o $@

auto_com: bin/auto_com
bin/auto_com: build/fmt.o build/log.o src/guidance/auto_com.cpp lib/libpaho-mqtt3a.so build/str_utils.o lib/libpaho-mqttpp3.so build/math.o
	${CXX} ${CPPFLAGS} -pthread $^ -o $@

motor_controller: bin/auto_com
bin/motor_controller: build/fmt.o build/log.o src/motor_controller.cpp build/str_utils.o build/i2c.o build/pca9685.o lib/libpaho-mqtt3a.so lib/libpaho-mqttpp3.so build/math.o
	${CXX} ${CPPFLAGS} -pthread $^ -o $@


bin/gps_test: src/gps_test.cpp build/str_utils.o 
	${CXX} ${CPPFLAGS} -pthread $^ -o $@

bin/mag_test: src/mag_test.cpp build/i2c.o build/qmc5883.o lib/libpaho-mqtt3a.so lib/libpaho-mqttpp3.so 
	${CXX} ${CPPFLAGS} -pthread $^ -o $@

bin/pressure: src/tests/pressure.cpp build/i2c.o build/bmp390.o build/timer.o build/filter.o
	${CXX} ${CPPFLAGS} -pthread $^ -o $@

bin/hello: src/tests/hello.cpp
	$(CXX) $^ -o $@ -static

clean: 
	rd -r ${BUILD_DIR}