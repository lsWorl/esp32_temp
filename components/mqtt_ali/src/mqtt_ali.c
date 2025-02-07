#include "mqtt_ali.h"

static const char *TAG = "MQTT_ALI";
static esp_mqtt_client_handle_t mqtt_client = NULL;
static bool mqtt_connected = false;

// MQTT事件处理函数
void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = event_data;

    switch ((esp_mqtt_event_id_t)event_id)
    {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT Connected to broker");
        mqtt_connected = true;
        break;

    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT Disconnected from broker");
        mqtt_connected = false;
        break;

    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        break;

    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;

    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;

    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "MQTT_EVENT_DATA");
        //接收订阅到的数据
        printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
        printf("DATA=%.*s\r\n", event->data_len, event->data);
        //将订阅到的数据通过uart发送到stm32
        uart_send_data(event->data, event->data_len);
        break;

    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT)
        {
            ESP_LOGI(TAG, "Last error code reported from esp-tls: 0x%x", event->error_handle->esp_tls_last_esp_err);
            ESP_LOGI(TAG, "Last tls stack error number: 0x%x", event->error_handle->esp_tls_stack_err);
            ESP_LOGI(TAG, "Last captured errno : %d (%s)", event->error_handle->esp_transport_sock_errno,
                     strerror(event->error_handle->esp_transport_sock_errno));
        }
        break;

    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}

esp_err_t mqtt_ali_init(void)
{
    // MQTT客户端配置
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = MQTT_BROKER_URL,
        .credentials.client_id = MQTT_CLIENT_ID,
        .credentials.username = MQTT_USERNAME,
        .credentials.authentication.password = MQTT_PASSWORD,
        .broker.verification.certificate = MQTT_SERVER_ROOT_CERT,
    };

    // 创建MQTT客户端
    mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
    if (mqtt_client == NULL)
    {
        ESP_LOGE(TAG, "Failed to initialize MQTT client");
        return ESP_FAIL;
    }

    // 注册MQTT事件处理函数
    ESP_ERROR_CHECK(esp_mqtt_client_register_event(mqtt_client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL));

    // 启动MQTT客户端
    ESP_ERROR_CHECK(esp_mqtt_client_start(mqtt_client));

    ESP_LOGI(TAG, "MQTT client initialized");
    return ESP_OK;
}

esp_err_t mqtt_ali_publish(const char *data, size_t len)
{
    if (!mqtt_connected || mqtt_client == NULL)
    {
        ESP_LOGE(TAG, "MQTT not connected");
        return ESP_FAIL;
    }

    int msg_id = esp_mqtt_client_publish(mqtt_client, MQTT_TOPIC, data, len, 1, 0);
    if (msg_id < 0)
    {
        ESP_LOGE(TAG, "Failed to publish message");
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Published message successfully, msg_id=%d", msg_id);
    return ESP_OK;
}

esp_err_t mqtt_ali_subscribe(char *topic, int qos)
{
    if (!mqtt_connected || mqtt_client == NULL)
    {
        ESP_LOGE(TAG, "MQTT not connected, cannot subscribe");
        return ESP_FAIL;
    }
    // 订阅主题
    int msg_id = esp_mqtt_client_subscribe(mqtt_client, topic, qos);
    if (msg_id < 0)
    {
        ESP_LOGE(TAG, "Subscribe failed");
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "Subscribed to topic %s, msg_id=%d", topic, msg_id);
    return ESP_OK;
}



bool mqtt_ali_is_connected(void)
{
    return mqtt_connected;
}
