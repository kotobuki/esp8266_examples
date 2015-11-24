#include <Wire.h>

#include <ESP8266WiFi.h>

// MQTTを扱うためのライブラリ
#include <PubSubClient.h>

// JSONを扱うためのライブラリ
#include <ArduinoJson.h>

// HDC1000を扱うためのライブラリ
#include <Adafruit_HDC1000.h>

// 設定ファイル
#include "config.h"

WiFiClient wifiClient;
PubSubClient client(wifiClient);

Adafruit_HDC1000 hdc1000 = Adafruit_HDC1000();

// 温度と湿度を格納する変数
float temperature = 0.0;
float humidity = 0.0;

// コールバック関数
void callback(char* topic, byte* payload, unsigned int length) {
  // コールバックで必要な処理があればここで
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
      client.setServer(server, 1883);
    }
  }

  // クライアントがICDF Cloudのサーバに接続されていなければ
  if (!client.connected()) {
    // トリガー1のUUIDとトークンをユーザ名およびパスワードとしてサーバに接続
    client.connect(mqtt_client_id, trigger_1_uuid, trigger_1_token);

    if (client.connected()) {
      Serial.print("MQTT connected: ");
      Serial.println(server);
      client.setCallback(callback);
    } else {
      Serial.print("MQTT connection failed: ");
      Serial.println(client.state());
      delay(5000);
    }
  } else {
    // 既にサーバに接続されていれば通常処理を行う
    client.loop();

    // クライアントがICDF Cloudのサーバに接続されていたら以下の処理を実行
    // 温度と湿度を取得
    temperature = hdc1000.readTemperature();
    humidity = hdc1000.readHumidity();

    // JSONフォーマットでペイロードを用意
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    JsonArray& devices = root.createNestedArray("devices");
    devices.add(action_1_uuid);
//    devices.add(action_2_uuid);
    JsonObject& payload = root.createNestedObject("payload");
    payload["temperature"] = temperature;
    payload["humidity"] = humidity;
    root.prettyPrintTo(Serial);
    Serial.println();

    char buffer[256];
    root.printTo(buffer, sizeof(buffer));

    // IDCF CloudにMQTTでデータを送信
    bool sent = client.publish("message", buffer);
    if (sent) {
      Serial.println("Published");
    } else {
      Serial.println("Failed to publish");
    }

    Serial.println();
  }

  delay(10000);
}

