// Reference
// https://beebotte.com/docs/mqtt

#include <ESP8266WiFi.h>

// PubSubClientライブラリでのパケットサイズは128バイトなのを拡張
#define MQTT_MAX_PACKET_SIZE 1024
#include <PubSubClient.h>

#include <ArduinoJson.h>

#include "config.h"

const char* host = "mqtt.beebotte.com";

// メッセージを受け取ったらシリアルにプリント
void callback(char* topic, byte* payload, unsigned int length) {
  // PubSubClient.hで定義されているMQTTの最大パケットサイズ
  char buffer[MQTT_MAX_PACKET_SIZE];

  snprintf(buffer, sizeof(buffer), "%s", payload);
  Serial.println("received:");
  Serial.print("topic: ");
  Serial.println(topic);
  Serial.println(buffer);

  // 受け取ったJSON形式のペイロードをデコードする
  StaticJsonBuffer<MQTT_MAX_PACKET_SIZE> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(buffer);

  if (!root.success()) {
    Serial.println("parseObject() failed");
    return;
  }

  const char* parsedPayload = root["data"];
  if (parsedPayload != NULL) {
    Serial.print("payload: ");
    Serial.println(parsedPayload);
  }
}

WiFiClient wifiClient;
PubSubClient client(host, 1883, wifiClient);

void setup() {
  Serial.begin(115200);
  Serial.println();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("connecting to ");
    Serial.print(ssid);
    Serial.println("...");
    WiFi.begin(ssid, password);

    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
      // Wi-Fiアクスポイントへの接続に失敗したら5秒間待ってリトライ
      Serial.println("failed to connect");
      delay(5000);
      return;
    } else {
      Serial.print("WiFi connected: ");
      Serial.println(WiFi.localIP());
    }
  }

  // クライアントがサーバに接続されていなければ
  if (!client.connected()) {
    // ユーザ名を指定して接続
    String username = "token:";
    username += channelToken;
    client.connect(clientID, username.c_str(), NULL);

    if (client.connected()) {
      Serial.println("MQTT connected");
      client.setCallback(callback);

      // トピック名を指定してsubscribe
      client.subscribe(topic);
    } else {
      Serial.print("MQTT connection failed: ");
      switch (client.state()) {
        case -4:
          Serial.println("MQTT_CONNECTION_TIMEOUT - the server didn't respond within the keepalive time");
          break;
        case -3 :
          Serial.println("MQTT_CONNECTION_LOST - the network connection was broken");
          break;
        case -2 :
          Serial.println("MQTT_CONNECT_FAILED - the network connection failed");
          break;
        case -1 :
          Serial.println("MQTT_DISCONNECTED - the client is disconnected cleanly");
          break;
        case 0 :
          Serial.println("MQTT_CONNECTED - the cient is connected");
          break;
        case 1 :
          Serial.println("MQTT_CONNECT_BAD_PROTOCOL - the server doesn't support the requested version of MQTT");
          break;
        case 2 :
          Serial.println("MQTT_CONNECT_BAD_CLIENT_ID - the server rejected the client identifier");
          break;
        case 3 :
          Serial.println("MQTT_CONNECT_UNAVAILABLE - the server was unable to accept the connection");
          break;
        case 4 :
          Serial.println("MQTT_CONNECT_BAD_CREDENTIALS - the username / password were rejected");
          break;
        case 5 :
          Serial.println("MQTT_CONNECT_UNAUTHORIZED - the client was not authorized to connect");
          break;
        default:
          break;
      }

      Serial.println(client.state());
      delay(5000);
    }
  } else {
    // 既にサーバに接続されていれば通常処理を行う
    client.loop();
  }
}

