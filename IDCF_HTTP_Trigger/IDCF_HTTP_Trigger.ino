/*
 * References
 *
 * mqtt_auth by Ian Tester (originally by Nicholas O'Leary)
 * https://github.com/Imroy/pubsubclient/blob/master/examples/mqtt_auth/mqtt_auth.ino
 *
 * bme280_test by Embedded Adventures
 * https://github.com/embeddedadventures/BME280/blob/master/examples/bme280_test/bme280_test.ino
 */
#include <Wire.h>

#include <ESP8266WiFi.h>
#include <BME280_MOD-1022.h>

#include "config.h"

float temperature = 0.0;
float humidity = 0.0;
float pressure = 0.0;

void printFormattedFloat(float val) {
  char buffer[10];

  dtostrf(val, 4, 2, buffer);
  Serial.print(buffer);
}

void setup() {
  // I2Cの通信を開始
  // SDA: DIO4
  // SCL: DIO14
  Wire.begin(4, 14);

  // シリアル通信を開始
  Serial.begin(115200);

  // BME280の補償値を読み取る
  BME280.readCompensationParams();

  // オーバーサンプリングの回数を設定
  BME280.writeOversamplingTemperature(os1x);
  BME280.writeOversamplingHumidity(os1x);
  BME280.writeOversamplingPressure(os1x);
}

void loop() {
  // BME280を1度だけ測定を行うモードに設定し計測が終わるまで待機
  BME280.writeMode(smForced);
  while (BME280.isMeasuring()) {
    delay(1);
  }

  // BME280から測定値を読み取る
  BME280.readMeasurements();
  temperature = BME280.getTemperature();
  humidity = BME280.getHumidity();
  pressure = BME280.getPressure();

  // 読み取った温度をシリアルにプリント
  Serial.print("Temperature: ");
  printFormattedFloat(temperature);
  Serial.println("ºC");

  // もし現在の温度が閾値よりも高ければ以下を実行
  if (temperature > threshold) {
    if (WiFi.status() != WL_CONNECTED) {
      Serial.print("Connecting to ");
      Serial.print(ssid);
      Serial.println("...");
      WiFi.begin(ssid, password);

      if (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.println("Failed");
        return;
      } else {
        Serial.println("WiFi connected");
        Serial.println("IP address: ");
        Serial.println(WiFi.localIP());
      }
    }

    Serial.print("Connecting to ");
    Serial.println(host);
    WiFiClient client;
    if (!client.connect(host, 80)) {
      Serial.println("Connection failed");
      return;
    }

    // IDCFチャンネルサーバの/data/{trigger-1のuuid}にHTTP POST
    String url = "/data/";
    url += trigger_1_uuid;
    client.print(String("POST ") + url + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "meshblu_auth_uuid: " + trigger_1_uuid + "\r\n" +
                 "meshblu_auth_token: " + trigger_1_token + "\r\n" +
                 "Connection: close\r\n\r\n");
    delay(100);

    while (client.available()) {
      String line = client.readStringUntil('\r');
      Serial.print(line);
    }
  }

  delay(5000);
}

