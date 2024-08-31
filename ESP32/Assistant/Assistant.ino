#include <WiFi.h>
#include <Ticker.h>
#include <TM1637.h>
#include <Arduino.h>
#include <WiFiUdp.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include <SSD1306Wire.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include "Config.h"
#include "cJSON.h"
#include "images.h"
// c/c++ header
#include <map>
#include <time.h>
//-------------------------------------------- Global param start------------------------------------------------
double      g_blink_led_freq    =-1;
int         g_blink_led_state   = 1;
int         g_ui_index          = 0;


String      g_host_serial       ="SN00000001";
String      g_host_mac;
String      g_host_ip;
int         g_host_connected    =0;
int         g_productID         =0x02;//产品类型
WiFiUDP         Udp;
unsigned  int   UdpPort = 6689; //本地端口号
WiFiServer      Tcp;         //声明TCP服务器对象
unsigned  int   TcpPort = 6688; //本地端口号
// 计时器变量
unsigned long g_timer_previousMillis = 0; // 记录上一次更新的时间
const long g_timer_interval = 1000; // 1秒（1000毫秒）间隔
int g_timer_hours = 0;
int g_timer_minutes = 0;
int g_timer_seconds = 0;
String g_timer_text;

String g_weather_text;
String g_weather_temperature;
std::map<String, const uint8_t*> mp = { { "Sunny", sunny }, { "Cloudy", cloudy }, { "Overcast", overcast }, { "Rain", rain }, { "Snow", snow }, { "Haze", haze } };

HTTPClient http;
Ticker ticker_time;
Ticker ticker_timer;
Ticker ticker_weather;
Ticker ticker_distance;

