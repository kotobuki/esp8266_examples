#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

// Read the configurations from the config.h tab
#include "config.h"

// Constants
const char* host = "maker.ifttt.com";
const char* event   = "ping";
const int httpsPort = 443;
const int buttonPin = 4;

// Variables
WiFiClientSecure client;

int lastState = -1;

void sendWebRequest() {
  Serial.print("connecting to ");
  Serial.println(host);

  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }

  // Make a URL
  // maker.ifttt.com/trigger/{event}/with/key/{key}
  String url = "/trigger/";
  url += event;
  url += "/with/key/";
  url += key;

  Serial.print("requesting URL: ");
  Serial.println(url);

  // Send a web request
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");

  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      // Finished reading a header
      break;
    }
  }

  String line = client.readStringUntil('\n');
  Serial.print("reply: ");
  Serial.println(line);
  Serial.println("closing connection");
  Serial.println();
}

void setup() {
  // Initialize the button pin as an input
  pinMode(buttonPin, INPUT);

  // Initialize serial communication
  Serial.begin(115200);
  Serial.println();
  Serial.print("connecting to ");
  Serial.println(ssid);

  // Connect to the Wi-Fi access point
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  int state = lastState;
  int sensorReading = analogRead(A0);

  if (sensorReading > highThreshold) {
    state = HIGH;
  } else if (sensorReading < lowThreshold) {
    state = LOW;
  }

  if (lastState == LOW && state == HIGH) {
    // Do something on a rising edge, if needed
    // Send a web request to the IFTTT server
    Serial.println("_|");
    sendWebRequest();
  } else if (lastState == HIGH && state == LOW) {
    // Do something on a falling edge, if needed
    Serial.println("|_");
  }

  lastState = state;
}

