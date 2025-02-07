#ifndef MQTT_ALI_H
#define MQTT_ALI_H

#include "uart_handler.h"
#include "esp_log.h"
#include "mqtt_client.h"
#include "esp_tls.h"
#include <string.h>

// ���������ļ�
#include "mqtt_config.h"

/**
 * @brief MQTT�¼��ص�����
 * @param handler_args �û�����
 * @param base �¼�������
 * @param event_id �¼�ID
 * @param event_data �¼�����
 */
void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);

/**
 * @brief ��ʼ��MQTT�ͻ��˲����ӵ�EMQX
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
 * @brief ����MQTT����������
 * @param topic ����
 * @param qos ��������
 * @return ESP_OK �ɹ�������ֵ��ʾʧ��
 */
esp_err_t mqtt_ali_subscribe(char*topic,int qos);

/**
 * @brief ���MQTT����״̬
 * @return true �����ӣ�false δ����
 */
bool mqtt_ali_is_connected(void);


#endif // MQTT_ALI_H
