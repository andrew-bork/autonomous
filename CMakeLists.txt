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
    src/backend/net.cpp
    src/backend/udp.cpp
    
    src/subsystem/adiru.cpp
    src/subsystem/led_driver.cpp
    src/subsystem/motor_driver.cpp


    src/math/filter.cpp
    src/math/vector.cpp
    src/math/matrix.cpp
    src/math/quarternion.cpp

    src/util/log.cpp
    src/util/server.cpp
    src/util/str_utils.cpp
    src/util/timer.cpp

    lib-Mission-Control/src/missioncontrol.cpp
)

add_executable(gps src/main/gps_test.cpp)
target_link_libraries(gps autonomous)

add_executable(magtest src/main/mag_test.cpp)
target_link_libraries(magtest autonomous)

add_executable(arm src/main/arm.cpp)
target_link_libraries(arm autonomous)

add_executable(discovery src/main/discovery.cpp src/backend/udp.cpp)


add_executable(drone src/main/drone.cpp)
target_link_libraries(drone autonomous)

add_executable(hello src/main/hello.cpp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -g")
# set(CMAKE_INSTALL_PREFIX ./build)

install(TARGETS gps EXPORT bin)
install(TARGETS magtest EXPORT bin)
install(TARGETS drone EXPORT bin)
install(TARGETS arm EXPORT bin)
install(TARGETS discovery EXPORT bin)
install(TARGETS hello EXPORT bin)
install(TARGETS autonomous EXPORT lib)