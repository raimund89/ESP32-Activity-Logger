#ifndef _BLUETOOTH_H_
#define _BLUETOOTH_H_

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

// **** GATT service UUIDs ****
// 0x180D Heart Rate
// 0x1814 Cycling Speed and Cadence
static BLEUUID hrserviceUUID((uint16_t)0x180D);
static BLEUUID scserviceUUID((uint16_t)0x180D);
// **** GATT characteristic UUIDs ****
// 0x2A37 Heart Rate Measurement
// 0x2A38 Body Location
// 0x2A5B Cycling Speed and Cadence
// 0x2A5C Cycling Speed and Cadence Feature
static BLEUUID        hrUUID((uint16_t)0x2A37);
static BLEUUID        blUUID((uint16_t)0x2A38);
static BLEUUID        scUUID((uint16_t)0x2A5B);
static BLEUUID       scfUUID((uint16_t)0x2A5C);

typedef void (*message_callback)(const char* message);

class Bluetooth {
  private:
    // The scan device
    BLEScan* pBLEScan;
    // Result of scanning
    BLEAdvertisedDevice* myDevice;
    
    // Control flow booleans
    bool doConnect;
    bool connected;
    bool doScan;

    // Remote characteristics
    BLERemoteCharacteristic* pRemoteCharacteristic;
    
    notify_callback notifyCallback;
    message_callback messageCallback;
    
    bool connectToServer();

    friend class MyAdvertisedDeviceCallbacks;
    friend class MyClientCallback;

  public:
    Bluetooth(message_callback _message, notify_callback _notify);
    
    void init();
    
    void handle();
};

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
  private:
    Bluetooth* bluetooth;

  public:
    MyAdvertisedDeviceCallbacks(Bluetooth* bt);
    void onResult(BLEAdvertisedDevice advertisedDevice);
};

class MyClientCallback: public BLEClientCallbacks {
  private:
    Bluetooth* bluetooth;

  public:
    MyClientCallback(Bluetooth* bt);
    
    void onConnect(BLEClient* pclient);
    void onDisconnect(BLEClient* pclient);
};

#endif
