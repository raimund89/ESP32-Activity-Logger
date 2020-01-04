#include <string>
#include "Bluetooth.h"

Bluetooth::Bluetooth(message_callback _message, notify_callback _notify) {
  doConnect = false;
  connected = false;
  doScan = false;
  notifyCallback = _notify;
  messageCallback = _message;
}

MyClientCallback::MyClientCallback(Bluetooth* bt) {
  bluetooth = bt;
}

void MyClientCallback::onConnect(BLEClient* pclient) {} // Needs to be defined

void MyClientCallback::onDisconnect(BLEClient* pclient) {
  bluetooth->connected = false;
}
    
bool Bluetooth::connectToServer() {
  BLEClient* pClient = BLEDevice::createClient();
  messageCallback(" - Created client");

  pClient->setClientCallbacks(new MyClientCallback(this));

  // Now connect
  pClient->connect(myDevice);
  messageCallback(" - Connected to server");

  // Obtain reference to the service
  BLERemoteService* pRemoteService = pClient->getService(hrserviceUUID);
  if(pRemoteService == nullptr) {
    messageCallback(" - Failed to found the service");
    pClient->disconnect();
    return false;
  }
  messageCallback(" - Found the service");

  std::map<std::string, BLERemoteCharacteristic*>* characteristics = pRemoteService->getCharacteristics();
  std::map<std::string, BLERemoteCharacteristic*>::iterator it;

  for(it = characteristics->begin(); it != characteristics->end(); it++)
  {
    messageCallback(it->first.c_str());
  }

  pRemoteCharacteristic = pRemoteService->getCharacteristic(hrUUID);
  if(pRemoteCharacteristic == nullptr) {
    messageCallback(" - Failed to find the characteristic");
    pClient->disconnect();
    return false;
  }
  messageCallback(" - Found the characteristic");

  if(pRemoteCharacteristic->canRead()) {
    std::string value = pRemoteCharacteristic->readValue();
    messageCallback(value.c_str());
  }

  if(pRemoteCharacteristic->canNotify()) {
    pRemoteCharacteristic->registerForNotify(notifyCallback);
  }

  // Make sure the characteristic will send notifications. Only do this once, not neccessary to do it more
  const uint8_t notificationsOn[] = {0x1, 0x0}; // Notifications on, indications off
  pRemoteCharacteristic->getDescriptor(BLEUUID((uint16_t)0x2902))->writeValue((uint8_t*)notificationsOn, 2, true);

  connected = true;
}

MyAdvertisedDeviceCallbacks::MyAdvertisedDeviceCallbacks(Bluetooth* bt) {
  bluetooth = bt;
}
    
void MyAdvertisedDeviceCallbacks::onResult(BLEAdvertisedDevice advertisedDevice) {
  bluetooth->messageCallback(advertisedDevice.toString().c_str());
  
  if(advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(hrserviceUUID)){
    BLEDevice::getScan()->stop();

    bluetooth->myDevice = new BLEAdvertisedDevice(advertisedDevice);
    bluetooth->doConnect = true; // Next step is connecting
    bluetooth->doScan = true; // If we ever get disconnected, we are allowed to scan again
  }
}

void Bluetooth::init() {
  messageCallback(" - Beginning scanning");

  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks(this));
  pBLEScan->setActiveScan(true);
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);
  pBLEScan->start(5, false);
}
    
void Bluetooth::handle() {
  if(doConnect) {
    if(connectToServer()) {
      messageCallback(" - Full connection established");
    } else {
      messageCallback(" - Failed to connect");
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
}
