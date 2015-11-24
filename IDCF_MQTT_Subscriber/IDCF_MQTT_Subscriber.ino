#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#include "config.h"

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

  const char* parsedPayload = root["data"]["payload"];
  if (parsedPayload != NULL) {
    Serial.print("payload: ");
    Serial.println(parsedPayload);
  }
}

WiFiClient wifiClient;
PubSubClient client(wifiClient);

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

