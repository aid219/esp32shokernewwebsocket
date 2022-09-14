#include <ArduinoWebsockets.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include "rom/rtc.h"
#include "EEPROM.h"
#include <ArduinoJson.h>
using namespace websockets;
WebsocketsClient client;
HTTPClient http;
#define EEPROM_SIZE 64
DynamicJsonDocument outcome(500); // Json для отправки
DynamicJsonDocument outcome2(500);  
DynamicJsonDocument income(500); // Json входящий
char jsonBuffer[500];
String uuid = "n";
const char* ssid = "Q-my"; // SSID WIFI сети
const char* password = "gH5+Kl20"; // пароль WIFI сети
const char* websockets_server_host = "192.168.1.100"; //Enter server adress
const uint16_t websockets_server_port = 8797; // Enter server port
String websockets_path = "/api/devices/ws";
String serverName = "http://192.168.1.100:8797/api/devices/register";
String serverName2 = "http://192.168.1.100:8797/api/devices/event/";//офисный для теста цеха
// const char* websockets_server_host = "192.168.1.100"; //Enter server adress
// const uint16_t websockets_server_port = 8822; // Enter server port
// String websockets_path = "/api/device/ws";
// String serverName = "http://192.168.1.100:8822/api/device/register";
// String serverName2 = "http://192.168.1.100:8822/api/device/event/huidi";//офисный - исследовательский
long preTime2 = 0;

String print_reset_reason(int reason)
{
  switch ( reason)
  {
    case 1 : return "POWERON_RESET";break;          /**<1,  Vbat power on reset*/
    case 3 : return "SW_RESET";break;               /**<3,  Software reset digital core*/
    case 4 : return "OWDT_RESET";break;             /**<4,  Legacy watch dog reset digital core*/
    case 5 : return "DEEPSLEEP_RESET";break;        /**<5,  Deep Sleep reset digital core*/
    case 6 : return "SDIO_RESET";break;             /**<6,  Reset by SLC module, reset digital core*/
    case 7 : return "TG0WDT_SYS_RESET";break;       /**<7,  Timer Group0 Watch dog reset digital core*/
    case 8 : return "TG1WDT_SYS_RESET";break;       /**<8,  Timer Group1 Watch dog reset digital core*/
    case 9 : return "RTCWDT_SYS_RESET";break;       /**<9,  RTC Watch dog Reset digital core*/
    case 10 : return "INTRUSION_RESET";break;       /**<10, Instrusion tested to reset CPU*/
    case 11 : return "TGWDT_CPU_RESET";break;       /**<11, Time Group reset CPU*/
    case 12 : return "SW_CPU_RESET";break;          /**<12, Software reset CPU*/
    case 13 : return "RTCWDT_CPU_RESET";break;      /**<13, RTC Watch dog Reset CPU*/
    case 14 : return "EXT_CPU_RESET";break;         /**<14, for APP CPU, reseted by PRO CPU*/
    case 15 : return "RTCWDT_BROWN_OUT_RESET";break;/**<15, Reset when the vdd voltage is not stable*/
    case 16 : return "RTCWDT_RTC_RESET";break;      /**<16, RTC Watch dog reset digital core and rtc module*/
    default : return "NO_MEAN";
  }
}






void onEventsCallback(WebsocketsEvent event, String data) {
    if(event == WebsocketsEvent::ConnectionOpened) {
        Serial.println("Connnection Opened");
        lostConnection = false;
    } else if(event == WebsocketsEvent::ConnectionClosed) {
        Serial.println("Connnection Closed");
        lostConnection = true;
    } else if(event == WebsocketsEvent::GotPing) {
        Serial.println("Got a Ping!");
    } else if(event == WebsocketsEvent::GotPong) {
        Serial.println("Got a Pong!");
    }
}
void serializeData() // херачим наш Json в удобный для пересылки вид
{
  outcome["mode"] = shokerMode;
  outcome["stage"] = progLen;
  outcome["temps"][0] = temperature1;
  outcome["temps"][1] = temperature2;
  outcome["temps"][2] = temperature3;
  outcome["temps"][3] = temperature4;
  outcome["temps"][4] = temperature5;
  outcome["temps"][5] = temperature6;
  outcome["time"] = globalTime;
  outcome2["status"] = "close";
  outcome2["openned_time"] = 0;
  outcome["door"] = outcome2;
  serializeJsonPretty(outcome, jsonBuffer);
  outcome.clear();
  outcome2.clear();
}
void deserializeData(String inData) // разбираем прилетевший Json
{
  deserializeJson(income, inData); // десериализация, но уже не такая симпотичная как сериализация
  
  String indata;
  if(uuid.length() < 5){
    String ind = income["device_id"];
    indata = ind;
    uuid = ind;
  }else{
    progLen = income["program"].size();
    stageTime = income["program"][0]["time"];
    String inc = income["program"][0]["mode"];
    shokerMode = inc;
    preGlobalTime = millis();
    preTime = 0;

  }  
     
}
void getUuid(){
  int httpResponseCode = http.POST("");
  if (httpResponseCode>0) {
    String payload = http.getString();
    Serial.println(payload);
    deserializeData(payload);
    websockets_path = String("/api/shokers/ws/") + uuid;
  }
    else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    http.end();
}

