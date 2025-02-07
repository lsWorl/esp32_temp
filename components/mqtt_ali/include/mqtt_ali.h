#ifndef MQTT_ALI_H
#define MQTT_ALI_H

#include "uart_handler.h"
#include "esp_log.h"
#include "mqtt_client.h"
#include "esp_tls.h"
#include <string.h>

// 包含配置文件
#include "mqtt_config.h"

/**
 * @brief MQTT事件回调函数
 * @param handler_args 用户参数
 * @param base 事件基类型
 * @param event_id 事件ID
 * @param event_data 事件数据
 */
void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);

/**
 * @brief 初始化MQTT客户端并连接到EMQX
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
 * @brief 订阅MQTT发来的数据
 * @param topic 主题
 * @param qos 服务质量
 * @return ESP_OK 成功，其他值表示失败
 */
esp_err_t mqtt_ali_subscribe(char*topic,int qos);

/**
 * @brief 检查MQTT连接状态
 * @return true 已连接，false 未连接
 */
bool mqtt_ali_is_connected(void);


#endif // MQTT_ALI_H
