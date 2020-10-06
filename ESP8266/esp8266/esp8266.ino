#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
//#include <PubSubClient.h>
#include <SocketIOClient.h>
#include <SerialCommand.h>  

//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager

//// Update these with values suitable for your network.
//const char* mqtt_server = "io.adafruit.com";
//
//WiFiClient espClient;
//PubSubClient clientMQTT(espClient);
 
//include thư viện để kiểm tra free RAM trên con esp8266
extern "C" {
  #include "user_interface.h"
}
 
 
const byte RX = 2;
const byte TX = 0;
 
SoftwareSerial mySerial(RX, TX, false, 256); 
SerialCommand sCmd(mySerial); // Khai báo biến sử dụng thư viện Serial Command
 
SocketIOClient client;

//char host[] = "gianphoidoonline.herokuapp.com";  //Địa chỉ IP dịch vụ, hãy thay đổi nó theo địa chỉ IP Socket server của bạn.
char host[] = "192.168.43.234";  //Địa chỉ IP dịch vụ, hãy thay đổi nó theo địa chỉ IP Socket server của bạn.
//int port = 80;                  //Cổng dịch vụ socket server do chúng ta tạo!
int port = 3000;                  //Cổng dịch vụ socket server do chúng ta tạo!
char namespace_esp8266[] = "esp8266";   //Thêm Arduino!
 
//từ khóa extern: dùng để #include các biến toàn cục ở một số thư viện khác. Trong thư viện SocketIOClient có hai biến toàn cục
// mà chúng ta cần quan tâm đó là
// RID: Tên hàm (tên sự kiện
// Rfull: Danh sách biến (được đóng gói lại là chuối JSON)
extern String RID;
extern String Rfull;

//String content = "";

//void callback(char* topic, byte* payload, unsigned int length) {
//  for (int i = 0; i < length; i++) {
//    content += ((char)payload[i]);
//  }
//  if (content == "layDoVo") {
//    Serial.println("LOW");
//  
//    StaticJsonBuffer<200> jsonBuffer;
//    JsonObject& root = jsonBuffer.createObject();
//    //đọc giá trị cảm biến rồi in ra root
//    root["dieukhienDC"] = "thuVao";
//  
//    //Gửi đi hoy!
//    //in ra cổng software serial để ESP8266 nhận
//    mySerial.print("svdkDC");   //gửi tên lệnh
//    mySerial.print('\r');           // gửi \r
//    root.printTo(mySerial);        //gửi chuỗi JSON
//    mySerial.print('\r');
//  
//  //    //in ra serial cho Arduino
//  //    mySerial.print(RID);
//  //    mySerial.print('\r');
//  //    mySerial.print(Rfull);
//  //    mySerial.print('\r');
//  
//    //in ra serial monitor
//    Serial.print("svdkDC");
//    Serial.print(' ');
//    Serial.println(root.printTo(mySerial));        //gửi chuỗi JSON);
//    
//    //Kiểm tra xem còn dư bao nhiêu RAM, để debug
//    uint32_t free = system_get_free_heap_size();
//    Serial.println(free);
//  }
//  if (content == "layDoRa") {
//    Serial.println("HIGH");
//    
//    StaticJsonBuffer<200> jsonBuffer;
//    JsonObject& root = jsonBuffer.createObject();
//    //đọc giá trị cảm biến rồi in ra root
//    root["dieukhienDC"] = "dayRa";
//  
//    //Gửi đi hoy!
//    //in ra cổng software serial để ESP8266 nhận
//    mySerial.print("svdkDC");   //gửi tên lệnh
//    mySerial.print('\r');           // gửi \r
//    root.printTo(mySerial);        //gửi chuỗi JSON
//    mySerial.print('\r');
//  
//    //in ra serial monitor
//    Serial.print("svdkDC");
//    Serial.print(' ');
//    Serial.println(root.printTo(mySerial));        //gửi chuỗi JSON);
//    
//    //Kiểm tra xem còn dư bao nhiêu RAM, để debug
//    uint32_t free = system_get_free_heap_size();
//    Serial.println(free);
//  }
//
//}
//
//void reconnect() {
//  // Loop until we're reconnected
//  while (!clientMQTT.connected()) {
//    Serial.print("Attempting MQTT connection...");
//    // Create a random client ID
//    String clientId = "ESP8266Client-";
//    clientId += String(random(0xffff), HEX);
//    // Attempt to connect
//    if (clientMQTT.connect(clientId.c_str(), "hungle783", "afd1b61f24aa4e7c8dbf0f9fa0964783")) {
//      Serial.println("connected");
//      // ... and resubscribe
//      clientMQTT.subscribe("hungle783/feeds/onoff");
//    } else {
//      Serial.print("failed, rc=");
//      Serial.print(clientMQTT.state());
//      Serial.println(" try again in 5 seconds");
//      // Wait 5 seconds before retrying
//      delay(5000);
//    }
//  }
//}
 
void setup()
{
    //Bật baudrate ở mức 57600 để giao tiếp với máy tính qua Serial
    Serial.begin(57600);
    mySerial.begin(57600); //Bật software serial để giao tiếp với Arduino, nhớ để baudrate trùng với software serial trên mạch arduino
    delay(10);

//    clientMQTT.setServer(mqtt_server, 1883);
//    clientMQTT.setCallback(callback);

    WiFiManager wifiManager;
    wifiManager.autoConnect("AutoConnectAP");
    Serial.println("connected...yeey :)");
 
    if (!client.connect(host, port)) {
        Serial.println(F("Ket noi den socket server that bai!"));
        return;
    }
 
    sCmd.addDefaultHandler(defaultCommand); //Lệnh nào đi qua nó cũng bắt hết, rồi chuyển xuống hàm defaultCommand!
    Serial.println("Da san sang nhan lenh");
    
}
 
void loop()
{
//  if (!clientMQTT.connected()) {
//    reconnect();
//  }
//  clientMQTT.loop();
 
    //Khi bắt được bất kỳ sự kiện nào thì chúng ta có hai tham số:
    //  +RID: Tên sự kiện
    //  +RFull: Danh sách tham số được nén thành chuỗi JSON!
    if (client.monitor()) {
      
        //in ra serial cho Arduino
        mySerial.print(RID);
        mySerial.print('\r');
        mySerial.print(Rfull);
        mySerial.print('\r');
 
        //in ra serial monitor
        Serial.print(RID);
        Serial.print(' ');
        Serial.println(Rfull);
        
        //Kiểm tra xem còn dư bao nhiêu RAM, để debug
        uint32_t free = system_get_free_heap_size();
        Serial.println(free);
    }
 
    //Kết nối lại!
    if (!client.connected()) {
      client.reconnect(host, port);
    }
 
    sCmd.readSerial();
}
 
void defaultCommand(String command) {
  char *json = sCmd.next();
  client.send(command, (String) json);//gửi dữ liệu về cho Socket Server
 
  //In ra serial monitor để debug
  Serial.print(command);
  Serial.print(' ');
  Serial.println(json);
//  Serial.println("Tot lam, da gui du lieu roi, xem trong console cua Socket server di");
}