void sendRestart(){

  int httpResponseCode = http.POST(print_reset_reason(rtc_get_reset_reason(0)));
  if (httpResponseCode==200) {
    Serial.println("zaebis");
  }
    else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    http.end();
}

void Wifi_disconnected(WiFiEvent_t event, WiFiEventInfo_t info){
  Serial.println("Disconnected from WIFI access point");
  Serial.print("WiFi lost connection. Reason: ");
  Serial.println(info.disconnected.reason);
  Serial.println("Reconnecting...");
  WiFi.begin(ssid, password);
}
void memoryWrite(String inDat){
  EEPROM.write(5, '9');
  int adr = 6;
  for (int i = 0; i < 36; ++i) {
        EEPROM.write(adr, inDat[i]);
        adr += 1;
    }
  EEPROM.commit();
}
bool checkMemorySet(){
  
  byte readValue = EEPROM.read(5);
  char readValueChar = char(readValue);
  Serial.println(readValueChar);
  if(readValueChar == '9'){
    return true;
  }else{
    return false;
  }
} 

void memoryClear(){
  int adr = 5;
  for (int i = 0; i < 37; ++i) {
        EEPROM.write(adr, 0);
        adr += 1;
    }
  EEPROM.commit();
}

String memoryRead(){
  int adr = 6;
  String ui = "";
  for (int i = 0; i < 36; i++) {
        byte readValue = EEPROM.read(adr);
        char readValueChar = char(readValue);
        ui = ui + readValueChar;
        ++adr;
    }
  return ui;
}
void sendData(){
    if(preTime2 == 0){
        preTime2 = millis();
    }
    if((millis() - preTime2) < 2000){}
    else{
        getAllTemps();
        Serial.println(shokerMode);
        serializeData();
        client.send(jsonBuffer);
        // Serial.println(progLen);
        // Serial.println(currentProgram);
        // Serial.println(stageTime);
        // Serial.print(temperature3);
        // Serial.print("-");
        // Serial.print(temperature4);
        // Serial.print("-");
        // Serial.print(temperature5);
        // Serial.print("-");
        // Serial.println(temperature6);
        preTime2 = 0;
    }
}
void onMessageCallback(WebsocketsMessage message) {
    Serial.print("Got Message: ");
    Serial.println(message.data());
    deserializeData(message.data());
}

void beginCycle(){
    Serial.begin(115200);
    Serial.println("reset reason is");
    Serial.println(print_reset_reason(rtc_get_reset_reason(0)));
    preTime = millis();
    EEPROM.begin(EEPROM_SIZE);
    String readMem = memoryRead();
    WiFi.begin(ssid, password);
    WiFi.onEvent(Wifi_disconnected, SYSTEM_EVENT_STA_DISCONNECTED); 
    while (WiFi.status() != WL_CONNECTED) // пока не подключется к wifi будет точки писать в порт
    {
        delay(500);
        Serial.println("...");
    }
    Serial.println();
    Serial.print("ip adress is: ");
    Serial.println(WiFi.localIP());
    http.begin(serverName.c_str());
    if (!checkMemorySet()){
    while(uuid.length() < 5){
      Serial.println("kokoko");
      getUuid();
      Serial.println("getting uuid...");
      delay(5000);
      Serial.println(websockets_path);
    }
        memoryWrite(uuid); 
    }else{
        uuid = readMem;
        websockets_path = String("/api/devices/ws/") + uuid;
    }
    http.begin((serverName2 + uuid).c_str());
    sendRestart();
      
    client.onMessage(onMessageCallback);
    // run callback when events are occuring
    client.onEvent(onEventsCallback);
    // Connect to server
    client.connect(websockets_server_host, websockets_server_port, websockets_path);
    client.ping();
}

