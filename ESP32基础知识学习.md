# ESP32 与 FreeRTOS 基础知识

## 1. FreeRTOS 任务管理
### 任务创建
在ESP32中，我们使用 `xTaskCreatePinnedToCore()` 函数创建任务。这个函数的主要参数包括：
- 任务函数
- 任务名称
- 堆栈大小（例如2048字节）
- 任务参数
- 任务优先级（0-25，数字越大优先级越高）
- 任务句柄
- 核心选择（ESP32有两个核心：0和1）

### 任务延时
使用 `vTaskDelay()` 函数可以让任务进入阻塞状态一段时间。
- `pdMS_TO_TICKS()` 用于将毫秒转换为系统节拍数

## 2. FreeRTOS 队列
### 队列概述
队列是FreeRTOS中最基本的任务间通信方式，用于在不同任务间传递数据。

### 队列操作
1. 创建队列
   - 使用 `xQueueCreate()` 创建队列
   - 需要指定队列长度和每个数据项的大小

2. 发送数据到队列
   - 使用 `xQueueSend()` 发送数据
   - 可以设置超时时间（如果队列已满，等待多久）

3. 从队列接收数据
   - 使用 `xQueueReceive()` 接收数据
   - 可以设置超时时间（如果队列为空，等待多久）
   - 返回 `pdTRUE` 表示成功接收数据

## 3. ESP32 日志系统
ESP32提供了一个强大的日志系统，使用 `esp_log.h` 头文件中的函数：
- `ESP_LOGI()`: 输出信息级别的日志
- `ESP_LOGW()`: 输出警告级别的日志
- `ESP_LOGE()`: 输出错误级别的日志

## 4. 多核编程
ESP32是双核处理器：
- Core 0: 通常用于处理WiFi和BLE等无线通信
- Core 1: 通常用于运行用户应用程序
- 使用 `xTaskCreatePinnedToCore()` 可以指定任务运行在特定核心上

## 5. 内存管理
- 在ESP32中要注意堆栈大小的设置
- 使用结构体时建议使用 `memset()` 初始化，避免出现未定义的行为

## 6. FreeRTOS 信号量
### 二进制信号量
- 使用 `xSemaphoreCreateBinary()` 创建二进制信号量
- 只有两个状态：可用（1）和不可用（0）
- 常用于任务同步、事件通知等场景
- 主要操作函数：
  - `xSemaphoreGive()`: 释放信号量
  - `xSemaphoreTake()`: 获取信号量
  - 可以设置等待超时时间，`portMAX_DELAY` 表示永久等待

### 互斥锁（Mutex）
- 使用 `xSemaphoreCreateMutex()` 创建互斥锁
- 用于保护共享资源，防止多个任务同时访问
- 特点：
  - 支持优先级继承，避免优先级反转问题
  - 必须由获取者释放
- 主要操作函数：
  - `xSemaphoreTake()`: 获取互斥锁
  - `xSemaphoreGive()`: 释放互斥锁

## 7. FreeRTOS 事件组
### 事件组概述
- 使用 `xEventGroupCreate()` 创建事件组
- 事件组是一个包含多个事件位的集合
- 每个事件位可以表示一个二进制状态（0或1）
- 常用于多事件等待和同步场景

### 事件组操作
1. 设置事件位
   - 使用 `xEventGroupSetBits()` 设置一个或多个事件位
   - 可以同时设置多个事件位（使用位运算）

2. 等待事件位
   - 使用 `xEventGroupWaitBits()` 等待事件位
   - 参数说明：
     - 要等待的事件位（可以是多个）
     - 是否在返回前清除事件位（pdTRUE/pdFALSE）
     - 是否需要等待所有事件位（pdTRUE：与等待，pdFALSE：或等待）
     - 超时时间

### 使用示例
```c
// 定义事件位
#define NUM0_BIT BIT0
#define NUM1_BIT BIT1

// 创建事件组句柄
EventGroupHandle_t test_event;

// 设置事件位的任务
void task1(void *pvParameters) {
    while (1) {
        xEventGroupSetBits(test_event, NUM0_BIT);    // 设置事件位0
        vTaskDelay(pdMS_TO_TICKS(1000));
        xEventGroupSetBits(test_event, NUM1_BIT);    // 设置事件位1
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

// 等待事件位的任务
void task2(void *pvParameters) {
    EventBits_t ev;
    while (1) {
        // 等待任意一个事件位被设置
        ev = xEventGroupWaitBits(test_event,
                                NUM0_BIT | NUM1_BIT,  // 等待的事件位
                                pdTRUE,               // 清除事件位
                                pdFALSE,              // 或等待
                                portMAX_DELAY);       // 永久等待
        
        if (ev & NUM0_BIT) {
            ESP_LOGI("task2", "event bit 0 is set");
        } else if (ev & NUM1_BIT) {
            ESP_LOGI("task2", "event bit 1 is set");
        }
    }
}
```

