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
 * @brief 初始化WiFi为STA模式并连接到指定AP
 * @param ssid WiFi名称
 * @param password WiFi密码
 * @return ESP_OK 成功，其他值表示失败
 */
esp_err_t wifi_init_sta(const char *ssid, const char *password);

/**
 * @brief 获取WiFi连接状态
 * @return true 已连接，false 未连接
 */
bool wifi_is_connected(void);

/**
 * @brief 获取当前IP地址
 * @param ip_str 用于存储IP地址的字符串缓冲区
 * @param max_len 缓冲区最大长度
 * @return ESP_OK 成功，其他值表示失败
 */
esp_err_t wifi_get_ip_string(char *ip_str, size_t max_len);

#endif // WIFI_LINK_H