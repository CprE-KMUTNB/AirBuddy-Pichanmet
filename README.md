# Air Buddy
## What is it?
Air Buddy is a solution for turning air conditioner automatically or manually from anywhere. In automatic mode, it will be turn on by particular temperature or a defined schedule.
## Hardware Requirement
- WEMOS LOLIN32 LITE
- IR Receiver module
- IR Sensor module
- Raspberry PI 4
- Zigbee Dongle (EFR32MG21)
- Xiaomi Zigbee Smart Temperature sensor
## Software
- Zigbee2MQTT on Raspberry Pi 64-bit OS
- NodeJS v20.10.0 LTS
- Nginx v1.22.1
- Arduino IDE v2.2.1
  - ArduinoJSON v6.21.3
  - IRremoteESP8266 v2.8.6
- Certbot v2.1.0
- Node-RED v3.1.3
## Installation
### Arduino
Change configuration variable and simply upload it.
### Node-RED
You must have Node-RED installed on your system and accessable on the internet. Then import the Node-RED file to the system and run Node-RED dashboard.
### Websocket
Change domain name, certificate and private key variable and run by issuing `npm start`
### MQTT
MQTT is used together with zigbee2mqtt, refer to mosquitto for installation guide. After that, config it to listen on secure channel by setting certificate and private key.
### Raspberry Pi
Raspberry Pi is used to connect with temperature sensor. You must install zigbee2mqtt on it, refer you zigbee2mqtt for installation guide.
## Example Screenshot for Node-RED dashboard
![image](https://github.com/buratud/air-buddy/assets/62024314/67f16975-eecf-4dd0-9e5c-4ead10b2e89b)
## Architecture Structure
![image](https://github.com/buratud/air-buddy/assets/62024314/678b5cf3-ec78-497e-b6c3-af409d316f6e)