#ifdef OLED_DISPLAY
SSD1306Wire display(0x3c, 6, 5);
#endif
//--------------------------------------------  Global param end ------------------------------------------------
//------------------------------------------------WIFI start ----------------------------------------------------
WiFiMulti wifiMulti;
void connectToWiFi() {
    WiFi.begin(SSID, PW);
    Serial.println("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println();
    Serial.println("Connected to the WiFi network");
    g_host_mac=String(WiFi.macAddress());
    g_host_ip=String(WiFi.localIP().toString());
#ifdef OLED_DISPLAY
  display.clear();
  display.drawString(64, 15, String("Connected to ") + "\n" + WiFi.SSID() + "\n");
  display.display();
#endif
}
//------------------------------------------------WiFi  end  ----------------------------------------------------

//------------------------------------------------Mqtt start ----------------------------------------------------
WiFiClientSecure espClient;
PubSubClient mqttClient(espClient);
void connectToMQTTBroker() {
    espClient.setCACert(ca_cert);
    mqttClient.setKeepAlive(60);
    while (!mqttClient.connected()) {
        String client_id = String(MQTT_CLIENT_ID)+ "-" + String(WiFi.macAddress());
        Serial.printf("Connecting to MQTT Broker as %s ...\n", client_id.c_str());
        if (mqttClient.connect(client_id.c_str(), MQTT_USRNAME, MQTT_PASSWD)) {
            Serial.println("Connected to MQTT broker");
            mqttClient.subscribe(MQTT_TOPIC);
            // Publish message upon successful connection
            mqttClient.publish(HOST_TOPIC, "Hi EMQX I'm ESP32-s3 ^^");
        } else {
            Serial.print("Failed to connect to MQTT broker, rc=");
            Serial.print(mqttClient.state());
            Serial.println(" try again in 5 seconds");
            delay(5000);
        }
    }
}
void mqttCallback(char *topic, byte *payload, unsigned int length) {
    Serial.print("Message received on topic: ");
    Serial.println(topic);
    Serial.print("Message:");
    String message = "";
    for (unsigned int i = 0; i < length; i++) {
        message += (char)payload[i];
    }
    handleMqqtMessage(message);
    Serial.println(message);
    Serial.println();
}

void handleMqqtMessage(String msg){
  String slaveMessage="Execution Successful";
  cJSON *root= cJSON_Parse(msg.c_str());
  if(root!=NULL){
    cJSON *codeName = cJSON_GetObjectItem(root, "code");
    cJSON *stateName = cJSON_GetObjectItem(root, "state");
    if (!cJSON_IsNumber(codeName)&&!cJSON_IsNumber(stateName)){
      Serial.println("Json parse error");
      return;
    }
    int code=codeName->valueint;
    int state=stateName->valueint;
    switch(code){
      case 1:
        g_blink_led_state=state;
        break;
      case 2:
        mqttClient.publish(HOST_TOPIC,String(g_blink_led_state).c_str());
        break;
      default:
        slaveMessage="Execution Failed";
        break;
    }
  }
  if(mqttClient.connected()){
    mqttClient.publish(HOST_TOPIC,slaveMessage.c_str());
  }
}

void keepAlive(){
  if (!mqttClient.connected()) {
    Serial.println("Disconnect from MQTT broken");
    connectToMQTTBroker();
  }
  if(WiFi.status() != WL_CONNECTED){
    Serial.println("Disconnect from WiFi");
    connectToWiFi();
  }
}

//------------------------------------------------Mqtt end ----------------------------------------------------

//------------------------------------------------Time start --------------------------------------------------
TM1637 tm1637(CLK, DIO);
void initTime(){
  configTime(8 * 3600, 0, NTP_SERVER_URL);
}

void updateTimeInfo(){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    if (!getLocalTime(&timeinfo)) {
      Serial.println("Failed to obtain time");
    }
  }
  tm1637.display(0, timeinfo.tm_hour / 10);
  tm1637.display(1, timeinfo.tm_hour % 10);
  tm1637.display(2, timeinfo.tm_min / 10);
  tm1637.display(3, timeinfo.tm_min % 10);
  if((timeinfo.tm_sec % 2)==0){
    tm1637.point(false);
  }else{
    tm1637.point(true);
  }
}
//------------------------------------------------Time end  ---------------------------------------------------
//------------------------------------------------Loops function-----------------------------------------------
int parseWeatherInfo(String info) {
  String msg = "Parse failure";
  cJSON* root = cJSON_Parse(info.c_str());
  do {
    if (!root) {
      break;
    }
    cJSON* results = cJSON_GetObjectItem(root, "results");
    if (!results) {
      break;
    }
    cJSON* sub = cJSON_GetArrayItem(results, 0);
    if (!sub) {
      break;
    }
    //Serial.print(cJSON_GetObjectItem(sub,"last_update")->valuestring);
    cJSON* now = cJSON_GetObjectItem(sub, "now");
    if (!now) {
      break;
    }
    char* text = cJSON_GetObjectItem(now, "text")->valuestring;
    char* temperature = cJSON_GetObjectItem(now, "temperature")->valuestring;
    g_weather_text = String(text);
    g_weather_temperature = String(temperature);
    Serial.println("text:" + g_weather_text);
    Serial.println("temperature:" + g_weather_temperature);
    return HI_SUCCESS;
  } while (0);
  Serial.println(msg.c_str());
  return HI_FAILURE;
}
void updateWeatherInfo() {
  http.begin("http://api.seniverse.com/v3/weather/now.json?key=SnMLgNL9x_uRyvyIG&location=xian&language=en&unit=c");
  int httpCode = http.GET();
  // httpCode will be negative on error
  if (httpCode > 0) {
    // file found at server
    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      parseWeatherInfo(payload);
      Serial.println(payload);
    }
  } else {
    Serial.printf("[HTTP] GET... failed, error: %s httpCode: %d\n", http.errorToString(httpCode).c_str(), httpCode);
  }
  http.end();
}

void updateTimerInfo() {
    unsigned long currentMillis = millis();

    if (currentMillis -  g_timer_previousMillis >= g_timer_interval) {
         g_timer_previousMillis = currentMillis;

        // Update timer
        g_timer_seconds++;
        if (g_timer_seconds >= 60) {
            g_timer_seconds = 0;
            g_timer_minutes++;
            if (g_timer_minutes >= 60) {
                g_timer_minutes = 0;
                g_timer_hours++;
                if (g_timer_hours >= 24) {
                    g_timer_hours = 0;
                }
            }
        }
    }

    // Construct formatted time string
    char timeString[9]; // "HH:MM:SS\0" 8 characters + 1 null terminator
    snprintf(timeString, sizeof(timeString), "%02d:%02d:%02d", g_timer_hours, g_timer_minutes, g_timer_seconds);
    g_timer_text = String(timeString);
    // Serial.println("g_timer_text: " + g_timer_text);
}

