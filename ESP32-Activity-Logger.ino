#include <TinyGPS++.h>
#include <Ticker.h>
#include "Bluetooth.h"

TinyGPSPlus gps;
Bluetooth* btConnection;
bool btUpdate = false;

struct {
  // Sensor data
  uint8_t heartrate;
  uint8_t cadence;
  // GPS location info
  bool fix = false;
  double lat;
  double lng;
  // GPS time info
  uint32_t time;
  uint32_t time_age;
} currentData;

static void notifyCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify) {
  Serial.println(pData[1]);
}

static void messageCallback(const char* message) {
  Serial.println(message);
}

// For the Bluetooth connection, which only needs to be
// called every few seconds, give or take, to keep it alive.
Ticker doBT;
void handleBluetooth() {
  btUpdate = true;
}

void setup() {
  // Use Serial1 for debug logging
  Serial.begin(115200);
  // Now use Serial2 for GPS.
  // Initialize before BT, because it generally takes longer to setup
  Serial2.begin(9600);
  
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

  // GPS
  while(Serial2.available() > 0) {
    gps.encode(Serial2.read());
  }

  if(gps.location.isUpdated()) {
    Serial.print("Fix time = ");
    Serial.print(gps.date.value());
    Serial.print(gps.time.value());
    Serial.print(",Latitude = ");
    Serial.print(gps.location.lat(),8);
    Serial.print(", Longitude = ");
    Serial.println(gps.location.lng(),8);
  }
  
  delay(1000);
}
