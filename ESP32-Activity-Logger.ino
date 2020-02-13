#include <TinyGPS++.h>
#include <Ticker.h>
#include <SPI.h>
#include <SPIMemory.h>
#include "Bluetooth.h"

// TODO: Add checks to see if things are working

TinyGPSPlus gps;
Bluetooth* btConnection;
bool btUpdate = false;

/*
 * First Int contains last written address
 * After that, flash contains just time-identified structs
 */

SPIFlash flash(5);

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
  //Serial.println(pData[1]);
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

uint32_t nextLocation;

// For saving our data. We want to save every 2 seconds for now
Ticker doWrite;
void handleWrite() {
  Serial.println("Writing data");
  flash.eraseSection(nextLocation, sizeof(currentData));
  flash.writeAnything(nextLocation, currentData);
  nextLocation += sizeof(currentData);
  Serial.printf("Next location is %d\n", nextLocation);
  flash.eraseSection(0, 4);
  flash.writeULong(0, nextLocation);
  Serial.printf("Reading next location gives %u\n", flash.readULong(0));
}

void clearFlash() {
  flash.eraseChip();
  flash.writeULong(0, 4); // Write to first byte
  nextLocation = 4;
}

void setup() {
  // Use Serial1 for debug logging
  Serial.begin(115200);

  // Now initialize the SPI flash
  flash.begin();
  nextLocation = flash.readULong(0);
  Serial.printf("Location of newest pointer: %u\n", nextLocation);
  
  // Now use Serial2 for GPS.
  // Initialize before BT, because it generally takes longer to setup
  Serial2.begin(9600);
  
  btConnection = new Bluetooth(messageCallback, notifyCallback);
  btConnection->init();

  // Fire the ticker every 2 seconds
  doBT.attach(2.0, handleBluetooth);

  // Now we can start writing the activity
  doWrite.attach(2.0, handleWrite);
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
    Serial.println(" - Received location");
    currentData.fix = true;
    currentData.lat = gps.location.lat();
    currentData.lng = gps.location.lng();
    currentData.time = gps.time.value();
    currentData.time_age = gps.time.age();
  } else {
    currentData.time_age = gps.location.age();

    // If we don't have our location updated for
    // 10 seconds, apparently we have lost our fix
    if(currentData.time_age > 10000) {
      currentData.fix = false;
    }
  }
  delay(10); // Give the OS some time to do things
}
