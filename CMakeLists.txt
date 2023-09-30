cmake_minimum_required (VERSION 3.27.6)
project(Autonomous)

file(MAKE_DIRECTORY install)

include_directories(include include/mqtt lib-Mission-Control/include)

set(CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD 23)

add_library(autonomous STATIC
    src/backend/bmp390.cpp
    src/backend/gps.cpp
    src/backend/i2c.cpp
    src/backend/mpu6050.cpp
    src/backend/pca9685.cpp
    src/backend/qmc5883.cpp

    src/math/filter.cpp
    src/math/vector.cpp
    src/math/matrix.cpp
    src/math/quarternion.cpp

    src/util/log.cpp
    src/util/server.cpp
    src/util/str_utils.cpp
    src/util/timer.cpp
)

add_executable(adiru src/main/adiru-nosocket.cpp)
target_link_libraries(adiru autonomous)

add_executable(gps src/main/gps_test.cpp)
target_link_libraries(gps autonomous)

add_executable(magtest src/main/mag_test.cpp)
target_link_libraries(magtest autonomous)

# add_executable(motor_controller src/main/motor_controller.cpp)
# target_link_libraries(motor_controller autonomous)
# target_link_directories(motor_controller PUBLIC ./lib)
# target_link_libraries(motor_controller paho-mqtt3a)
# target_link_libraries(motor_controller paho-mqtt3as)
# target_link_libraries(motor_controller paho-mqttpp3)
# target_link_libraries(motor_controller crypto)

add_executable(monitor src/main/monitor.cpp)
target_link_libraries(monitor autonomous)

add_executable(drone src/main/drone.cpp)
target_link_libraries(drone autonomous)

add_executable(hello src/main/hello.cpp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -g")
# set(CMAKE_INSTALL_PREFIX ./build)

install(TARGETS adiru EXPORT bin)
install(TARGETS gps EXPORT bin)
install(TARGETS magtest EXPORT bin)
install(TARGETS monitor EXPORT bin)
install(TARGETS drone EXPORT bin)
install(TARGETS hello EXPORT bin)
install(TARGETS autonomous EXPORT lib)