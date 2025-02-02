idf_component_register(
    SRCS "src/wifi_link.c"        # 指定源文件路径（相对于组件目录）
    INCLUDE_DIRS "include"        # 指定头文件目录（相对于组件目录）
    REQUIRES esp_wifi nvs_flash esp_event esp_common
)