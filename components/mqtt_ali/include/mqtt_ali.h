#ifndef MQTT_ALI_H
#define MQTT_ALI_H

#include "esp_log.h"
#include "mqtt_client.h"
#include "esp_tls.h"
#include <string.h>

// ������������ƽ̨���Ӳ���
#define PRODUCT_KEY "k29vejNwYJr"
#define DEVICE_NAME "ESP32"
#define DEVICE_SECRET "1f5a6ce9738d7237e8951eed08111334"

#define MQTT_BROKER_URL "mqtt://iot-06z00a4sk1miiui.mqtt.iothub.aliyuncs.com:1883"
#define MQTT_CLIENT_ID "k29vejNwYJr.ESP32|securemode=2,signmethod=hmacsha256,timestamp=1738390328837|"
#define MQTT_USERNAME "ESP32&k29vejNwYJr"
#define MQTT_PASSWORD "a648c76454b5fa359984edfaacd2971215101fb58f6177baddc5c4829c3bde4d"  // ������Ҫʹ����ȷ��HMAC-SHA256���������
#define MQTT_TOPIC "/sys/k29vejNwYJr/ESP32/thing/event/property/post"

/**
 * @brief MQTT�¼��ص�����
 * @param handler_args �û�����
 * @param base �¼�������
 * @param event_id �¼�ID
 * @param event_data �¼�����
 */
void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);

/**
 * @brief ��ʼ��MQTT�ͻ��˲����ӵ�������
 * @return ESP_OK �ɹ�������ֵ��ʾʧ��
 */
esp_err_t mqtt_ali_init(void);

/**
 * @brief �������ݵ�MQTT����
 * @param data Ҫ����������
 * @param len ���ݳ���
 * @return ESP_OK �ɹ�������ֵ��ʾʧ��
 */
esp_err_t mqtt_ali_publish(const char *data, size_t len);

/**
 * @brief ���MQTT����״̬
 * @return true �����ӣ�false δ����
 */
bool mqtt_ali_is_connected(void);

#endif // MQTT_ALI_H
