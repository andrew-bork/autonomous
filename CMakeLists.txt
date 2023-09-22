cmake_minimum_required(VERSION 3.27.0)
set(CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD 23)


project(autonomous)

add_executable(adiru src/adiru.cpp src/math/filter.cpp src/math/math.cpp src/backend/mpu6050.cpp src/backend/i2c.cpp src/backend/bmp390.cpp src/backend/qmc5883.cpp  src/backend/socket.cpp lib/libpaho-mqtt3a.so lib/libpaho-mqttpp3.so)
add_executable(gps_test src/gps_test.cpp src/math/filter.cpp src/math/math.cpp src/backend/mpu6050.cpp src/backend/i2c.cpp src/backend/bmp390.cpp src/backend/qmc5883.cpp  src/backend/socket.cpp)
add_executable(mag_test src/mag_test.cpp src/math/filter.cpp src/math/math.cpp src/backend/mpu6050.cpp src/backend/i2c.cpp src/backend/bmp390.cpp src/backend/qmc5883.cpp  src/backend/socket.cpp)
# add_executable(motor_controller src/motor_controller.cpp)
link_directories(./lib)

include_directories(include)
# add_library(./lib/)

# Turn on all warning.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")