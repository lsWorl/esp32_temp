#ifndef MQTT_CONFIG_H
#define MQTT_CONFIG_H

// MQTT连接配置 - 请复制此文件为 mqtt_config.h 并填入你的配置
#define MQTT_BROKER_URL "mqtts://your-broker-url:8883"
#define MQTT_CLIENT_ID "your-client-id"
#define MQTT_USERNAME "your-username"
#define MQTT_PASSWORD "your-password"
#define MQTT_TOPIC "your/topic"

// EMQX的根证书 - 请替换为你的服务器证书
static const char *MQTT_SERVER_ROOT_CERT = 
"-----BEGIN CERTIFICATE-----\n"
"Your certificate here\n"
"-----END CERTIFICATE-----\n";

#endif // MQTT_CONFIG_H 