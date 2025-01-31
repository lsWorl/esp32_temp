#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "lwip/sockets.h"
#include "driver/uart.h"

#define WIFI_SSID "white"
#define WIFI_PASS "11111111"
// tcp server port
#define PORT 80
// tcp server max clients
#define MAX_CLIENTS 1

// uart config
#define UART_NUM UART_NUM_2
#define BUF_SIZE 1024
#define UART_TX_PIN GPIO_NUM_17
#define UART_RX_PIN GPIO_NUM_16

// 标识
static const char *TAG = "TCP_SERVER";

// wifi init
static void wifi_init_sta(void)
{
  esp_netif_init();
  esp_event_loop_create_default();
  esp_netif_create_default_wifi_sta();

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  esp_wifi_init(&cfg);
  wifi_config_t wifi_config = {
      .sta = {
          .ssid = WIFI_SSID,
          .password = WIFI_PASS,
      },
  };
  esp_wifi_set_mode(WIFI_MODE_STA);
  esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config);
  esp_wifi_start();

  ESP_LOGI(TAG, "Connecting to Wifi...");
  esp_wifi_connect();

  // 等待 Wi-Fi 连接
  while (1)
  {
    if (esp_wifi_connect() == ESP_OK)
    {
      break;
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
  // 获取 IP 地址
  esp_netif_ip_info_t ip_info;
  esp_netif_get_ip_info(esp_netif_get_handle_from_ifkey("WIFI_STA_DEF"), &ip_info);
  ESP_LOGI(TAG, "Connected to WiFi");
  ESP_LOGI(TAG, "IP Address: " IPSTR, IP2STR(&ip_info.ip));
}

// UART init
static void uart_init(void)
{
  uart_config_t uart_config = {
      .baud_rate = 115200,                   // 波特率
      .data_bits = UART_DATA_8_BITS,         // 数据位8
      .parity = UART_PARITY_DISABLE,         // 无校验
      .stop_bits = UART_STOP_BITS_1,         // 停止位1
      .flow_ctrl = UART_HW_FLOWCTRL_DISABLE, // 硬件流控制关闭
      .source_clk = UART_SCLK_APB,           // 时钟源
  };
  uart_param_config(UART_NUM, &uart_config);
  uart_set_pin(UART_NUM, UART_TX_PIN, UART_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
  uart_driver_install(UART_NUM, BUF_SIZE * 2, 0, 0, NULL, 0);
}

// tcp server task
static void tcp_server_task(void *pvParameter)
{
  char rx_buffer[BUF_SIZE];
  int listen_sock, client_sock;
  struct sockaddr_in server_addr, client_addr;
  socklen_t addr_len = sizeof(client_addr);

  // 创建tcp套接字
  // AF_INET:IPv4协议族
  // SOCK_STREAM:TCP协议
  // IPPROTO_IP:IP协议
  listen_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
  if (listen_sock < 0)
  {
    ESP_LOGE(TAG, "Failed to create socket");
    vTaskDelete(NULL);
  }

  // 设置地址族为IPv4
  server_addr.sin_family = AF_INET;
  // 设置端口号 htons将端口号从主机字节序转换为网络字节序
  server_addr.sin_port = htons(PORT);
  // 设置IP地址为0.0.0.0
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  // 绑定套接字到指定的地址和端口
  if (bind(listen_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
  {
    ESP_LOGE(TAG, "Failed to bind socket");
    close(listen_sock);
    vTaskDelete(NULL);
  }

  // 监听连接
  if (listen(listen_sock, MAX_CLIENTS) < 0)
  {
    ESP_LOGE(TAG, "Failed to listen on socket");
    close(listen_sock);
    vTaskDelete(NULL);
  }

  ESP_LOGI(TAG, "TCP server started, listening on port %d", PORT);

  while (1)
  {
    // 接受客户端连接
    client_sock = accept(listen_sock, (struct sockaddr *)&client_addr, &addr_len);
    if (client_sock < 0)
    {
      ESP_LOGE(TAG, "Failed to accept connection");
      continue;
    }
    ESP_LOGI(TAG, "New client connected");

    // 处理客户端数据
    while (1)
    {
      // 接收客户端数据存储到rx_buffer
      int len = recv(client_sock, rx_buffer, sizeof(rx_buffer) - 1, 0);
      if (len < 0)
      {
        ESP_LOGE(TAG, "Failed to receive data");
        break;
      }
      else if (len == 0)
      {
        ESP_LOGI(TAG, "Client disconnected");
        break;
      }
      rx_buffer[len] = '\0';
      ESP_LOGI(TAG, "Received data: %s", rx_buffer);
      // 根据接收到的数据控制STM32
      if (strstr(rx_buffer, "LED_ON") != NULL)
      {
        uart_write_bytes(UART_NUM, "1", 1); // 发送1到STM32
        send(client_sock, "LED IS ON\n", 10, 0);
      }
      else if (strstr(rx_buffer, "LED_OFF") != NULL)
      {
        uart_write_bytes(UART_NUM, "0", 1); // 发送0到STM32
        send(client_sock, "LED IS OFF\n", 11, 0);
      }
      else
      {
        send(client_sock, "Invalid command\n", 16, 0);
      }
    }
  }
  // 关闭监听套接字
  close(listen_sock);
  vTaskDelete(NULL);
}

void app_main(void)
{
  // 初始化NVS
  esp_err_t err = nvs_flash_init();
  if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
  {
    ESP_ERROR_CHECK(nvs_flash_erase());
    err = nvs_flash_init();
  }
  ESP_ERROR_CHECK(err);

  // 初始化wifi
  wifi_init_sta();

  // 初始化UART
  uart_init();

  // 创建 TCP 服务器任务
  xTaskCreate(tcp_server_task, "tcp_server", 4096, NULL, 5, NULL);

  char rx_buffer[BUF_SIZE];
  while (1)
  {
    // 读取stm32发送的数据
    int len = uart_read_bytes(UART_NUM, rx_buffer, sizeof(rx_buffer), 0);
    if (len > 0)
    {
      ESP_LOGI("uart_Test", "Received %d bytes: %.*s", len, len, rx_buffer);
    }
    // 延时100ms
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}
