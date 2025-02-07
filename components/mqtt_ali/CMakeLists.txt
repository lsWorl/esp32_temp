idf_component_register(
    SRCS "src/mqtt_ali.c"
    INCLUDE_DIRS "include"
    REQUIRES mqtt esp_wifi nvs_flash esp_event esp_common uart_handler
)
