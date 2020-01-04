# ESP32-Activity-Logger
 ESP32-based logger for activities including GPS and BLE-devices

## Credits
The project uses Neil Kolban's libraries for Bluetooth connections ([here](https://github.com/nkolban/ESP32_BLE_Arduino)). It also uses TinyGPS++ from Mikal Hart ([here](http://arduiniana.org/libraries/tinygpsplus/)) for controlling and interpreting the GY-GPS6MV2 module.

## Capabilities
- [x] Receive GPS location from GY-GPS6MV2 module
- [x] Connecting with GATT Heart Rate devices

## Todo list
- [ ] Make a central structure for the most recent data
- [ ] Make a variable indicating GPS connection is established
- [ ] Add more callback methods for the Bluetooth connection
- [ ] Add Bluetooth support for cadence sensors
- [ ] Make every Bluetooth connection a separate client to support multiple sensors
- [ ] Save data to flash or, or maybe SD?
- [ ] Add WiFi capability to automatically connect and upload data
- [ ] Increase GPS capabilities (time, speed, etc)
- [ ] Increase Bluetooth capabilities (battery, etc)
- [ ] In circuit, add an on/off switch, battery, RTC