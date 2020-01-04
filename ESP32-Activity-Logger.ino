#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

// Using Neil Kolban's Bluetooth libraries. Can save a lot of space on the device by not doing this...
// Bluetooth specs for GATT at https://www.bluetooth.com/specifications/gatt/characteristics/

// **** GATT service UUIDs ****
// 0x180D Heart Rate
// 0x1814 Running Speed and Cadence
static BLEUUID serviceUUID((uint16_t)0x180D);
// **** GATT characteristic UUIDs ****
// 0x2A37 Heart Rate Measurement
// 0x2A38 Body Location
// 0x2A5B Running Speed and Cadence
// 0x2A5C Running Speed and Cadence Feature
static BLEUUID      hrUUID((uint16_t)0x2A37);

// The scan device
BLEScan* pBLEScan;
// Result of scanning
static BLEAdvertisedDevice* myDevice;

// Control flow booleans
static boolean doConnect = false;
static boolean connected = false;
static boolean doScan = false;

// Remote characteristics
static BLERemoteCharacteristic* pRemoteCharacteristic;

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
  Serial.println(String("Heart rate: ") + pData[1]);
}

class MyClientCallback : public BLEClientCallbacks {
  void onConnect(BLEClient* pclient) {} // Needs to be defined

  void onDisconnect(BLEClient* pclient) {
    connected = false;
  }
};

bool connectToServer() {
  BLEClient* pClient = BLEDevice::createClient();
  Serial.println(" - Created client");

  pClient->setClientCallbacks(new MyClientCallback());

  // Now connect
  pClient->connect(myDevice);
  Serial.println(" - Connected to server");

  // Obtain reference to the service
  BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
  if(pRemoteService == nullptr) {
    Serial.print(" - Failed to found the service");
    pClient->disconnect();
    return false;
  }
  Serial.println(" - Found the service");

  std::map<std::string, BLERemoteCharacteristic*>* characteristics = pRemoteService->getCharacteristics();
  std::map<std::string, BLERemoteCharacteristic*>::iterator it;

  for(it = characteristics->begin(); it != characteristics->end(); it++)
  {
    Serial.println(String("   - Characteristic found: ") + it->first.c_str());
  }

  pRemoteCharacteristic = pRemoteService->getCharacteristic(hrUUID);
  if(pRemoteCharacteristic == nullptr) {
    Serial.print(" - Failed to find the characteristic");
    pClient->disconnect();
    return false;
  }
  Serial.println(" - Found the characteristic");

  if(pRemoteCharacteristic->canRead()) {
    std::string value = pRemoteCharacteristic->readValue();
    Serial.print(" - The characteristic value was: ");
    Serial.println(value.c_str());
  }

  if(pRemoteCharacteristic->canNotify()) {
    pRemoteCharacteristic->registerForNotify(notifyCallback);
  }

  // Make sure the characteristic will send notifications. Only do this once, not neccessary to do it more
  const uint8_t notificationsOn[] = {0x1, 0x0}; // Notifications on, indications off
  pRemoteCharacteristic->getDescriptor(BLEUUID((uint16_t)0x2902))->writeValue((uint8_t*)notificationsOn, 2, true);

  connected = true;
}

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    Serial.printf("   - Advertised Device: %s \n", advertisedDevice.toString().c_str());
    if(advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID)){
      BLEDevice::getScan()->stop();

      myDevice = new BLEAdvertisedDevice(advertisedDevice);
      doConnect = true; // Next step is connecting
      doScan = true; // If we ever get disconnected, we are allowed to scan again
    }
  }
};

void setup() {
  Serial.begin(115200);
  Serial.println(" - Beginning scanning");

  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true);
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);
  pBLEScan->start(5, false);
}

void loop() {

  if(doConnect) {
    if(connectToServer()) {
      Serial.println(" - Full connection established");
    } else {
      Serial.println(" - Failed to connect");
    }
    doConnect = false; // Don't end up in this loop again unless we have disconnected and scanned again
  }

  if(connected) {
    // Not necessary, only when writing to a characteristic...
    //String newValue = "Time since boot: " + String(millis()/1000);
    //Serial.println("Setting new characteristic value to \"" + newValue + "\"");

    // Characteristic value
    //pRemoteCharac
  } else if(doScan) {
    BLEDevice::getScan()->start(0); // In case of disconnect
  }
  
  delay(1000);
}
