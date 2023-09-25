cmake_minimum_required (VERSION 3.27.6)
project(Autonomous)

include_directories(include include/mqtt)

set(CXX_STANDARD C++23)

add_library(autonomous STATIC 
    src/backend/bmp390.cpp
    src/backend/gps.cpp
    src/backend/i2c.cpp
    src/backend/mpu6050.cpp
    src/backend/pca9685.cpp
    src/backend/qmc5883.cpp

    src/math/filter.cpp
    src/math/math.cpp

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

add_executable(motor_controller src/main/motor_controller.cpp)
target_link_libraries(motor_controller autonomous)

add_executable(monitor src/main/monitor.cpp)
target_link_libraries(monitor autonomous)