## 8. FreeRTOS 任务通知
### 任务通知概述
- 轻量级的任务间通信机制
- 每个任务都有一个32位的通知值
- 比队列和信号量更快，内存占用更少
- 适用于一对一的任务通信场景

### 任务通知操作
1. 发送任务通知
   - 使用 `xTaskNotify()` 发送通知
   - 通知更新选项：
     - eSetValueWithOverwrite：直接覆盖通知值
     - eSetBits：按位或操作
     - eIncrement：递增通知值
     - eNoAction：仅通知，不修改值

2. 接收任务通知
   - 使用 `xTaskNotifyWait()` 等待和接收通知
   - 可以指定超时时间
   - 可以获取通知值

### 使用示例
```c
TaskHandle_t task4_handle;

// 发送任务通知的任务
void task3(void *pvParameters) {
    uint32_t notify_value = 0;
    while (1) {
        // 发送递增的通知值
        xTaskNotify(task4_handle,
                   notify_value,
                   eSetValueWithOverwrite);  // 覆盖方式
        notify_value++;
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

// 接收任务通知的任务
void task4(void *pvParameters) {
    uint32_t value;
    while (1) {
        // 等待任务通知
        xTaskNotifyWait(0,                // 入口时不清除任何位
                       ULONG_MAX,         // 退出时清除所有位
                       &value,            // 存储通知值
                       portMAX_DELAY);    // 永久等待
        ESP_LOGI("task4", "task notify is received, value:%"PRId32, value);
    }
}
```

## 实际应用示例
### 1. 生产者-消费者模型（使用队列）
- task2（生产者）每秒产生一个递增的数值并发送到队列
- task1（消费者）从队列中读取数据并打印
- 两个任务都运行在核心1上
- 使用队列实现了任务间的安全通信

### 2. 任务同步（使用二进制信号量）
- task1作为生产者，每秒释放一次信号量
- task2作为消费者，通过等待信号量来同步操作
- 使用二进制信号量实现了任务间的同步机制

### 3. 共享资源保护（使用互斥锁）
- count_inc和count_dec任务共享同一个计数器变量
- 使用互斥锁保护计数器的读写操作
- count_inc每次将计数器加2
- count_dec每次将计数器减1
- 通过互斥锁确保了共享资源的安全访问

### 4. 多事件同步（使用事件组）
- task1定时设置不同的事件位
- task2等待并响应不同的事件
- 通过事件组实现了多个事件的同步管理

### 5. 轻量级通信（使用任务通知）
- task3在核心0上运行，定时发送递增的通知值
- task4在核心1上运行，等待并处理通知值
- 展示了跨核心的轻量级任务间通信

## 注意事项
1. 在使用队列时要注意超时处理
2. 合理设置任务优先级
3. 注意内存的合理分配和使用
4. 多核编程时要注意资源竞争问题
5. 使用互斥锁时要注意避免死锁
6. 信号量和互斥锁的选择：
   - 用于任务同步时，选择二进制信号量
   - 用于共享资源保护时，选择互斥锁
7. 事件组的选择：
   - 适用于多个事件的同步场景
   - 可以实现与等待和或等待
8. 任务通知的使用建议：
   - 优先考虑用于一对一的任务通信
   - 注意通知值的覆盖问题
   - 跨核心通信时要考虑同步开销

## 9. ESP32项目模块化实践
### 模块化的意义
1. 代码组织更清晰
   - 每个功能模块独立封装
   - 降低代码耦合度
   - 提高代码可维护性
   - 便于团队协作开发

2. 代码复用性更好
   - 模块可以在不同项目中重复使用
   - 避免代码重复编写
   - 降低维护成本

3. 更易于测试和调试
   - 可以独立测试每个模块
   - 问题定位更准确
   - 修改影响范围可控

