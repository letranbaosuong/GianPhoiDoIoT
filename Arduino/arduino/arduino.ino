#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include <SerialCommand.h>  // Thêm vào sketch thư viện Serial Command
#include "DHT.h"

const byte RX = 13;          // Chân 3 được dùng làm chân RX
const byte TX = 12;          // Chân 2 được dùng làm chân TX
#define DHTPIN 10     // what digital pin we're connected to
#define DHTTYPE DHT11   // DHT 11
DHT dht(DHTPIN, DHTTYPE);
 
SoftwareSerial mySerial = SoftwareSerial(RX, TX); 
 
SerialCommand sCmd(mySerial); // Khai báo biến sử dụng thư viện Serial Command
 
// motor
#define IN1 7
#define IN2 6
#define MAX_SPEED 255 //từ 0-255
#define MIN_SPEED 0

int rainSensor = 3; // Chân tín hiệu cảm biến mưa ở chân digital 6 (arduino)
int congTacHanhTrinh1 = 9;
int congTacHanhTrinh2 = 8;
int valueCongTac2;
int valueCongTac1;

const unsigned long CHU_KY_1_LA_BAO_NHIEU = 200UL; //Cứ sau 2000ms = 2s thì chu kỳ lặp lại
unsigned long chuky1 = 0;
int kt_lenh_dkDC = -1;
boolean lenh_allowDKDC = false;
String chuoi_lenh_dkDC;
int valueRainSensor = digitalRead(rainSensor);

String inputString = "";         // a String to hold incoming data
bool stringComplete = false;  // whether the string is complete

void setup() {
  //Khởi tạo Serial ở baudrate 57600 để debug ở serial monitor
  Serial.begin(57600);
 
  //Khởi tạo Serial ở baudrate 57600 cho cổng Serial thứ hai, dùng cho việc kết nối với ESP8266
  mySerial.begin(57600);

  // cam bien mua, cong tac hanh trinh
  pinMode(rainSensor, INPUT);// Đặt chân cảm biến mưa là INPUT, vì tín hiệu sẽ được truyền đến cho Arduino
  pinMode(congTacHanhTrinh1, INPUT);// Đặt chân công tắc hành trình 1
  pinMode(congTacHanhTrinh2, INPUT);// Đặt chân công tắc hành trình 2
  digitalWrite(congTacHanhTrinh1, HIGH);
  digitalWrite(congTacHanhTrinh2, HIGH);
  // diver
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

  dht.begin();
  
  // Một số hàm trong thư viện Serial Command
  sCmd.addCommand("svdkDC", DKDC); //Khi có lệnh server_gui_dieuKhienDC thì sẽ thực thi hàm DKDC  
  sCmd.addCommand("svgCT", allowDKDC);
  
  Serial.println("da khoi dong xong");

  // reserve 200 bytes for the inputString:
  inputString.reserve(200);
}

void motor_1_Dung() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
}

void motor_1_Tien(int speed) { //speed: từ 0 - MAX_SPEED
  speed = constrain(speed, MIN_SPEED, MAX_SPEED);//đảm báo giá trị nằm trong một khoảng từ 0 - MAX_SPEED - http://arduino.vn/reference/constrain
  digitalWrite(IN1, HIGH);// chân này không có PWM
  analogWrite(IN2, 255 - speed);
}
 
void motor_1_Lui(int speed) {
  speed = constrain(speed, MIN_SPEED, MAX_SPEED);//đảm báo giá trị nằm trong một khoảng từ 0 - MAX_SPEED - http://arduino.vn/reference/constrain
  digitalWrite(IN1, LOW);// chân này không có PWM
  analogWrite(IN2, speed);
}

void GuiTrangThaiDC(int valueRainSensor, int valueCongTac1, int valueCongTac2) {
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  //đọc giá trị cảm biến rồi in ra root
  root["valueRainSensor"] = valueRainSensor;
  root["valueCongTac1"] = valueCongTac1;
  root["valueCongTac2"] = valueCongTac2;
  root["voice"] = 0;

  //Gửi đi hoy!
  //in ra cổng software serial để ESP8266 nhận
  mySerial.print("STATUSDC");   //gửi tên lệnh
  sCmd.readSerial();
  mySerial.print('\r');           // gửi \r
  sCmd.readSerial();
  root.printTo(mySerial);        //gửi chuỗi JSON
  sCmd.readSerial();
  mySerial.print('\r');
  sCmd.readSerial();
}

