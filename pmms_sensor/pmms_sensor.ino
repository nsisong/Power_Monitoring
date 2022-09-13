#include <PZEM004Tv30.h>
#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#include "Wire.h" // For I2C
//#include "LCD.h" // For LCD
#include "LiquidCrystal_I2C.h" // Added library*
LiquidCrystal_I2C lcd(0x27,2,1,0,4,5,6,7);// D1 FOR SCL AND D2 FOR SDA

//// wifi name
//#define ssid  "Alma4G" 
//// wifi password
//#define password   "magnanimous111" 

// Data wire is connected to GPIO15
#define ONE_WIRE_BUS D4
// Setup a oneWire instance to communicate with a OneWire device
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire); 

//include the address of each of the sensor from the address code
DeviceAddress sensor1 = { 0x28, 0xFF, 0x64, 0x1E, 0x15, 0x8D, 0xDF, 0xA4 };
DeviceAddress sensor2 = { 0x28, 0xFF, 0x64, 0x1E, 0x0D, 0xB0, 0xFB, 0x2D };

const uint16_t port = 8091;
// the host is the ip of the raspberry pi
const char * host = "192.168.0.103";

//create a long press button variabes
const int button_pin = D3;
const int Short_press_time = 5000;
int lastState =LOW;
int currentState;
unsigned long pressTime =0;
unsigned long releasedTime = 0;

//vibration sensors config
int vib_pin1 = D8;
int vib_pin2 = D7;
int high_time;
int low_time;
float time_period;
float freq;
float frequency;

PZEM004Tv30 pzem(D5, D6);// Rx, Tx (for esp8266 TX, RX)

WiFiManager wifiManager;

void setup() {
  
  Serial.begin(115200);
  sensors.begin();
  pinMode(vib_pin1, INPUT);
  pinMode(vib_pin2, INPUT);
  pinMode(button_pin, INPUT);

// LCD setup
  lcd.begin(20, 4);
  lcd.setBacklightPin(3,POSITIVE);
  lcd.setBacklight(HIGH);
  lcd.setCursor(5,1);
  lcd.print("KODEHAUZ");
  lcd.setCursor(4,2);
  lcd.print("Power Meter");

// connect esp to wifi
//  WiFiManager wifiManager;
  wifiManager.setConfigPortalTimeout(180);
  
  connect_wifi();

  //if you get here you have connected to the WiFi
  lcd.clear();
  lcd.setCursor(1,1);
  lcd.println("connected to WIFI.");
//  WiFi.begin(ssid, password);
//  while (WiFi.status() != WL_CONNECTED) {
//    delay(500);
//    Serial.println("...");
//  }
//  Serial.print("WiFi connected with IP: ");
  Serial.println(WiFi.localIP());
  
  Serial.print("Reset Energy");
  pzem.resetEnergy();
  Serial.print("Set address to 0x42");
  pzem.setAddress(0x42);
  lcd.clear();
}
void connect_wifi(){
      if(!wifiManager.autoConnect("KH-pmms", "kodehauz")) {
    Serial.println("failed to connect and hit timeout");
    delay(3000);
    //reset and try again, or maybe put it to deep sleep
    ESP.restart();
    delay(5000);
  }
}
void vib_sensor(){

    high_time=pulseIn(vib_pin1,HIGH);
    low_time=pulseIn(vib_pin1,LOW);

 if (low_time != 0) {
    time_period=high_time+low_time;
    time_period=time_period/1000;
    frequency=1000/time_period;
 }
}
void reset_wifi(){
    currentState = digitalRead(button_pin);

  if( lastState == HIGH && currentState == LOW){
  pressTime = millis();
  }
  else if (lastState == LOW && currentState == HIGH){
    releasedTime = millis();

    long pressDuration = releasedTime - pressTime;
    Serial.println(pressDuration);
    if ( pressDuration > Short_press_time){
      wifiManager.resetSettings();
      connect_wifi();
    Serial.println("5s button pressed");
    }
  }
  lastState = currentState;
}

void loop() {
  reset_wifi();
  WiFiClient client;
      if (!client.connect(host, port)) {
        lcd.setCursor(0,0);
        lcd.print ("SERVER ERROR 500");
        lcd.setCursor(0,3);
        lcd.print ("CONNECTING TO HOST..");
        Serial.println("Connection to host failed");

        delay(2000);
        lcd.clear();
        return;
    }
  
    Serial.println("Connected to server successful!");
//    lcd.setCursor(0,0);
//    lcd.print ("CONNECTED....");   

  sensors.requestTemperatures(); // Send the command to get temperatures

  Serial.print("Sensor 1(°C): ");
  Serial.println(sensors.getTempC(sensor1));  
  Serial.print("Sensor 2(°C): ");
  Serial.println(sensors.getTempC(sensor2)); 
 

  float volt = pzem.voltage();
  float cur = pzem.current();
  float powe = pzem.power();
  float ener = pzem.energy();
  float freq = pzem.frequency();
  float pf = pzem.pf();
  vib_sensor();
  client.printf("%i,%.2f,%.2f,%.2f,%.3f,%.2f,%.2f,%.2f,%.2f,%.2f", 1, volt, 
                  cur, powe, ener, freq, pf, sensors.getTempC(sensor1),sensors.getTempC(sensor2),
                  frequency);

  lcd.setCursor(0,0);
  lcd.print(volt);
  lcd.setCursor(7,0);
  lcd.print("V");

  lcd.setCursor(0,1);
  lcd.print(cur);
  lcd.setCursor(7,1);
  lcd.print("A");

  lcd.setCursor(0,2);
  lcd.print(powe);
  lcd.setCursor(7,2);
  lcd.print("W");

  lcd.setCursor(0,3);
  lcd.print(ener,3);
  lcd.setCursor(7,3);
  lcd.print("KWh");

  lcd.setCursor(11,0);
  lcd.print(freq,2);
  lcd.setCursor(17,0);
  lcd.print("Hz");

  lcd.setCursor(11,1);
  lcd.print(pf);
  lcd.setCursor(17,1);
  lcd.print("P.F");
  

  delay(1000);
  lcd.clear();
  
  lcd.setCursor(0,0);
  lcd.print("Temp 1:");
  lcd.setCursor(8,0);
  lcd.print((sensors.getTempC(sensor1)));
  lcd.setCursor(14,0);
  lcd.print("\xDF""C");

  lcd.setCursor(0,1);
  lcd.print("Temp 1:");
  lcd.setCursor(8,1);
  lcd.print((sensors.getTempC(sensor2)));
  lcd.setCursor(14,1);
  lcd.print("\xDF""C");

  lcd.setCursor(0,2);
  lcd.print("Frequency:");
  lcd.setCursor(11,2);
  lcd.print(frequency);
  lcd.setCursor(17,2);
  lcd.print("Hz");

  delay(1000);
  lcd.clear();
  
}
