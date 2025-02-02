#ifndef MQTT_CONFIG_H
#define MQTT_CONFIG_H

// MQTT�������� - �븴�ƴ��ļ�Ϊ mqtt_config.h �������������
#define MQTT_BROKER_URL "mqtts://your-broker-url:8883"
#define MQTT_CLIENT_ID "your-client-id"
#define MQTT_USERNAME "your-username"
#define MQTT_PASSWORD "your-password"
#define MQTT_TOPIC "your/topic"

// EMQX�ĸ�֤�� - ���滻Ϊ��ķ�����֤��
static const char *MQTT_SERVER_ROOT_CERT = 
"-----BEGIN CERTIFICATE-----\n"
"Your certificate here\n"
"-----END CERTIFICATE-----\n";

#endif // MQTT_CONFIG_H 