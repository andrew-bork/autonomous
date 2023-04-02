
# Autonomous

A complete rewrite of the drone repository. It now uses a cross-compiler to compile for the Raspberry Pi.


## Usage

Requires the install of an [MQTT broker](https://randomnerdtutorials.com/how-to-install-mosquitto-broker-on-raspberry-pi/) on the Raspberry Pi.

Clone the project

```bash
  git clone https://link-to-project
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

