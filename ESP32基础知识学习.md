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
