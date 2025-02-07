#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "wifi_link.h"
#include "uart_handler.h"
#include "tcp_server.h"
#include "mqtt_ali.h"
#include "wifi_scanner.h"

#define WIFI_SSID "white"
#define WIFI_PASS "11111111"
#define BUFFER_SIZE 512  // 定义合理的缓冲区大小

static const char *TAG = "MAIN";

void app_main(void)
{
    // 初始化NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // 初始化WiFi
    ESP_ERROR_CHECK(wifi_init_sta(WIFI_SSID, WIFI_PASS));

    // 等待WiFi连接
    while (!wifi_is_connected()) {
        vTaskDelay(pdMS_TO_TICKS(100));
    }

    // 获取并打印IP地址
    char ip_str[16];
    if (wifi_get_ip_string(ip_str, sizeof(ip_str)) == ESP_OK) {
        ESP_LOGI(TAG, "Connected to WiFi, IP: %s", ip_str);
    }

    // 初始化MQTT客户端
    ESP_ERROR_CHECK(mqtt_ali_init());

    // 等待MQTT连接
    while (!mqtt_ali_is_connected()) {
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    ESP_LOGI(TAG, "MQTT connected to EMQX");

    // 订阅MQTT主题
    const char *subscribe_topic = "data/#";
    ESP_ERROR_CHECK(mqtt_ali_subscribe(subscribe_topic, 1));  // 使用QoS 1

    // 初始化UART
    uart_init();

    // 启动TCP服务器
    ESP_ERROR_CHECK(start_tcp_server());

    // 初始化并启动WiFi扫描器
    ESP_ERROR_CHECK(wifi_scanner_init());
    ESP_ERROR_CHECK(wifi_scanner_start());
    ESP_LOGI(TAG, "WiFi scanner started");

    // 主循环：读取UART数据并发布到MQTT
    char rx_buffer[BUFFER_SIZE];
    char mqtt_data[BUFFER_SIZE];  // 减小MQTT数据缓冲区大小
    while (1) {
        int len = uart_receive_data(rx_buffer, sizeof(rx_buffer));
        if (len > 0) {
            ESP_LOGI(TAG, "Received from UART: %.*s", len, rx_buffer);
            
            // 构建JSON格式的数据，确保不会溢出
            snprintf(mqtt_data, sizeof(mqtt_data), 
                    "{\"device\":\"stm32\",\"data\":\"%.*s\"}", 
                    (len < BUFFER_SIZE/2) ? len : BUFFER_SIZE/2,  // 限制数据长度
                    rx_buffer);
            
            // 发布到MQTT
            if (mqtt_ali_publish(mqtt_data, strlen(mqtt_data)) == ESP_OK) {
                ESP_LOGI(TAG, "Published to EMQX: %s", mqtt_data);
            } else {
                ESP_LOGE(TAG, "Failed to publish to EMQX");
            }
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
