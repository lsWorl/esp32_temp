#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include "esp_log.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "uart_handler.h"

// TCP����������
#define TCP_PORT 80
#define MAX_CLIENTS 1
#define BUFFER_SIZE 1024

/**
 * @brief TCP����������
 * @param pvParameters FreeRTOS�������
 */
void tcp_server_task(void *pvParameters);

/**
 * @brief ����TCP������
 * @return ESP_OK �ɹ�������ֵ��ʾʧ��
 */
esp_err_t start_tcp_server(void);

#endif // TCP_SERVER_H 