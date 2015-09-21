/*
 * References
 * 
 * mqtt_subscriber by Ian Tester (originally by Nicholas O'Leary)
 * https://github.com/Imroy/pubsubclient/tree/master/examples/mqtt_subscriber
 */

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include "config.h"

#define BUFFER_SIZE 100

// メッセージを受け取ったらシリアルにプリント
void callback(const MQTT::Publish& pub) {
  Serial.print(pub.topic());
  Serial.print(" => ");
  if (pub.has_stream()) {
    // ペイロードのサイズが大きい場合にはローカルに用意したバッファに分割して読み取り
    // 読み取った単位ごとにシリアルにプリント
    uint8_t buf[BUFFER_SIZE];
    int read;
    while (read = pub.payload_stream()->read(buf, BUFFER_SIZE)) {
      Serial.write(buf, read);
    }
    pub.payload_stream()->stop();
    Serial.println();
  } else {
    // ペイロードのサイズが小さい場合にはそのままシリアルにプリント
    Serial.println(pub.payload_string());
  }
}

WiFiClient wclient;
PubSubClient client(wclient, server);

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
    WiFi.begin(ssid, pass);

    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
      return;
    }
    Serial.println("WiFi connected");
  }

  if (!client.connected()) {
    // アクション1のUUIDとトークンをユーザ名およびパスワードとしてサーバに接続
    MQTT::Connect mqttConnect(mqtt_client_id);
    mqttConnect.set_auth(action_1_uuid, action_1_token);

    if (client.connect(mqttConnect)) {
      client.set_callback(callback);
      client.subscribe(action_1_uuid);
    }
  }

  if (client.connected()) {
    client.loop();
  }
}

