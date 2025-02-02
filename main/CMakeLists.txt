idf_component_register(
    SRCS "test.c"
    INCLUDE_DIRS "."
    REQUIRES wifi_link uart_handler tcp_server mqtt_ali esp_wifi nvs_flash esp_system freertos driver log esp_common mqtt
)
