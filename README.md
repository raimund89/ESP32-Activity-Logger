# ESP32-Activity-Logger
 ESP32-based logger for activities including GPS and BLE-devices

## Credits
The project uses Neil Kolban's libraries for Bluetooth connections ([here](https://github.com/nkolban/ESP32_BLE_Arduino)). It also uses TinyGPS++ from Mikal Hart ([here](https://github.com/mikalhart/TinyGPSPlus)) for controlling and interpreting the GY-GPS6MV2 module.

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

## Some background
Just to record the basics of GATT for the software. Don't mind this section, they are kind of random notes...

// **** For Heart Rate 0x2A37 ****
// pData[0] contains the flags of the transmission [format, status(2), energy expenditure, RR-interval length]
// pData[1] contains the heart rate in 8-bit or 16-bit depending on the flags
// pData[2:] contain energy expenditure (1 value) and RR values (sets of 2), depending on the flags
// **** For Body Location 0x2A38 ****
// pData[0] sensor location, choice of Other, Chest, Wrist, Finger, Hand, Ear Lobe, Foot
// **** For Cadence 0x2A5B ****
// pData[0] contains the flags of the transmission [wheel revolution, crank revolution]
// pdata[1] cumulative wheel revolutions
// pData[2] last wheel event time
// pData[3] cumulative crank revolutions
// pData[4] last crank event time
// **** For Cadence Feature 0x2A5C ****
// pData[0] contains the feature fields [wheel revolution, crank revolution, multiple sensor locations]