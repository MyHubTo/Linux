// c/c++ header
#include <map>
#include <time.h>

//library header
#include <WiFi.h>
#include <IRrecv.h>
#include <IRutils.h>
#include <Arduino.h>
#include <OneButton.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include <SSD1306Wire.h>
#include <IRremoteESP8266.h>
#include <PubSubClient.h>
//for Digit display
#include <TM1637.h>
#define CLK 16  //Set the CLK pin connection to the display
#define DIO 15  //Set the DIO pin connection to the display
TM1637 tm1637(CLK, DIO);
bool g_dot = false;

// local header
#include "cJson.h"
#include "images.h"

#define HI_SUCCESS 0
#define HI_FAILURE -1
#define HI_ON 1
#define HI_OFF 0

WiFiMulti wifiMulti;
HTTPClient http;

#define kRecvPin 10
IRrecv irrecv(kRecvPin);
decode_results results;

hw_timer_t* g_timer = NULL;

const char* ssid_1 = "TP-LINK_5DAB";
const char* password_1 = "gdragon1001.";
const char* ssid_2 = "HORNOR 30";
const char* password_2 = "123654789";
const char* ssid_3 = "TP-LINK_5G_EA90";
const char* password_3 = "********";

const char* MQTT_SERVER  = "***********************";
const char* MQTT_USRNAME = "*********";
const char* MQTT_PASSWD  = "*********";
const char* TOPIC = "esp32-s/led";
const char* CLIENT_ID    = "esp32-mqtt-client";
const int   MQTT_PORT    = 8084;

WiFiClient espClient;
PubSubClient  mqttClient(espClient);
String mqttMessage;
String mqttHost="host";
const char* ntp_server_url = "ntp.aliyun.com";
const char* weather_url = "http://api.seniverse.com/v3/weather/now.json?key=SnMLgNL9x_uRyvyIG&location=xian&language=en&unit=c";
int g_year;
int g_mon;
int g_day;
int g_hour;
int g_min;
int g_sec;

int g_uptime;

String g_weather_text;
String g_weather_temperature;

int g_counter = 0;
int g_mode = 0;
int g_max_mode = 7;
struct tm g_last_time;

SSD1306Wire display(0x3c, 6, 5);
#define PIN_INPUT 0
OneButton button(PIN_INPUT, true);

void click() {
  Serial.println("Button 1 click.");
  g_mode++;
  if (g_mode > g_max_mode) {
    g_mode = 0;
  }
}  // click1

const int g_led=17;
double g_blink_led_freq=-1;
int g_blink_led_state=1;
void ledBlink(){
  if(g_blink_led_state<=0){
    digitalWrite(g_led,LOW);
    return;
  }
  if(g_blink_led_freq<0){
    return;
  }
  digitalWrite(g_led,LOW);
  delay(g_blink_led_freq*1000);
  digitalWrite(g_led,HIGH);
  delay(g_blink_led_freq*1000);
}

// This function will be called when the button1 was pressed 2 times in a short timeframe.
void doubleclick() {
  Serial.println("Button 1 doubleclick.");
  g_mode = 0;
}  // doubleclick1

// This function will be called once, when the button1 is pressed for a long time.
void longPressStart() {
  Serial.println("Button 1 longPress start");
}  // longPressStart1

// This function will be called often, while the button1 is pressed for a long time.
void longPress() {
  Serial.println("Button 1 longPress...");
}  // longPress1

// This function will be called once, when the button1 is released after beeing pressed for a long time.
void longPressStop() {
  Serial.println("Button 1 longPress stop");
}  // longPressStop1

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
  http.begin(weather_url);
  int httpCode = http.GET();
  // httpCode will be negative on error
  if (httpCode > 0) {
    // file found at server
    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      parseWeatherInfo(payload);
    }
  } else {
    Serial.printf("[HTTP] GET... failed, error: %s httpCode: %d\n", http.errorToString(httpCode).c_str(), httpCode);
  }
  http.end();
}

