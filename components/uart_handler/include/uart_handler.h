#ifndef UART_HANDLER_H
#define UART_HANDLER_H

#include "driver/uart.h"
#include "driver/gpio.h"
#include "esp_log.h"

// UART���ò���
#define UART_NUM UART_NUM_2
#define BUF_SIZE 1024
#define UART_TX_PIN GPIO_NUM_17
#define UART_RX_PIN GPIO_NUM_16

/**
 * @brief ��ʼ��UART
 */
void uart_init(void);

/**
 * @brief �������ݵ�UART
 * @param data Ҫ���͵�����
 * @param len ���ݳ���
 * @return ���͵��ֽ���
 */
int uart_send_data(const char* data, size_t len);

/**
 * @brief ��UART��������
 * @param data �������ݵĻ�����
 * @param max_len �����ճ���
 * @return ���յ��ֽ���
 */
int uart_receive_data(char* data, size_t max_len);

#endif // UART_HANDLER_H 