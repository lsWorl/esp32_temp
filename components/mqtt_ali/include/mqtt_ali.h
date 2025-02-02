#ifndef MQTT_ALI_H
#define MQTT_ALI_H

#include "esp_log.h"
#include "mqtt_client.h"
#include "esp_tls.h"
#include <string.h>

// 阿里云物联网平台连接参数
#define PRODUCT_KEY "k29vejNwYJr"
#define DEVICE_NAME "ESP32"
#define DEVICE_SECRET "1f5a6ce9738d7237e8951eed08111334"

#define MQTT_BROKER_URL "mqtt://iot-06z00a4sk1miiui.mqtt.iothub.aliyuncs.com:1883"
#define MQTT_CLIENT_ID "k29vejNwYJr.ESP32|securemode=2,signmethod=hmacsha256,timestamp=1738390328837|"
#define MQTT_USERNAME "ESP32&k29vejNwYJr"
#define MQTT_PASSWORD "a648c76454b5fa359984edfaacd2971215101fb58f6177baddc5c4829c3bde4d"  // 这里需要使用正确的HMAC-SHA256计算的密码
#define MQTT_TOPIC "/sys/k29vejNwYJr/ESP32/thing/event/property/post"

/**
 * @brief MQTT事件回调函数
 * @param handler_args 用户参数
 * @param base 事件基类型
 * @param event_id 事件ID
 * @param event_data 事件数据
 */
void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);

/**
 * @brief 初始化MQTT客户端并连接到阿里云
 * @return ESP_OK 成功，其他值表示失败
 */
esp_err_t mqtt_ali_init(void);

/**
 * @brief 发布数据到MQTT主题
 * @param data 要发布的数据
 * @param len 数据长度
 * @return ESP_OK 成功，其他值表示失败
 */
esp_err_t mqtt_ali_publish(const char *data, size_t len);

/**
 * @brief 检查MQTT连接状态
 * @return true 已连接，false 未连接
 */
bool mqtt_ali_is_connected(void);

#endif // MQTT_ALI_H
