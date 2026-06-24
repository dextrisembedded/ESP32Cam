#include <WiFi.h>
#include <WebServer.h>

const char* ap_ssid = "ESP32_Config";
const char* ap_password = "12345678";

WebServer server(80);

String htmlPage =
"<html><body>"
"<h2>ESP32 WiFi Setup</h2>"
"<form action='/save'>"
"SSID:<br><input type='text' name='ssid'><br><br>"
"Password:<br><input type='password' name='pass'><br><br>"
"<input type='submit' value='Save'>"
"</form>"
"</body></html>";

void handleRoot() {
  server.send(200, "text/html", htmlPage);
}

void handleSave() {
  String ssid = server.arg("ssid");
  String pass = server.arg("pass");

  Serial.println("Received Credentials:");
  Serial.print("SSID: ");
  Serial.println(ssid);
  Serial.print("Password: ");
  Serial.println(pass);

  server.send(200, "text/html",
              "<h2>Credentials Received</h2>");
}

void setup() {
  Serial.begin(115200);

  WiFi.softAP(ap_ssid, ap_password);

  Serial.println("Access Point Started");
  Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);
  server.on("/save", handleSave);

  server.begin();
}

void loop() {
  server.handleClient();
}
