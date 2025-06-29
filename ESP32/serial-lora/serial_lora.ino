#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>

#define RX1PIN 17
#define TX1PIN 18

int count = 0;
SemaphoreHandle_t mutex;  // FreeRTOS互斥量
String serialBuffer; // 全局串口缓存
// 发送任务
void send_handler(void *arg) {
  while (true) {
    // 获取互斥锁（带50ms超时）
    if (xSemaphoreTake(mutex, pdMS_TO_TICKS(50))) {
      Serial1.printf("test %d \r\n", count++);
      xSemaphoreGive(mutex);  // 释放锁
    }
    // 非阻塞延迟1秒
    delay(1000);
  }
}

void setup() {
  Serial.begin(115200);
  Serial1.begin(9600, SERIAL_8N1, RX1PIN, TX1PIN);

  // // 创建互斥锁
  // mutex = xSemaphoreCreateMutex();

  // // 创建发送任务（核心0，优先级1）
  // xTaskCreatePinnedToCore(
  //   send_handler,   // 任务函数
  //   "serial_tx",    // 任务名称
  //   4096,           // 栈大小（字节）
  //   NULL,           // 参数
  //   1,              // 优先级
  //   NULL,           // 任务句柄
  //   0               // 核心0（避免与WiFi/BT冲突）
  // );
}


void loop() {
  //! 接收数据，并响应,发送数据需要添加\r\n
  if (Serial1.available()) {
    String data = Serial1.readStringUntil('\n');
    data.trim(); // 移除\r
    if (data.length() > 0) {
      Serial.print("From Serial: ");
      Serial.println(data);
      Serial1.printf("ok\r\n");
    }
  }
}