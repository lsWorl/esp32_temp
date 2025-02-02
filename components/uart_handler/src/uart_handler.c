#include "uart_handler.h"

static const char *TAG = "UART_HANDLER";

void uart_init(void)
{
    uart_config_t uart_config = {
        .baud_rate = 115200,                   // 波特率
        .data_bits = UART_DATA_8_BITS,         // 数据位8
        .parity = UART_PARITY_DISABLE,         // 无校验
        .stop_bits = UART_STOP_BITS_1,         // 停止位1
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE, // 硬件流控制关闭
        .source_clk = UART_SCLK_APB,           // 时钟源
    };
    
    ESP_ERROR_CHECK(uart_param_config(UART_NUM, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(UART_NUM, UART_TX_PIN, UART_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    ESP_ERROR_CHECK(uart_driver_install(UART_NUM, BUF_SIZE * 2, 0, 0, NULL, 0));
    
    ESP_LOGI(TAG, "UART initialized successfully");
}

int uart_send_data(const char* data, size_t len)
{
    if (data == NULL || len == 0) {
        ESP_LOGE(TAG, "Invalid parameters for uart_send_data");
        return -1;
    }
    
    int written = uart_write_bytes(UART_NUM, data, len);
    if (written < 0) {
        ESP_LOGE(TAG, "Failed to send data through UART");
        return -1;
    }
    
    return written;
}

int uart_receive_data(char* data, size_t max_len)
{
    if (data == NULL || max_len == 0) {
        ESP_LOGE(TAG, "Invalid parameters for uart_receive_data");
        return -1;
    }
    
    int len = uart_read_bytes(UART_NUM, (uint8_t*)data, max_len, 1000);
    if (len > 0) {
        ESP_LOGI(TAG, "Received %d bytes", len);
    }
    
    return len;
} 