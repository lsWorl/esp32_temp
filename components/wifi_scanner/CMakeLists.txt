idf_component_register(
    SRCS "src/wifi_scanner.c"
    INCLUDE_DIRS "include"
    REQUIRES esp_wifi mqtt_ali json esp_wifi nvs_flash esp_event esp_common
) 