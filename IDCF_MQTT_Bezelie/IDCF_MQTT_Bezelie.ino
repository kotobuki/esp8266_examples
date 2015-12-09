#include <Servo.h>
#include <Ticker.h>

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#include "Bezelie.h"
#include "config.h"

WiFiClient wifiClient;
PubSubClient client(wifiClient);

Bezelie bezelie;
Ticker ticker;

// メッセージを受け取ったらシリアルにプリント
void callback(char* topic, byte* payload, unsigned int length) {
  // PubSubClient.hで定義されているMQTTの最大パケットサイズ
  char buffer[MQTT_MAX_PACKET_SIZE];

  snprintf(buffer, sizeof(buffer), "%s", payload);
  Serial.print("Received: ");
  Serial.println(buffer);

  // 受け取ったJSON形式のペイロードをデコードする
  StaticJsonBuffer<MQTT_MAX_PACKET_SIZE> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(buffer);

  if (!root.success()) {
    Serial.println("parseObject() failed");
    return;
  }

  String parsedPayload = String((const char*)root["data"]["payload"]);
  if (parsedPayload != NULL) {
    Serial.print("payload: ");
    Serial.println(parsedPayload);
    if (parsedPayload.equals("晴れ")) {
      // 天気が「晴れ」であればモーション0を実行
      bezelie.startMotion(0);
    } else {
      // それ以外であればモーション1を実行
      bezelie.startMotion(1);
    }
  }
}

void updateBezelie() {
  bezelie.update();
}

void setup() {
  Serial.begin(115200);
  delay(10);
  Serial.println();
  Serial.println();
}

void loop() {
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

  // クライアントがサーバに接続されていなければ
  if (!client.connected()) {
    // アクション1のUUIDとトークンをユーザ名およびパスワードとしてサーバに接続
    client.connect(mqtt_client_id, action_1_uuid, action_1_token);

    if (client.connected()) {
      Serial.print("MQTT connected: ");
      Serial.println(server);
      client.setCallback(callback);
      client.subscribe(action_1_uuid);

      // Pitch: 2
      // Roll: 12
      // Yaw: 13
      bezelie.begin(2, 12, 13);
      ticker.attach(0.1, updateBezelie);

      Serial.println("Ready to roll");
    } else {
      Serial.print("MQTT connection failed: ");
      Serial.println(client.state());
      delay(5000);
    }
  } else {
    // 既にサーバに接続されていれば通常処理を行う
    client.loop();
  }
}

