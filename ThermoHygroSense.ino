/*
 * Project by Syazwan AI & Amierul S
 * Gas ID: XXXXXX
 */

#include<Wire.h>
#include "ClosedCube_HDC1080.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define CCS811_ADDR 0x5A

Adafruit_SSD1306 display(128, 64, & Wire, -1);
ClosedCube_HDC1080 hdc1080;

//------------- WiFi -----
const char* ssid = "username"; //--> Your wifi name or SSID.
const char* password = "password"; //--> Your wifi password.
const char* host = "script.google.com";
const int httpsPort = 443;

WiFiClientSecure client; //--> Create a WiFiClientSecure object.
String GAS_ID = "WebApps"; //--> spreadsheet script ID

//-------------end WiFi----

float thi; float t; float h; float nt; float nh; String latong = "1.5459771077916329,103.79077517258013"; String  statusthi; 

void setup() {
  Wire.begin();
  hdc1080.begin(0x40);
  Serial.begin(115200);

  //OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  delay(2000);
  display.clearDisplay();
  display.setTextColor(WHITE);
  Serial.println("PM25 found!");
  delay(1000);
  
  //WiFi
  WiFi.begin(ssid, password); //--> Connect to your WiFi router
  Serial.println("");
  
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    //----------------------------------------Make the On Board Flashing LED on the process of connecting to the wifi router.
    digitalWrite(LED_BUILTIN, LOW);
    delay(250);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(250);
    //----------------------------------------
  }
  //----------------------------------------
  digitalWrite(LED_BUILTIN, HIGH); //--> Turn off the On Board LED when it is connected to the wifi router.
  Serial.println("");
  Serial.print("Successfully connected to : ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  //----------------------------------------
  Serial.println("I'm awake, but I'm going into deep sleep mode for 30 seconds");

  client.setInsecure();  
}

void loop() {
  //Temperature and Humidity
  nt = hdc1080.readTemperature();
  nh = hdc1080.readHumidity();
  
  //final calibration based on algorithm ***THI****
  t = (((1.0081 * nt) - (0.3202)) + 1.6); //Rsquare = 0.9999999125
  h = (((1.0305 * nh) - (0.7127)) - 16.54); // Rqsuare = 0.9996
  float tf = ((t*1.8)+32); //convertion of the temperature into Farenhiet
  thi = (tf - (0.55-(0.55*h/100))*(tf-58));

  //display in serial result
  String temp = "Temperature : " + String(t) + " Degree Celcius";
  String humid = "Humidity : " + String(h) + " %";
  String temphumid = "THI Index : " + String(thi) + " THI Value";
  Serial.println(temp);
  Serial.println(humid);
  Serial.println(temphumid);

  if(thi>80)
  {
    sendData1(); //--> Calls the sendData Subroutine
    }
  else
  {
    sendData2();    
  }
}

//-----

