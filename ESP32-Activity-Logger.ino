#include <Ticker.h>
#include "Bluetooth.h"

// Using Neil Kolban's Bluetooth libraries. Can save a lot of space on the device by not doing this...
// Bluetooth specs for GATT at https://www.bluetooth.com/specifications/gatt/characteristics/

// TODO: Add more callback methods
// TODO: Add support for heart rate AND cadence
// TODO: Somehow be able to spawn multiple BT connections

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

Bluetooth* btConnection;
bool btUpdate = false;

// For the Bluetooth connection, which only needs to be
// called every few seconds, give or take, to keep it alive.
Ticker doBT;
void handleBluetooth() {
  btUpdate = true;
}

void setup() {
  // Use Serial1 for debug logging
  Serial.begin(115200);
  btConnection = new Bluetooth(messageCallback, notifyCallback);
  btConnection->init();

  // Fire the ticker every 2 seconds
  doBT.attach(2.0, handleBluetooth);
}

void loop() {
  // Bluetooth
  if(btUpdate) {
    btUpdate = false;
    btConnection->handle();
  }
  delay(1000);
}
