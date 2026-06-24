#include <WiFi.h>
#include <WiFiManager.h>

const char* ssid = "spray";
const char* password = "12345678";

const int LED_PIN = 2;

void setup() {
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);

  WiFi.begin(ssid, password);

  Serial.print("Connecting to default WiFi");

  int count = 0;

  while (WiFi.status() != WL_CONNECTED && count < 20) {
    delay(500);
    Serial.print(".");
    count++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.println("Connected to spray");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  }
  else {
    Serial.println();
    Serial.println("Default WiFi Failed");

    WiFiManager wm;

    bool res = wm.autoConnect("ESP32_Setup");

    if (res) {
      Serial.println("Connected through WiFiManager");
      Serial.print("IP Address: ");
      Serial.println(WiFi.localIP());
    }
    else {
      Serial.println("Connection Failed");

      while (true) {
        digitalWrite(LED_PIN, HIGH);
        delay(200);
        digitalWrite(LED_PIN, LOW);
        delay(200);
      }
    }
  }
}

void loop() {
  digitalWrite(LED_PIN, HIGH);
  Serial.println("LED ON");
  delay(1000);

  digitalWrite(LED_PIN, LOW);
  Serial.println("LED OFF");
  delay(1000);
}