void GuiTrangThaiDCVoice(int valueRainSensor, int valueCongTac1, int valueCongTac2) {
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  //đọc giá trị cảm biến rồi in ra root
  root["valueRainSensor"] = valueRainSensor;
  root["valueCongTac1"] = valueCongTac1;
  root["valueCongTac2"] = valueCongTac2;
  root["voice"] = 1;

//  if (millis() - chuky1 > CHU_KY_1_LA_BAO_NHIEU) {
//    chuky1 = millis();

    //Gửi đi hoy!
    //in ra cổng software serial để ESP8266 nhận
    mySerial.print("STATUSDC");   //gửi tên lệnh
    sCmd.readSerial();
    mySerial.print('\r');           // gửi \r
    sCmd.readSerial();
    root.printTo(mySerial);        //gửi chuỗi JSON
    sCmd.readSerial();
    mySerial.print('\r');
    sCmd.readSerial();
//  }
  sCmd.readSerial();
}

void _GuiTrangThaiDC(){
  sCmd.readSerial();
  valueRainSensor = digitalRead(rainSensor);
  valueCongTac1 = digitalRead(congTacHanhTrinh1);
  valueCongTac2 = digitalRead(congTacHanhTrinh2);
  GuiTrangThaiDC(valueRainSensor, valueCongTac1, valueCongTac2);
}

void HeThongTuDong(){
  sCmd.readSerial();
  valueRainSensor = digitalRead(rainSensor);//Đọc tín hiệu cảm biến mưa
  if (valueRainSensor == HIGH) { // Cảm biến đang không mưa
    Serial.println("Dang khong mua");
    motor_1_Tien(MAX_SPEED); // motor 1 tiến
    valueCongTac2 = digitalRead(congTacHanhTrinh2);
    if (valueCongTac2 == LOW) {
      Serial.println("congTacHanhTrinh2 bat");
      motor_1_Dung();
      sCmd.readSerial();
//      _GuiTrangThaiDC();
      delay(200);
      GuiTrangThaiDC(1, 1, 0);
    } else {
      Serial.println("congTacHanhTrinh2 tat");
      sCmd.readSerial();
//      _GuiTrangThaiDC();
//      GuiTrangThaiDC(1, 0, 1);
    }
  } else if (valueRainSensor == LOW){
    Serial.println("Dang mua");
    motor_1_Lui(MAX_SPEED); // motor 1 tiến
    valueCongTac1 = digitalRead(congTacHanhTrinh1);
    if (valueCongTac1 == LOW) {
      Serial.println("congTacHanhTrinh1 bat");
      motor_1_Dung();
      sCmd.readSerial();
//      _GuiTrangThaiDC();
      delay(200);
      GuiTrangThaiDC(0, 0, 1);
    } else {
      Serial.println("congTacHanhTrinh1 tat");
      sCmd.readSerial();
//      _GuiTrangThaiDC();
//      GuiTrangThaiDC(0, 1, 0);
    }
  }
}
 
void loop() {
  sCmd.readSerial();
  //Bạn không cần phải thêm bất kỳ dòng code nào trong hàm loop này cả
  if (lenh_allowDKDC == true){
    while (1){
      sCmd.readSerial();
      valueCongTac1 = digitalRead(congTacHanhTrinh1);
      valueCongTac2 = digitalRead(congTacHanhTrinh2);
      if (kt_lenh_dkDC == 1 && valueCongTac1 == HIGH && valueCongTac2 == LOW){
        motor_1_Dung();
        if (chuoi_lenh_dkDC == "d"){
          Serial.println("d");
          delay(10);
          GuiTrangThaiDC(1, 1, 0);
//          delay(10);
//          GuiTrangThaiDC(1, 1, 0);
//          delay(10);
//          GuiTrangThaiDC(1, 1, 0);
        } else if (chuoi_lenh_dkDC == "D"){
          Serial.println("dv");
          delay(10);
          GuiTrangThaiDCVoice(1, 1, 0);
//          delay(10);
//          GuiTrangThaiDCVoice(1, 1, 0);
//          delay(10);
//          GuiTrangThaiDCVoice(1, 1, 0);
        }
        kt_lenh_dkDC = -1;
      } else if (kt_lenh_dkDC == 0 && valueCongTac1 == LOW && valueCongTac2 == HIGH){
        motor_1_Dung();
        if (chuoi_lenh_dkDC == "t"){
          Serial.println("t");
          delay(10);
          GuiTrangThaiDC(0, 0, 1);
//          delay(10);
//          GuiTrangThaiDC(0, 0, 1);
//          delay(10);
//          GuiTrangThaiDC(0, 0, 1);
        } else if (chuoi_lenh_dkDC == "T"){
          Serial.println("tv");
          delay(10);
          GuiTrangThaiDCVoice(0, 0, 1);
//          delay(10);
//          GuiTrangThaiDCVoice(0, 0, 1);
//          delay(10);
//          GuiTrangThaiDCVoice(0, 0, 1);
        }
        kt_lenh_dkDC = -1;
      }
      if (lenh_allowDKDC == false){
        break;
      }
    }
  } else if (lenh_allowDKDC == false){
    while (1){
      sCmd.readSerial();
      if (millis() - chuky1 > CHU_KY_1_LA_BAO_NHIEU) {
        chuky1 = millis();
    
        HeThongTuDong();
      }
      if (millis() - chuky1 > CHU_KY_1_LA_BAO_NHIEU) {
        chuky1 = millis();
    
        dht11Sensor();
      }
      if (lenh_allowDKDC == true){
        break;
      }
    }
  }
}

