#ifndef WIFI_SCANNER_H
#define WIFI_SCANNER_H

#include "esp_wifi.h"
#include "esp_log.h"
#include "cJSON.h"
#include "mqtt_ali.h"

// ɨ������
#define MAX_AP_NUM 20                  // ���AP����
#define SCAN_INTERVAL_MS 30000         // ɨ����(ms)
#define WIFI_SCAN_TOPIC "esp32/wifi"   // MQTT����

/**
 * @brief ��ʼ��WiFiɨ����
 * @return ESP_OK �ɹ�������ֵ��ʾʧ��
 */
esp_err_t wifi_scanner_init(void);

/**
 * @brief ����WiFiɨ������
 * @return ESP_OK �ɹ�������ֵ��ʾʧ��
 */
esp_err_t wifi_scanner_start(void);

/**
 * @brief ֹͣWiFiɨ������
 */
void wifi_scanner_stop(void);

#endif // WIFI_SCANNER_H 