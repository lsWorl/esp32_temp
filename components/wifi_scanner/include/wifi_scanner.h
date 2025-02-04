#ifndef WIFI_SCANNER_H
#define WIFI_SCANNER_H

#include "esp_wifi.h"
#include "esp_log.h"
#include "cJSON.h"
#include "mqtt_ali.h"

// 扫描配置
#define MAX_AP_NUM 20                  // 最大AP数量
#define SCAN_INTERVAL_MS 30000         // 扫描间隔(ms)
#define WIFI_SCAN_TOPIC "esp32/wifi"   // MQTT主题

/**
 * @brief 初始化WiFi扫描器
 * @return ESP_OK 成功，其他值表示失败
 */
esp_err_t wifi_scanner_init(void);

/**
 * @brief 启动WiFi扫描任务
 * @return ESP_OK 成功，其他值表示失败
 */
esp_err_t wifi_scanner_start(void);

/**
 * @brief 停止WiFi扫描任务
 */
void wifi_scanner_stop(void);

#endif // WIFI_SCANNER_H 