#include "wifi_scanner.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

static const char *TAG = "WIFI_SCANNER";
static TaskHandle_t scanner_task_handle = NULL;
static bool is_scanning = false;

// 将WiFi扫描结果转换为JSON字符串
static char* wifi_scan_to_json(wifi_ap_record_t *ap_records, uint16_t ap_count) {
    cJSON *root = cJSON_CreateObject();
    cJSON *ap_list = cJSON_CreateArray();
    
    // 添加设备信息
    cJSON_AddStringToObject(root, "device_id", "ESP32_SCANNER");
    cJSON_AddNumberToObject(root, "timestamp", esp_log_timestamp());
    
    // 添加AP列表
    for (int i = 0; i < ap_count; i++) {
        cJSON *ap = cJSON_CreateObject();
        char bssid_str[18];
        
        // 格式化BSSID
        snprintf(bssid_str, sizeof(bssid_str), "%02X:%02X:%02X:%02X:%02X:%02X",
                ap_records[i].bssid[0], ap_records[i].bssid[1], ap_records[i].bssid[2],
                ap_records[i].bssid[3], ap_records[i].bssid[4], ap_records[i].bssid[5]);
        
        cJSON_AddStringToObject(ap, "ssid", (char *)ap_records[i].ssid);
        cJSON_AddStringToObject(ap, "bssid", bssid_str);
        cJSON_AddNumberToObject(ap, "rssi", ap_records[i].rssi);
        cJSON_AddNumberToObject(ap, "channel", ap_records[i].primary);
        
        cJSON_AddItemToArray(ap_list, ap);
    }
    
    cJSON_AddItemToObject(root, "ap_list", ap_list);
    
    char *json_str = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);
    
    return json_str;
}

// WiFi扫描任务
static void wifi_scanner_task(void *pvParameters) {
    wifi_ap_record_t ap_records[MAX_AP_NUM];
    uint16_t ap_count = 0;
    
    while (is_scanning) {
        ESP_LOGI(TAG, "开始WiFi扫描...");
        
        // 开始扫描
        esp_wifi_scan_start(NULL, true);
        
        // 获取扫描结果
        esp_wifi_scan_get_ap_num(&ap_count);
        if (ap_count > MAX_AP_NUM) {
            ap_count = MAX_AP_NUM;
        }
        
        ESP_LOGI(TAG, "找到 %d 个AP", ap_count);
        
        if (ap_count > 0) {
            esp_wifi_scan_get_ap_records(&ap_count, ap_records);
            
            // 转换为JSON并通过MQTT发送
            char *json_str = wifi_scan_to_json(ap_records, ap_count);
            if (json_str != NULL) {
                if (mqtt_ali_is_connected()) {
                    esp_err_t err = mqtt_ali_publish(json_str, strlen(json_str));
                    if (err == ESP_OK) {
                        ESP_LOGI(TAG, "成功发送WiFi扫描结果");
                    } else {
                        ESP_LOGE(TAG, "发送WiFi扫描结果失败");
                    }
                } else {
                    ESP_LOGW(TAG, "MQTT未连接，无法发送数据");
                }
                free(json_str);
            }
        }
        
        // 等待指定时间后再次扫描
        vTaskDelay(pdMS_TO_TICKS(SCAN_INTERVAL_MS));
    }
    
    scanner_task_handle = NULL;
    vTaskDelete(NULL);
}

esp_err_t wifi_scanner_init(void) {
    wifi_scan_config_t scan_config = {
        .ssid = NULL,
        .bssid = NULL,
        .channel = 0,
        .show_hidden = true,
        .scan_type = WIFI_SCAN_TYPE_ACTIVE,
        .scan_time.active.min = 100,
        .scan_time.active.max = 300
    };
    
    esp_err_t err = esp_wifi_set_mode(WIFI_MODE_STA);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "设置WiFi模式失败");
        return err;
    }
    
    err = esp_wifi_scan_start(&scan_config, true);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "初始WiFi扫描失败");
        return err;
    }
    
    return ESP_OK;
}

esp_err_t wifi_scanner_start(void) {
    if (is_scanning) {
        ESP_LOGW(TAG, "WiFi扫描器已在运行");
        return ESP_OK;
    }
    
    is_scanning = true;
    BaseType_t ret = xTaskCreate(wifi_scanner_task,
                                "wifi_scanner",
                                4096,
                                NULL,
                                5,
                                &scanner_task_handle);
                                
    if (ret != pdPASS) {
        ESP_LOGE(TAG, "创建WiFi扫描任务失败");
        is_scanning = false;
        return ESP_FAIL;
    }
    
    return ESP_OK;
}

void wifi_scanner_stop(void) {
    is_scanning = false;
    if (scanner_task_handle != NULL) {
        vTaskDelay(pdMS_TO_TICKS(100));  // 给任务一些时间完成
    }
} 