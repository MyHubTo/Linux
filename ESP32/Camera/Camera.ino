#include "esp_camera.h"
#include "cJSON.h"
#include <WiFi.h>
#include <WiFiClient.h>

// Wi-Fi 配置信息
const char* ssid = "TP-LINK_5DAB";
const char* password = "gdragon1001.";

String      g_host_serial       ="SN00000002";
String      g_host_mac;
String      g_host_ip;
int         g_host_connected    =0;
int         g_productID         =0x01;//产品类型


WiFiUDP         Udp;
unsigned  int   UdpPort = 6689; //本地端口号
WiFiServer      Tcp;         //声明TCP服务器对象
unsigned  int   TcpPort = 6688; //本地端口号
void startCameraServer();
// 摄像头模块的引脚定义
#define CAM_PIN_PWDN    -1 // power down is not used
#define CAM_PIN_RESET   -1 // software reset will be performed
#define CAM_PIN_XCLK    21
#define CAM_PIN_SIOD    26 // SDA
#define CAM_PIN_SIOC    27 // SCL

#define CAM_PIN_D7      35
#define CAM_PIN_D6      34
#define CAM_PIN_D5      39
#define CAM_PIN_D4      36
#define CAM_PIN_D3      19
#define CAM_PIN_D2      18
#define CAM_PIN_D1       5
#define CAM_PIN_D0       4
#define CAM_PIN_VSYNC   25
#define CAM_PIN_HREF    23
#define CAM_PIN_PCLK    22

WiFiServer server(80);
void TcpServer() {
  WiFiClient client = Tcp.available();  //尝试建立客户对象
  if (client)                              //如果当前客户可用
  {
    g_host_connected=1;
    Serial.println("[Client connected]");
    String readBuff;
    while (client.connected())  //如果客户端处于连接状态
    {
      String readBuff = "";
      if (client.available())  //如果有可读数据
      {
        String data = client.readStringUntil('\n');
        Serial.print("Received: ");
        Serial.println(data);
        break;
      }
    }
    g_host_connected=0;
    client.stop();  //结束当前连接:
    Serial.println("[Client disconnected]");
  }
}

void HandlerUdpMessage(){
  int size = Udp.parsePacket(); //获取当前队首数据包长度
  if (size)                     //如果有数据可用
  {
    char buf[size]={0};
    memset(buf,0,sizeof(buf));
    Udp.read(buf, size);        //读取当前包数据
    Serial.println("Received:");
    Serial.println(buf);
    Serial.print("From IP: ");
    Serial.println(Udp.remoteIP());
    Serial.print("From Port: ");
    Serial.println(Udp.remotePort());
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort()); //准备发送数据
    // Udp.print("Received: ");                          //复制数据到发送缓存
    // Udp.write((const uint8_t*)buf, size);             //复制数据到发送缓存

    cJSON* root = cJSON_CreateObject();
    // 向对象中添加键值对
    cJSON_AddStringToObject(root, "SN", g_host_serial.c_str());    // SN为空字符串
    cJSON_AddStringToObject(root, "Mac",g_host_mac.c_str());   // Mac为空字符串
    cJSON_AddStringToObject(root, "IP",g_host_ip.c_str());   // Mac为空字符串
    cJSON_AddNumberToObject(root, "PID",g_productID);   // ProductID为空字符串
    cJSON_AddBoolToObject(root, "Connected", g_host_connected); // Connected为false，在cJSON中，false用0表示  

    // 将JSON对象转换成字符串并打印
    char* rendered = cJSON_Print(root);
    printf("%s\n", rendered);
    Udp.write((const uint8_t*)rendered, strlen(rendered));

    Udp.endPacket();                          //发送数据
    // 释放cJSON对象占用的内存
    cJSON_Delete(root);
    free(rendered);
  }
}
void setup() {
  Serial.begin(115200);

  // 配置摄像头
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = CAM_PIN_D0;
  config.pin_d1 = CAM_PIN_D1;
  config.pin_d2 = CAM_PIN_D2;
  config.pin_d3 = CAM_PIN_D3;
  config.pin_d4 = CAM_PIN_D4;
  config.pin_d5 = CAM_PIN_D5;
  config.pin_d6 = CAM_PIN_D6;
  config.pin_d7 = CAM_PIN_D7;
  config.pin_xclk = CAM_PIN_XCLK;
  config.pin_pclk = CAM_PIN_PCLK;
  config.pin_vsync = CAM_PIN_VSYNC;
  config.pin_href = CAM_PIN_HREF;
  config.pin_sscb_sda = CAM_PIN_SIOD;
  config.pin_sscb_scl = CAM_PIN_SIOC;
  config.pin_pwdn = CAM_PIN_PWDN;
  config.pin_reset = CAM_PIN_RESET;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size = FRAMESIZE_VGA;
  config.jpeg_quality = 12;
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.fb_count = 1;
  // if PSRAM IC present, init with UXGA resolution and higher JPEG quality
  // for larger pre-allocated frame buffer.
  if(psramFound()){
    Serial.printf("psramFound\n");
    config.jpeg_quality = 10;
    config.fb_count = 2;
    config.grab_mode = CAMERA_GRAB_LATEST;
  } else {
    // Limit the frame size when PSRAM is not available
    config.frame_size = FRAMESIZE_SVGA;
    config.fb_location = CAMERA_FB_IN_DRAM;
  }

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }
  Serial.printf("Camera init success\n");
  sensor_t * s = esp_camera_sensor_get();
  // initial sensors are flipped vertically and colors are a bit saturated
  s->set_vflip(s, 1); // flip it back
  s->set_brightness(s, 1); // up the brightness just a bit
  s->set_saturation(s, 0); // lower the saturation

  WiFi.begin(ssid, password);
  WiFi.setSleep(false);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  g_host_mac=String(WiFi.macAddress());
  g_host_ip=String(WiFi.localIP().toString());
  startCameraServer();

  Udp.begin(UdpPort);
  Tcp.begin(TcpPort);

  Serial.print("Camera Ready! Use 'http://");
  Serial.print(WiFi.localIP());
  Serial.println("' to connect");
}

void loop() {
  HandlerUdpMessage();
  TcpServer();
}