void Breathinglamp() {
  // 逐渐变亮
  for (int dutyCycle = 0; dutyCycle <= 255; dutyCycle = dutyCycle + 5) {
    ledcWrite(0, dutyCycle);
    delay(30);
  }
  // 逐渐变暗
  for (int dutyCycle = 255; dutyCycle >= 0; dutyCycle = dutyCycle - 5) {
    ledcWrite(0, dutyCycle);
    delay(30);
  }
}
//https://blog.csdn.net/Naisu_kun/article/details/86764221
void initTimingTasks(){
  ticker_distance.attach(0.2,updateDistanceInfo);
  ticker_time.attach(0.5, updateTimeInfo);          //每0.5秒调用updateTimeCallBack
  ticker_timer.attach(1,updateTimerInfo);
  ticker_weather.attach(60*30,updateWeatherInfo);   //30 min
}
void resetTimer(){
  g_timer_hours=0;
  g_timer_minutes=0;
  g_timer_seconds=0;
}
void updateDistanceInfo(){
  digitalWrite(TRIG, LOW);
	delayMicroseconds(2);
	digitalWrite(TRIG, HIGH);
	delayMicroseconds(10);
	digitalWrite(TRIG, LOW);
	noInterrupts();
	float d = pulseIn(ECHO, HIGH, 23529.4); // max sensor dist ~4m
	interrupts();
	long distance= d / 58.8235;
  // Serial.printf("%d cm \n",distance);

  if(distance>20){
    digitalWrite(BLINK_LED, LOW);
    g_ui_index=0;
    resetTimer();
  }else{
    g_ui_index=1;
    digitalWrite(BLINK_LED, HIGH);
  }
}
//------------------------------------------------Loops function-----------------------------------------------
void drawBoardInfomationUI() {
  display.clear();
  // draw the percentage as String
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setFont(ArialMT_Plain_16);
  String info = "Clock:" + String(ESP.getCpuFreqMHz()) + "MHz\n" + "Free:" + String(ESP.getFreeHeap() / ((float)ESP.getHeapSize()) * 100.0) + "% \n";
  display.drawString(64, 15, info);
  display.display();
}

void drawImageUI() {
  display.clear();
  display.setFont(ArialMT_Plain_16);
  const uint8_t* p = nullptr;
  p = mp[g_weather_text];
  if (p != nullptr) {
    display.drawXbm(0, 12, LOGO_W, LOGO_H, p);
  } else {
    display.drawXbm(0, 12, LOGO_W, LOGO_H, default_);
  }
  display.drawString(90, 15, g_weather_text + "\n" + g_weather_temperature + "°C");
  display.display();
}
void drawWeatherInfoUI() {
  display.clear();
  // draw the percentage as String
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setFont(ArialMT_Plain_16);
  display.drawString(64, 15, "text:" + g_weather_text + "\n" + "temperature:" + g_weather_temperature + "°C");
  display.display();
}

void updateTimerUI(){
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setFont(ArialMT_Plain_16);
  display.drawString(64, 15, g_timer_text);
  display.drawProgressBar(0, 32, 120, 10, (g_timer_seconds*1.0*5)/3);
  display.display();
}

void updateDisplay(void* pvParameters) {
  (void) pvParameters;
  for(;;){
    switch(g_ui_index){
      case 0:
        drawImageUI();
        break;
      case 1:
        updateTimerUI();
        break;
      case 2:
        drawBoardInfomationUI();
        break;
    }
    delay(30);
  }
}

//--------------------------------------------------Loops main----------------------------------------------------

void setup() {
  Serial.begin(SERIAL_BAUD_RATE);
  pinMode(TRIG, OUTPUT); 
  pinMode(ECHO, INPUT);
  pinMode(BLINK_LED, OUTPUT); 
#ifdef OLED_DISPLAY
  // init display
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 15, "Connecting ...");
  display.display();
#endif
  //init wifi
  connectToWiFi();

  //init Mqtt
  mqttClient.setServer(MQTT_BROKER, MQTT_PORT);
  mqttClient.setCallback(mqttCallback);
  connectToMQTTBroker();

  //init time
  initTime();

  //init TM1637
  tm1637.init();
  tm1637.point(true);
  tm1637.set(BRIGHT_TYPICAL);  //BRIGHT_TYPICAL = 2,BRIGHT_DARKEST = 0,BRIGHTEST = 7;

  //init weatherinfo
  updateWeatherInfo();

  initTimingTasks();

  Udp.begin(UdpPort);
  Tcp.begin(TcpPort);
  // updateDisplay
  xTaskCreate(updateDisplay, "updateDisplay", 1024 * 2, NULL, 1, NULL);

}
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
    client.stop();  //结束当前连接
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
    cJSON_AddStringToObject(root, "IP",g_host_ip.c_str());   // IP为空字符串
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

void loop() {
  keepAlive();
  mqttClient.loop();
  HandlerUdpMessage();
  TcpServer();
}

