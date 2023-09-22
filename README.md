
# Autonomous

A complete rewrite of my [drone repository](https://github.com/andrew-bork/drone). It now uses a cross-compiler to compile for the Raspberry Pi and an MQTT broker to handle messaging.

This is a C++ project to use a Raspberry Pi as an onboard flight controller in a drone. 

This code is not portable, as it directly interfaces with i2c devices.

#### Sensors and Devices:

- [PCA9685](https://www.adafruit.com/product/815)
- [BMP390](https://www.bosch-sensortec.com/products/environmental-sensors/pressure-sensors/bmp390/)
- [MPU6050](https://www.sparkfun.com/products/11028)
- 
- Simonk 35A ESC
- EMAX 2213/935KV BLDC

#### Dependencies:
- Raspberry Pi i2c
- MQTT broker
- [paho-mqtt](https://www.eclipse.org/paho/)
- [fmt](https://github.com/fmtlib/fmt)
## Usage

Requires the install of an [MQTT broker](https://randomnerdtutorials.com/how-to-install-mosquitto-broker-on-raspberry-pi/) on the Raspberry Pi.

Clone the project

```bash
  git clone https://github.com/andrew-bork/autonomous
```

Go to the project directory

```bash
  cd autonomous
```

Build project

```bash
  make
```

Copy files to Raspberry Pi

```bash
    scp -r bin/* raspberrypi@hostname:~/install
```

Start an SSH session, then run.

```bash
  ssh raspberrypi@hostname
  ./install/adiru && ./install/motor_controller 
```

