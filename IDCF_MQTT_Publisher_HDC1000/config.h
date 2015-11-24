// MQTTのクライアントID（ユニークにするためにスケッチをコンパイルした日付と時刻を使用）
const char *mqtt_client_id = "publisher" __DATE__ __TIME__;

// Wi-FiアクセスポイントのSSIDとパスワード
const char *ssid = "********";
const char *password = "********";

// IDCF Cloudに関する設定
const char *server = "210-***-***-***.jp-east.compute.idcfcloud.com";
const char* trigger_1_uuid = "********-****-****-****-************";
const char* trigger_1_token = "********";
const char* action_1_uuid = "********-****-****-****-************";
const char* action_2_uuid = "********-****-****-****-************";

