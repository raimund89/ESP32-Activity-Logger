#include "Bluetooth.h"

// Using Neil Kolban's Bluetooth libraries. Can save a lot of space on the device by not doing this...
// Bluetooth specs for GATT at https://www.bluetooth.com/specifications/gatt/characteristics/

static void notifyCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify) {
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
  Serial.println(pData[1]);
}

static void messageCallback(const char* message) {
  Serial.println(message);
}

Bluetooth btConnection(messageCallback, notifyCallback);

void setup() {
  Serial.begin(115200);
  btConnection.init();
}

void loop() {
  btConnection.handle();
  delay(1000);
}
