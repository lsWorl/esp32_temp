#include "tcp_server.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "TCP_SERVER";

void tcp_server_task(void *pvParameters)
{
    char rx_buffer[BUFFER_SIZE];
    int listen_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    // 创建TCP套接字
    listen_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (listen_sock < 0) {
        ESP_LOGE(TAG, "Failed to create socket");
        vTaskDelete(NULL);
    }

    // 配置服务器地址
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(TCP_PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // 绑定套接字
    if (bind(listen_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        ESP_LOGE(TAG, "Failed to bind socket");
        close(listen_sock);
        vTaskDelete(NULL);
    }

    // 监听连接
    if (listen(listen_sock, MAX_CLIENTS) < 0) {
        ESP_LOGE(TAG, "Failed to listen");
        close(listen_sock);
        vTaskDelete(NULL);
    }

    ESP_LOGI(TAG, "TCP server started on port %d", TCP_PORT);

    while (1) {
        // 接受客户端连接
        client_sock = accept(listen_sock, (struct sockaddr *)&client_addr, &addr_len);
        if (client_sock < 0) {
            ESP_LOGE(TAG, "Failed to accept connection");
            continue;
        }

        ESP_LOGI(TAG, "New client connected");

        // 处理客户端数据
        while (1) {
            int len = recv(client_sock, rx_buffer, sizeof(rx_buffer) - 1, 0);
            if (len < 0) {
                ESP_LOGE(TAG, "Error receiving data");
                break;
            } else if (len == 0) {
                ESP_LOGI(TAG, "Client disconnected");
                break;
            }

            rx_buffer[len] = '\0';
            ESP_LOGI(TAG, "Received: %s", rx_buffer);

            // 处理命令
            if (strstr(rx_buffer, "LED_ON") != NULL) {
                uart_send_data("1", 1);
                send(client_sock, "LED IS ON\n", 10, 0);
            } else if (strstr(rx_buffer, "LED_OFF") != NULL) {
                uart_send_data("0", 1);
                send(client_sock, "LED IS OFF\n", 11, 0);
            } else {
                send(client_sock, "Invalid command\n", 16, 0);
            }
        }

        close(client_sock);
    }

    close(listen_sock);
    vTaskDelete(NULL);
}

esp_err_t start_tcp_server(void)
{
    xTaskCreate(tcp_server_task, "tcp_server", 4096, NULL, 5, NULL);
    return ESP_OK;
} 