void updateTime(void* pvParameters) {
  while (true) {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
      Serial.println("Failed to obtain time");
    }
    g_year = timeinfo.tm_year + 1900;  //years since 1900
    g_mon = timeinfo.tm_mon + 1;       //months since January - [0, 11]
    g_day = timeinfo.tm_mday;          //day of the month - [1, 31]
    g_hour = timeinfo.tm_hour;
    g_min = timeinfo.tm_min;
    g_sec = timeinfo.tm_sec;
    vTaskDelay(1000 / portTICK_PERIOD_MS);  // wait for one second
    g_uptime++;
    //Serial.println(&timeinfo, "%A, %Y-%m-%d %H:%M:%S");
    if (timeinfo.tm_hour != g_last_time.tm_hour) {
      updateWeatherInfo();
      g_last_time = timeinfo;
    }
  }
  vTaskDelete(NULL);
}

void buttonListener(void* pvParameters) {
  while (true) {
    button.tick();
    delay(10);
  }
}

void drawProgressBarUI() {
  display.setFont(ArialMT_Plain_16);
  display.clear();
  int progress = (g_counter / 5) % 100;
  // draw the progress bar
  display.drawProgressBar(0, 32, 120, 10, progress);

  // draw the percentage as String
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 15, String(progress) + "%");
  display.display();
}

void drawTimeInfoUI() {
  display.clear();
  // draw the percentage as String
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setFont(ArialMT_Plain_16);
  String current_date = String(g_year) + "/" + String(g_mon) + "/" + String(g_day) + "\n";
  String current_time;

  String hour_str = (g_hour < 10) ? "0" + String(g_hour) : String(g_hour);
  String min_str = (g_min < 10) ? "0" + String(g_min) : String(g_min);

  if ((g_sec % 2) == 0) {
    current_time = hour_str + ":" + min_str + "\n";
  } else {
    current_time = hour_str + " " + min_str + "\n";
  }

  display.drawString(64, 15, current_date + current_time);
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

std::map<String, const uint8_t*> mp = { { "Sunny", sunny }, { "Cloudy", cloudy }, { "Overcast", overcast }, { "Rain", rain }, { "Snow", snow }, { "Haze", haze } };

void drawImageUI() {
  display.clear();
  const uint8_t* p = nullptr;
  p = mp[g_weather_text];
  if (p != nullptr) {
    display.drawXbm(0, 12, Logo_width, Logo_height, p);
  } else {
    display.drawXbm(0, 12, Logo_width, Logo_height, default_);
  }
  display.drawString(90, 15, g_weather_text + "\n" + g_weather_temperature + "°C");
  display.display();
}

void drawWiFiInfomationUI() {
  display.clear();
  // draw the percentage as String
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setFont(ArialMT_Plain_16);
  display.drawString(64, 15, "WiFi SSID:\n" + WiFi.SSID() + "\n");
  display.display();
}

void drawBoardInfomationUI() {
  display.clear();
  // draw the percentage as String
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setFont(ArialMT_Plain_16);
  String info = "Clock:" + String(ESP.getCpuFreqMHz()) + "MHz\n" + "Free:" + String(ESP.getFreeHeap() / ((float)ESP.getHeapSize()) * 100.0) + "% \n";
  display.drawString(64, 15, info);
  display.display();
}

void drawUptimeInfoUI() {
  display.clear();
  // draw the percentage as String
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setFont(ArialMT_Plain_16);
  String info = "Uptime:\n" + String(g_uptime) + "s\n";
  display.drawString(64, 15, info);
  display.display();
}

void drawMqttMessage(){
   display.clear();
  // draw the percentage as String
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setFont(ArialMT_Plain_16);
  String info = "Received \n Mqqt Message";
  display.drawString(64, 15, info);
  display.display();
  delay(1000);
  g_mode=0;
  mqttMessage.clear();
}

void updateUI() {
  switch (g_mode) {
    case 0:
      {
        drawTimeInfoUI();
        break;
      }
    case 1:
      {
        drawProgressBarUI();
        break;
      }
    case 2:
      {
        drawWeatherInfoUI();
        break;
      }
    case 3:
      {
        drawImageUI();
        break;
      }
    case 4:
      {
        drawWiFiInfomationUI();
        break;
      }
    case 5:
      {
        drawBoardInfomationUI();
        break;
      }
    case 6:
      {
        drawUptimeInfoUI();
        break;
      }
      case 7:
      {
        drawMqttMessage();
        break;
      }
  }
}
void timerCallbackHandler() {
  tm1637.display(0, g_hour / 10);
  tm1637.display(1, g_hour % 10);
  tm1637.display(2, g_min / 10);
  tm1637.display(3, g_min % 10);
  tm1637.point(!g_dot);
  g_dot = !g_dot;
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  mqttMessage=message;
  handleMqqtMessage(message);
  Serial.println(message);
  g_mode=7;
  Serial.print(message.c_str());
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
    cJSON *freqName =NULL;
    switch(code){
      case 1:
        freqName = cJSON_GetObjectItem(root, "freq");
        if (cJSON_IsNumber(freqName)) {
          double freq=freqName->valuedouble;
          g_blink_led_freq=freq;
          g_blink_led_state=state;
        }
        break;
        default:
        slaveMessage="Execution Failed";
        break;
    }
  }  
  if(mqttClient.connected()){
    mqttClient.publish(mqttHost.c_str(),slaveMessage.c_str());
  }
}
void mqttConnect() {
  int flag=0;
  while (!mqttClient.connected()) {
    Serial.println("Attempting MQTT connection...");
    if (mqttClient.connect(CLIENT_ID,MQTT_USRNAME,MQTT_PASSWD)) {
      Serial.println("Connected to Mqtt");
      mqttClient.subscribe(TOPIC);
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 1 seconds");
      delay(1000);
      flag++;
      Serial.printf(" current flag %d \n",flag);
      // if(flag>10){
      //   Serial.println("Connect to MqttServer error");
      //   break;
      // }
    }
  }
}

