#include <ESP32Servo.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <HTTPClient.h>
#define USE_SERIAL Serial

Servo myservo;
int posVal = 0; 
int servoPin = 15;
int outPorts[] = {17, 27, 26, 25};
String address= "http://134.122.113.13/jtk2141/running";
const char *ssid_Router = "Columbia University";

void setup() {
  USE_SERIAL.begin(115200);
  WiFi.begin(ssid_Router);
  USE_SERIAL.println(String("Connecting to ")+ssid_Router);
  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    USE_SERIAL.print(".");
  }
  USE_SERIAL.println("\nConnected, IP address: ");
  USE_SERIAL.println(WiFi.localIP());
  for (int i = 0; i < 4; i++) {
    pinMode(outPorts[i], OUTPUT);
    }
  myservo.setPeriodHertz(50);
  myservo.attach(servoPin, 500, 2500);
}

void loop(){
  if((WiFi.status() == WL_CONNECTED)) {
    HTTPClient http;
    http.begin(address);
    
 
    int httpCode = http.GET(); // start connection and send HTTP header
    if (httpCode == HTTP_CODE_OK) { 
        String response = http.getString();
        USE_SERIAL.println(response);
        if (response.equals("false")) {
            USE_SERIAL.println("Waiting for True");
        }
        else if(response.equals("true")) {
            moveSteps(true, 32 * 64, 3);
            delay(1000);
            for (posVal = 0; posVal <= 180; posVal += 1) {
              myservo.write(posVal);
              delay(15);
              }
            moveSteps(false, 32 * 64, 3);
            delay(1000);
            for (posVal = 180; posVal >= 0; posVal -= 1) {
                myservo.write(posVal);
                delay(15);
              }
        }
        USE_SERIAL.println("Response was: " + response);
    } else {
        USE_SERIAL.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
    delay(500); // sleep for half of a second
  }
 
}

void moveSteps(bool dir, int steps, byte ms) {
 for (unsigned long i = 0; i < steps; i++) {
 moveOneStep(dir); // Rotate a step
 delay(constrain(ms,3,20)); // Control the speed
 }
}

void moveOneStep(bool dir) {
 static byte out = 0x01;
 if (dir) { // ring shift left
 out != 0x08 ? out = out << 1 : out = 0x01;
 }
 else{
  out != 0x01 ? out = out >> 1 : out = 0x08;
 }
 for (int i = 0; i < 4; i++) {
 digitalWrite(outPorts[i], (out & (0x01 << i)) ? HIGH : LOW);
 }
}

void moveAround(bool dir, int turns, byte ms){
 for(int i=0;i<turns;i++){
  moveSteps(dir,32*64,ms);
 }
}

void moveAngle(bool dir, int angle, byte ms){
 moveSteps(dir,(angle*32*64/360),ms);
}
