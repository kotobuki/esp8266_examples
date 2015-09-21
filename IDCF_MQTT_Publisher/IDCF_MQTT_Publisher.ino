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

// MQTTを扱うためのライブラリ
// https://github.com/Imroy/pubsubclient
#include <PubSubClient.h>

// BME280を扱うためのライブラリ
// https://github.com/embeddedadventures/BME280
#include <BME280_MOD-1022.h>

#include "config.h"

WiFiClient wclient;

// MQTTクライアント
PubSubClient client(wclient, server);

// 温度と湿度、気圧を格納する変数
float temperature = 0.0;
float humidity = 0.0;
float pressure = 0.0;

// コールバック関数
void callback(const MQTT::Publish& pub) {
  // コールバックで必要な処理があればここで
}

// JSONフォーマットでペイロードを用意
String getPayloadInJson() {
  String json = "{";
  json +=  "\"devices\":";
  json += "[\"";
  json += action_1_uuid;
  json += "\"";
  json += ",";
  json += "\"";
  json += action_2_uuid;
  json += "\"";
  json += "],";
  json += "\"payload\":";
  json += "{";
  json += "\"pressure\":\"";
  json += pressure;
  json += "\",";
  json += "\"humidity\":\"";
  json += humidity;
  json += "\",";
  json += "\"temperature\":\"";
  json += temperature;
  json += "\"}";
  json += "}";

  return json;
}

// 浮動小数点数を小数点以下2桁でシリアルにプリント
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
}

void loop() {
  // Wi-Fiアクセスポイントに接続していなければ接続
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Connecting to ");
    Serial.print(ssid);
    Serial.println("...");
    WiFi.begin(ssid, pass);

    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
      Serial.println("Failed");
      return;
    } else {
      Serial.println("WiFi connected");
    }
  }

  // クライアントがICDF Cloudのサーバに接続されていなければ接続
  if (!client.connected()) {
    // トリガー1のUUIDとトークンで認証
    MQTT::Connect mqttConnect(mqtt_client_id);
    mqttConnect.set_auth(trigger_1_uuid, trigger_1_token);

    if (client.connect(mqttConnect)) {
      client.set_callback(callback);
      Serial.println("Client connected");
    } else {
      Serial.println("Client failed to connect");
    }
  }

  // クライアントがICDF Cloudのサーバに接続されていたら以下の処理を実行
  if (client.connected()) {
    // BME280の計測が終わるのを待つ
    while (BME280.isMeasuring()) {
      delay(1);
    }

    // BME280から測定値を読み取る
    BME280.readMeasurements();

    // 温度と湿度、気圧を取得
    temperature = BME280.getTemperature();
    humidity = BME280.getHumidity();
    pressure = BME280.getPressure();

    // 温度と湿度、気圧をシリアルにプリント
    Serial.print("Temperature: ");
    printFormattedFloat(temperature);
    Serial.println("ºC");
    Serial.print("Humidity   : ");
    printFormattedFloat(humidity);
    Serial.println("%");
    Serial.print("Pressure   : ");
    printFormattedFloat(pressure);
    Serial.println("hPa");

    // IDCF CloudにMQTTでデータを送信
    bool sent = client.publish("message", getPayloadInJson());
    if (sent) {
      Serial.println("Sent");
    } else {
      Serial.println("Failed to send");
    }

    Serial.println();
  }

  delay(10000);
}