void setup() {
  Serial.begin(115200);
  // init led
  pinMode(g_led, OUTPUT);
  digitalWrite(g_led,HIGH);
  // init display
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 15, "Connecting ...");
  display.display();

  // init button
  button.reset();
  button.attachClick(click);
  button.attachDoubleClick(doubleclick);
  button.attachLongPressStart(longPressStart);
  button.attachLongPressStop(longPressStop);
  button.attachDuringLongPress(longPress);

  //init IR
  irrecv.enableIRIn();

  //init wifi
  wifiMulti.addAP(ssid_1, password_1);
  wifiMulti.addAP(ssid_2, password_2);
  wifiMulti.addAP(ssid_3, password_3);
  while (wifiMulti.run() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  display.clear();
  display.drawString(64, 15, String("Connected to ") + "\n" + WiFi.SSID() + "\n");
  display.display();
  delay(1 * 1000);

  // shanghai
  configTime(8 * 3600, 0, ntp_server_url);

  //update weatherinfo
  updateWeatherInfo();

  //uptime ui
  g_uptime = 0;

  //init timer
  g_timer = timerBegin(0, 80, true);
  timerAttachInterrupt(g_timer, timerCallbackHandler, true);
  timerAlarmWrite(g_timer, 1 * 1000000, true);
  timerAlarmEnable(g_timer);

  //init TM1637
  tm1637.init();
  tm1637.point(HI_ON);
  tm1637.set(BRIGHT_TYPICAL);  //BRIGHT_TYPICAL = 2,BRIGHT_DARKEST = 0,BRIGHTEST = 7;

  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  mqttClient.setCallback(mqttCallback);
  mqttConnect();

  //init tasks
  xTaskCreate(updateTime, "updateTime", 1024 * 4, NULL, 1, NULL);
  xTaskCreate(buttonListener, "buttonListener", 1024, NULL, 1, NULL);
}


void loop() {
  if ((wifiMulti.run() == WL_CONNECTED)) {
    updateUI();
    delay(30);
    g_counter++;
  } else {
    setup();
  }
  // if(!mqttClient.connected()){
  //   mqttConnect();
  // }
  // if (irrecv.decode(&results)) {
  //   // print() & println() can't handle printing long longs. (uint64_t)
  //   serialPrintUint64(results.value, HEX);
  //   Serial.println("");
  //   irrecv.resume();  // Receive the next value
  // }
  ledBlink();
}