### 项目结构组织
```
project/
├── CMakeLists.txt              # 项目主CMake文件
├── main/                       # 主程序目录
│   ├── CMakeLists.txt         # 主程序CMake文件
│   └── test.c                 # 主程序源文件
└── components/                 # 组件目录
    ├── uart_handler/          # UART处理组件
    │   ├── CMakeLists.txt     # 组件CMake文件
    │   ├── include/           # 头文件目录
    │   │   └── uart_handler.h
    │   └── src/              # 源文件目录
    │       └── uart_handler.c
    ├── tcp_server/           # TCP服务器组件
    │   ├── CMakeLists.txt
    │   ├── include/
    │   │   └── tcp_server.h
    │   └── src/
    │       └── tcp_server.c
    └── wifi_link/            # WiFi连接组件
        ├── CMakeLists.txt
        ├── include/
        │   └── wifi_link.h
        └── src/
            └── wifi_link.c
```

### 组件开发规范
1. 组件目录结构
   ```
   component_name/
   ├── CMakeLists.txt    # 组件构建文件
   ├── include/          # 公共头文件目录
   │   └── *.h          # 对外接口头文件
   └── src/             # 源文件目录
       └── *.c          # 实现文件
   ```

2. CMakeLists.txt配置
   ```cmake
   idf_component_register(
       SRCS "src/component_name.c"     # 源文件
       INCLUDE_DIRS "include"          # 头文件目录
       REQUIRES other_component        # 依赖的其他组件
   )
   ```

3. 头文件规范
   ```c
   #ifndef COMPONENT_NAME_H
   #define COMPONENT_NAME_H

   // 包含必要的头文件
   #include <...>

   // 常量和宏定义
   #define CONST_NAME value

   // 函数声明
   /**
    * @brief 函数功能简述
    * @param param 参数说明
    * @return 返回值说明
    */
   return_type function_name(params);

   #endif // COMPONENT_NAME_H
   ```

### 实际案例：TCP服务器项目模块化
1. UART处理模块（uart_handler）
   ```c
   // uart_handler.h
   void uart_init(void);
   int uart_send_data(const char* data, size_t len);
   int uart_receive_data(char* data, size_t max_len);
   ```
   - 负责UART通信初始化
   - 提供数据发送和接收接口
   - 封装了UART配置参数

2. TCP服务器模块（tcp_server）
   ```c
   // tcp_server.h
   void tcp_server_task(void *pvParameters);
   esp_err_t start_tcp_server(void);
   ```
   - 处理TCP服务器功能
   - 管理客户端连接
   - 实现命令解析和处理

3. WiFi连接模块（wifi_link）
   ```c
   // wifi_link.h
   esp_err_t wifi_init_sta(const char *ssid, const char *password);
   bool wifi_is_connected(void);
   esp_err_t wifi_get_ip_string(char *ip_str, size_t max_len);
   ```
   - 管理WiFi连接
   - 提供连接状态查询
   - 处理IP地址获取

4. 主程序精简化
   ```c
   void app_main(void)
   {
       // 初始化NVS
       esp_err_t ret = nvs_flash_init();
       // ...

       // 初始化WiFi
       ESP_ERROR_CHECK(wifi_init_sta(WIFI_SSID, WIFI_PASS));

       // 初始化UART
       uart_init();

       // 启动TCP服务器
       ESP_ERROR_CHECK(start_tcp_server());

       // 主循环
       while (1) {
           // 处理UART数据
           // ...
       }
   }
   ```

### 依赖关系管理
1. 组件间依赖
   - tcp_server依赖uart_handler
   - 所有组件依赖esp_common
   - 主程序依赖所有自定义组件

2. CMake配置示例
   ```cmake
   # uart_handler/CMakeLists.txt
   idf_component_register(
       SRCS "src/uart_handler.c"
       INCLUDE_DIRS "include"
       REQUIRES driver esp_common
   )

   # tcp_server/CMakeLists.txt
   idf_component_register(
       SRCS "src/tcp_server.c"
       INCLUDE_DIRS "include"
       REQUIRES lwip uart_handler freertos esp_common
   )
   ```

### 注意事项
1. 组件设计原则
   - 功能单一性：每个组件只负责一个核心功能
   - 接口简洁性：对外接口要简单清晰
   - 依赖最小化：尽量减少组件间的依赖

2. 常见问题处理
   - 头文件找不到：检查CMakeLists.txt中的INCLUDE_DIRS配置
   - 链接错误：检查REQUIRES中的组件依赖
   - 符号重定义：检查头文件保护宏

3. 开发建议
   - 先设计后实现：先规划好组件接口再开发
   - 渐进式重构：逐步将功能模块化，而不是一次性重构
   - 保持向后兼容：修改接口时考虑兼容性
   - 编写文档：及时更新组件使用说明
