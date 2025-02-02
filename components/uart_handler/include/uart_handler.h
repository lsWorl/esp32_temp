#ifndef UART_HANDLER_H
#define UART_HANDLER_H

#include "driver/uart.h"
#include "driver/gpio.h"
#include "esp_log.h"

// UART配置参数
#define UART_NUM UART_NUM_2
#define BUF_SIZE 1024
#define UART_TX_PIN GPIO_NUM_17
#define UART_RX_PIN GPIO_NUM_16

/**
 * @brief 初始化UART
 */
void uart_init(void);

/**
 * @brief 发送数据到UART
 * @param data 要发送的数据
 * @param len 数据长度
 * @return 发送的字节数
 */
int uart_send_data(const char* data, size_t len);

/**
 * @brief 从UART接收数据
 * @param data 接收数据的缓冲区
 * @param max_len 最大接收长度
 * @return 接收的字节数
 */
int uart_receive_data(char* data, size_t max_len);

#endif // UART_HANDLER_H 