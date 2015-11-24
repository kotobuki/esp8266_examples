#include <Wire.h>

#include <ESP8266WiFi.h>

// HDC1000を扱うためのライブラリ
#include <Adafruit_HDC1000.h>

// 設定ファイル
#include "config.h"

WiFiClient wifiClient;

Adafruit_HDC1000 hdc1000 = Adafruit_HDC1000();

float temperature = 0.0;
float humidity = 0.0;

void printFormattedFloat(float val) {
  char buffer[10];

  dtostrf(val, 4, 2, buffer);
  Serial.print(buffer);
}

void setup() {
  // I2Cの通信を開始
  // SDA: DIO4
  // SCL: DIO5
  Wire.begin();

  // シリアル通信を開始
  Serial.begin(115200);
  delay(10);
  Serial.println();
  Serial.println();

  // HDC1000を設定
  hdc1000.begin();
}

void loop() {
  // Wi-Fiアクセスポイントに接続していなければ接続
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Connecting to ");
    Serial.print(ssid);
    Serial.println("...");
    WiFi.begin(ssid, password);

    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
      // Wi-Fiアクスポイントへの接続に失敗したら5秒間待ってリトライ
      Serial.println("Failed to connect");
      delay(5000);
      return;
    } else {
      Serial.print("WiFi connected: ");
      Serial.println(WiFi.localIP());
    }
  }

  // HDC1000から測定値を読み取る
  temperature = hdc1000.readTemperature();
  humidity = hdc1000.readHumidity();

  // 読み取った温度をシリアルにプリント
  Serial.print("Temperature: ");
  printFormattedFloat(temperature);
  Serial.println("ºC");

  // もし現在の温度が閾値よりも高ければ以下を実行
  if (temperature > threshold) {
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

