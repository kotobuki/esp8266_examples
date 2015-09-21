// MQTTのクライアントID（ユニークにするためにスケッチをコンパイルした日付と時刻を使用）
const char *mqtt_client_id = "kotobuki" __DATE__ __TIME__;

// Wi-FiアクセスポイントのSSIDとパスワード
const char *ssid =  "********";
const char *pass =  "********";

// IDCF Cloudに関する設定
const char* action_1_uuid = "********-****-****-****-************";
const char* action_1_token = "********";
IPAddress server(210, ***, ***, ***);
