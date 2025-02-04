#include "wifi_scanner.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

static const char *TAG = "WIFI_SCANNER";
static TaskHandle_t scanner_task_handle = NULL;
static bool is_scanning = false;

// ��WiFiɨ����ת��ΪJSON�ַ���
static char* wifi_scan_to_json(wifi_ap_record_t *ap_records, uint16_t ap_count) {
    cJSON *root = cJSON_CreateObject();
    cJSON *ap_list = cJSON_CreateArray();
    
    // ����豸��Ϣ
    cJSON_AddStringToObject(root, "device_id", "ESP32_SCANNER");
    cJSON_AddNumberToObject(root, "timestamp", esp_log_timestamp());
    
    // ���AP�б�
    for (int i = 0; i < ap_count; i++) {
        cJSON *ap = cJSON_CreateObject();
        char bssid_str[18];
        
        // ��ʽ��BSSID
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

// WiFiɨ������
static void wifi_scanner_task(void *pvParameters) {
    wifi_ap_record_t ap_records[MAX_AP_NUM];
    uint16_t ap_count = 0;
    
    while (is_scanning) {
        ESP_LOGI(TAG, "��ʼWiFiɨ��...");
        
        // ��ʼɨ��
        esp_wifi_scan_start(NULL, true);
        
        // ��ȡɨ����
        esp_wifi_scan_get_ap_num(&ap_count);
        if (ap_count > MAX_AP_NUM) {
            ap_count = MAX_AP_NUM;
        }
        
        ESP_LOGI(TAG, "�ҵ� %d ��AP", ap_count);
        
        if (ap_count > 0) {
            esp_wifi_scan_get_ap_records(&ap_count, ap_records);
            
            // ת��ΪJSON��ͨ��MQTT����
            char *json_str = wifi_scan_to_json(ap_records, ap_count);
            if (json_str != NULL) {
                if (mqtt_ali_is_connected()) {
                    esp_err_t err = mqtt_ali_publish(json_str, strlen(json_str));
                    if (err == ESP_OK) {
                        ESP_LOGI(TAG, "�ɹ�����WiFiɨ����");
                    } else {
                        ESP_LOGE(TAG, "����WiFiɨ����ʧ��");
                    }
                } else {
                    ESP_LOGW(TAG, "MQTTδ���ӣ��޷���������");
                }
                free(json_str);
            }
        }
        
        // �ȴ�ָ��ʱ����ٴ�ɨ��
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
        ESP_LOGE(TAG, "����WiFiģʽʧ��");
        return err;
    }
    
    err = esp_wifi_scan_start(&scan_config, true);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "��ʼWiFiɨ��ʧ��");
        return err;
    }
    
    return ESP_OK;
}

esp_err_t wifi_scanner_start(void) {
    if (is_scanning) {
        ESP_LOGW(TAG, "WiFiɨ������������");
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
        ESP_LOGE(TAG, "����WiFiɨ������ʧ��");
        is_scanning = false;
        return ESP_FAIL;
    }
    
    return ESP_OK;
}

void wifi_scanner_stop(void) {
    is_scanning = false;
    if (scanner_task_handle != NULL) {
        vTaskDelay(pdMS_TO_TICKS(100));  // ������һЩʱ�����
    }
} 