void allowDKDC() {
//  Serial.println("allowDKDC");
  char *json = sCmd.next(); //Chỉ cần một dòng này để đọc tham số nhận đươc
//  Serial.println(json);
  StaticJsonBuffer<200> jsonBuffer; //tạo Buffer json có khả năng chứa tối đa 200 ký tự
  JsonObject& root = jsonBuffer.parseObject(json);//đặt một biến root mang kiểu json
  
  lenh_allowDKDC = root["status"];
  if (lenh_allowDKDC){
    lenh_allowDKDC = true;
    Serial.println(lenh_allowDKDC);
  } else {
    lenh_allowDKDC = false;
    Serial.println(lenh_allowDKDC);
  }
}

void DKDC() {
  Serial.println("DKDC");
  char *json = sCmd.next(); //Chỉ cần một dòng này để đọc tham số nhận đươc
  Serial.println(json);
  StaticJsonBuffer<200> jsonBuffer; //tạo Buffer json có khả năng chứa tối đa 200 ký tự
  JsonObject& root = jsonBuffer.parseObject(json);//đặt một biến root mang kiểu json
 
  String lenh_dkDC = root["dieukhienDC"];
  chuoi_lenh_dkDC = lenh_dkDC;
  Serial.println(lenh_dkDC);
  if (lenh_dkDC == "d" || lenh_dkDC == "D"){ // het mua
    motor_1_Tien(MAX_SPEED); // motor 1 tien
    kt_lenh_dkDC = 1;
    Serial.println(kt_lenh_dkDC);
  } else if (lenh_dkDC == "t" || lenh_dkDC == "T"){ // dang mua
    motor_1_Lui(MAX_SPEED); // motor 1 lui
    kt_lenh_dkDC = 0;
    Serial.println(kt_lenh_dkDC);
  }
//  int blueStatus = root["led"][1];//json -> tham số root --> phần tử thứ 0. Đừng lo lắng nếu bạn không có phần tử này, không có bị lỗi đâu!
//  int yellowStatus = root["led"][2];//json -> tham số root --> phần tử thứ 0. Đừng lo lắng nếu bạn không có phần tử này, không có bị lỗi đâu!
 
  //kiểm thử giá trị
//  Serial.print(F("redStatus "));
//  Serial.println(redStatus);
//  Serial.print(F("blueStatus "));
//  Serial.println(blueStatus);
//  Serial.print(F("yellowStatus "));
//  Serial.println(yellowStatus);
 
 
  //xuất ra màn hình
//  digitalWrite(red, redStatus);
//  digitalWrite(blue, blueStatus);
//  digitalWrite(yellow, yellowStatus);
}

void CamBienAS() {
//  Serial.println("ASang");
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  //đọc giá trị cảm biến rồi in ra root
//  root["digital"] = digitalRead(lightSensor);
//  root["message"] = digitalRead(lightSensor) ? "Toi" : "Sang";


  //Gửi đi hoy!
  //in ra cổng software serial để ESP8266 nhận
//  mySerial.print("LIGHT");   //gửi tên lệnh
//  mySerial.print('\r');           // gửi \r
//  root.printTo(mySerial);        //gửi chuỗi JSON
//  mySerial.print('\r');    
}

void dht11Sensor(){
  // Wait a few seconds between measurements.
//  delay(2000);

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
//    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

//  Serial.print("Humidity: ");
//  Serial.print(h);
//  Serial.print(" %\t");
//  Serial.print("Temperature: ");
//  Serial.print(t);
//  Serial.print(" *C ");
//  Serial.print(f);
//  Serial.print(" *F\t");
//  Serial.print("Heat index: ");
//  Serial.print(hic);
//  Serial.print(" *C ");
//  Serial.print(hif);
//  Serial.println(" *F");

  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  //đọc giá trị cảm biến rồi in ra root
  root["Humidity"] = h;
  root["Temperature"] = t;


  //Gửi đi hoy!
  //in ra cổng software serial để ESP8266 nhận
  mySerial.print("DHT11");   //gửi tên lệnh
  mySerial.print('\r');           // gửi \r
  root.printTo(mySerial);        //gửi chuỗi JSON
  mySerial.print('\r');    
}
