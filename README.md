
# Autonomous

A complete rewrite of my [drone repository](https://github.com/andrew-bork/drone). It now uses a cross-compiler to compile for the Raspberry Pi and my custom [libmissioncontrol](https://github.com/andrew-bork/lib-Mission-Control).

This is a C++ project to use a Raspberry Pi as an onboard flight controller in a drone. 

This code is probably not portable without some extra coercing.

#### Sensors and Devices:

- [PCA9685](https://www.adafruit.com/product/815)
- [BMP390](https://www.bosch-sensortec.com/products/environmental-sensors/pressure-sensors/bmp390/)
- [MPU6050](https://www.sparkfun.com/products/11028)
- 
- Simonk 35A ESC
- EMAX 2213/935KV BLDC

#### Dependencies:
- Raspberry Pi i2c
- [libmissioncontrol](https://github.com/andrew-bork/lib-Mission-Control)
## Usage

Clone the project

```bash
  git clone https://github.com/andrew-bork/autonomous
```

Go to the project directory

```bash
  cd autonomous
```

Build with Cmake

```bash
  cmake -S . -B ./build
  cd build
  make -j12
```

