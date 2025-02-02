#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include "esp_log.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "uart_handler.h"

// TCP服务器配置
#define TCP_PORT 80
#define MAX_CLIENTS 1
#define BUFFER_SIZE 1024

/**
 * @brief TCP服务器任务
 * @param pvParameters FreeRTOS任务参数
 */
void tcp_server_task(void *pvParameters);

/**
 * @brief 启动TCP服务器
 * @return ESP_OK 成功，其他值表示失败
 */
esp_err_t start_tcp_server(void);

#endif // TCP_SERVER_H 