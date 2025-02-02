#ifndef WIFI_LINK_H
#define WIFI_LINK_H

#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "lwip/err.h"
#include "lwip/sys.h"

/**
 * @brief ��ʼ��WiFiΪSTAģʽ�����ӵ�ָ��AP
 * @param ssid WiFi����
 * @param password WiFi����
 * @return ESP_OK �ɹ�������ֵ��ʾʧ��
 */
esp_err_t wifi_init_sta(const char *ssid, const char *password);

/**
 * @brief ��ȡWiFi����״̬
 * @return true �����ӣ�false δ����
 */
bool wifi_is_connected(void);

/**
 * @brief ��ȡ��ǰIP��ַ
 * @param ip_str ���ڴ洢IP��ַ���ַ���������
 * @param max_len ��������󳤶�
 * @return ESP_OK �ɹ�������ֵ��ʾʧ��
 */
esp_err_t wifi_get_ip_string(char *ip_str, size_t max_len);

#endif // WIFI_LINK_H