// Subroutine for sending data to Google Sheets SendData1
// float t, float h, float thi, int co2, int tvoc, int pm25, int pm10
void sendData1() {
  Serial.println("==========");
  Serial.print("connecting to ");
  Serial.println(host);

  //----------------------------------------Connect to Google host
  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }
  //----------------------------------------

  //----------------------------------------Processing data and sending data
  String string_t =  String(t);
  String string_h =  String(h);
  String string_thi =  String(thi);
  String statusthi = "Poor";
  String string_latlong = "1.5459771077916329,103.79077517258013";
  
  String url = "/macros/s/" + GAS_ID + "/exec?temperature=" + string_t + "&humidity=" + string_h + 
  "&thi=" + string_thi + "&status=" + statusthi + "&latlong=" + string_latlong;

  Serial.print("requesting URL: ");
  Serial.println(url);

  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: BuildFailureDetectorESP8266\r\n" +
               "Connection: close\r\n\r\n");

  Serial.println("request sent");
  //----------------------------------------

  //----------------------------------------Checking whether the data was sent successfully or not
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      Serial.println("headers received");
      break;
    }
  }
  String line = client.readStringUntil('\n');
  if (line.startsWith("{\"state\":\"success\"")) {
    Serial.println("esp8266/Arduino CI successfull!");
  } else {
    Serial.println("esp8266/Arduino CI has failed");
  }
  Serial.print("reply was : ");
  Serial.println(line);
  Serial.println("closing connection");
  Serial.println("==========");
  Serial.println();
  //---------------------------------------- 


     //display on OLED LCD
  delay(500);
  display.clearDisplay();

  // display temperature
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("Wataverse Technology");
  display.setTextSize(1);
  display.setCursor(0, 10);
  display.print("====================");
  display.setTextSize(1);
  display.setCursor(0, 20);
  display.print("Temp [C] & Hum [%]");
  display.setTextSize(1.5);
  display.setCursor(0, 30);
  display.print(t);
  display.print(" ");
  display.setTextSize(1);
  display.cp437(true);
  display.write(167);
  display.setTextSize(1);
  display.print("C ");
  display.setTextSize(1.5);
  display.setCursor(60, 30);
  display.print(h);
  display.print(" %");

  //display pm2.5
  display.setTextSize(1.5);
  display.setCursor(0, 40);
  display.print("THI:");
  display.setTextSize(1.5);
  display.setCursor(30, 40);
  display.print(thi);
  display.print(" ");
  display.setTextSize(1.5);
  display.setCursor(0, 50);
  display.print("STAT:");
  display.setTextSize(1.5);
  display.setCursor(40, 50);
  display.print(statusthi);
  display.print("");

  display.display();

  delay(1000);
}

//====== Send Data 2 for option No. 2
void sendData2() {
  Serial.println("==========");
  Serial.print("connecting to ");
  Serial.println(host);

  //----------------------------------------Connect to Google host
  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }
  //----------------------------------------

  //----------------------------------------Processing data and sending data
 String string_t =  String(t);
  String string_h =  String(h);
  String string_thi =  String(thi);
  String statusthi = "Good";
  String string_latlong = "1.5459771077916329,103.79077517258013";
  
  String url = "/macros/s/" + GAS_ID + "/exec?temperature=" + string_t + "&humidity=" + string_h + 
  "&thi=" + string_thi + "&status=" + statusthi + "&latlong=" + string_latlong;


  Serial.print("requesting URL: ");
  Serial.println(url);

  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: BuildFailureDetectorESP8266\r\n" +
               "Connection: close\r\n\r\n");

  Serial.println("request sent");
  //----------------------------------------

  //----------------------------------------Checking whether the data was sent successfully or not
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      Serial.println("headers received");
      break;
    }
  }
  String line = client.readStringUntil('\n');
  if (line.startsWith("{\"state\":\"success\"")) {
    Serial.println("esp8266/Arduino CI successfull!");
  } else {
    Serial.println("esp8266/Arduino CI has failed");
  }
  Serial.print("reply was : ");
  Serial.println(line);
  Serial.println("closing connection");
  Serial.println("==========");
  Serial.println();
  //---------------------------------------- 

     //display on OLED LCD
  delay(500);
  display.clearDisplay();

  // display temperature
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("Wataverse Technology");
  display.setTextSize(1);
  display.setCursor(0, 10);
  display.print("====================");
  display.setTextSize(1);
  display.setCursor(0, 20);
  display.print("Temp [C] & Hum [%]");
  display.setTextSize(1.5);
  display.setCursor(0, 30);
  display.print(t);
  display.print(" ");
  display.setTextSize(1);
  display.cp437(true);
  display.write(167);
  display.setTextSize(1);
  display.print("C ");
  display.setTextSize(1.5);
  display.setCursor(60, 30);
  display.print(h);
  display.print(" %");

  //display pm2.5
  display.setTextSize(1.5);
  display.setCursor(0, 40);
  display.print("THI:");
  display.setTextSize(1.5);
  display.setCursor(30, 40);
  display.print(thi);
  display.print(" ");
  display.setTextSize(1.5);
  display.setCursor(0, 50);
  display.print("STAT:");
  display.setTextSize(1.5);
  display.setCursor(40, 50);
  display.print(statusthi);
  display.print(".");

  display.display();

  delay(1000);
}
