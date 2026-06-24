#include <WiFi.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>

#define SERVICE_UUID        "12345678-1234-1234-1234-123456789abc"
#define CHARACTERISTIC_UUID "87654321-4321-4321-4321-cba987654321"

class MyCallbacks : public BLECharacteristicCallbacks {

  void onWrite(BLECharacteristic *pCharacteristic) {

    String data = pCharacteristic->getValue().c_str();

    if (data.length() > 0) {

      Serial.print("Received: ");
      Serial.println(data);

      int pos = data.indexOf(',');

      if (pos > 0) {

        String ssid = data.substring(0, pos);
        String pass = data.substring(pos + 1);

        Serial.print("SSID: ");
        Serial.println(ssid);

        Serial.print("PASS: ");
        Serial.println(pass);

        Serial.println("Connecting WiFi...");

        WiFi.begin(ssid.c_str(), pass.c_str());

        int count = 0;

        while (WiFi.status() != WL_CONNECTED && count < 20) {
          delay(500);
          Serial.print(".");
          count++;
        }

        Serial.println();

        if (WiFi.status() == WL_CONNECTED) {

          Serial.println("WiFi Connected");
          Serial.print("IP Address: ");
          Serial.println(WiFi.localIP());

        } else {

          Serial.println("WiFi Connection Failed");
        }
      }
    }
  }
};

void setup() {

  Serial.begin(115200);

  BLEDevice::init("ESP32_BLE_SETUP");

  BLEServer *pServer = BLEDevice::createServer();

  BLEService *pService =
    pServer->createService(SERVICE_UUID);

  BLECharacteristic *pCharacteristic =
    pService->createCharacteristic(
      CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_READ |
      BLECharacteristic::PROPERTY_WRITE
    );

  pCharacteristic->setCallbacks(new MyCallbacks());

  pService->start();

  BLEAdvertising *pAdvertising =
    BLEDevice::getAdvertising();

  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->start();

  Serial.println("BLE Ready");
}

void loop() {
}
