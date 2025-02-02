idf_component_register(
    SRCS "src/tcp_server.c"
    INCLUDE_DIRS "include"
    REQUIRES lwip uart_handler freertos esp_common
) 