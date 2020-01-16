#include <SoftwareSerial.h>
// BODY TEMP SENSOR
#include <LM75A.h>
// Create I2C LM75A instance
LM75A lm75a_sensor(false,  //A0 LM75A pin state
                   false,  //A1 LM75A pin state
                   false); //A2 LM75A pin state

// AIR QUALITY SENSOR (PIN A4&A5)
#include "Adafruit_CCS811.h"
Adafruit_CCS811 ccs;

#include <ESP8266WiFi.h>; 
#include <WiFiClient.h>; 
#include <ThingSpeak.h>; 
const char* ssid = "Jordimantap"; //Your Network SSID 
const char* password = "andara123"; //Your Network Password
float temperature_in_degrees;
float temp;
int ecg;
int co2;
int TVOC;



WiFiClient client;
unsigned long myChannelNumber = 954046; //Your Channel Number (Without Brackets) 
const char * myWriteAPIKey = "SV4ZZOB54H82RKNF"; //Your Write API Key

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); //enable Serial Monitor
  delay(10);
  // Connect to WiFi network 
  WiFi.begin(ssid, password);
  ThingSpeak.begin(client);




  
  // AIR QUALITY SENSOR
  Serial.println("Starting Up CCS811");
  if (!ccs.begin()) {
    Serial.println("Failed to start sensor! Please check your wiring.");
    while (1);
  }
  // CALIBRATE AIR TEMP SENSOR
  while (!ccs.available());
  float temp = ccs.calculateTemperature();
  ccs.setTempOffset(temp - 25.0);

  //  ECG SENSOR
  pinMode(D3, INPUT); // Setup for leads off detection LO +
  pinMode(D4, INPUT); // Setup for leads off detection LO -
}




void loop() {
  // put your main code here, to run repeatedly:

  //  BODY TEMP SENSOR
  temperature_in_degrees = lm75a_sensor.getTemperatureInDegrees();

  if (temperature_in_degrees == INVALID_LM75A_TEMPERATURE) {
    Serial.println("Error while getting temperature");
  } else {
    Serial.println("");
    Serial.print("Body Temperature: ");
    Serial.print(temperature_in_degrees);
    Serial.println(" C°");
  }

  //  AIR QUALITY SENSOR
  if (ccs.available()) {
    temp = ccs.calculateTemperature();
    co2 = ccs.geteCO2();
    TVOC = ccs.getTVOC();
    if (!ccs.readData()) {
      Serial.print("CO2: ");
      Serial.print(co2);
      Serial.println(" ppm");
      Serial.print("TVOC (Total Volatile Organic Compound): ");
      Serial.print(TVOC);
      Serial.println(" ppb");
      Serial.print("Air Temperature: ");
      Serial.print(temp);
      Serial.println(" C°");
    }
    else {
      Serial.println("ERROR!");
      while (1);
    }
  }

  //  ECG SENSOR
  if ((digitalRead(D3) == 1) || (digitalRead(D4) == 1)) {
    Serial.println('!');
  }
  else {
    ecg = analogRead(A0);
    Serial.println(ecg);
  }
  delay(10);
  ThingSpeak.setField(1,temperature_in_degrees); //Update in ThingSpeak
  ThingSpeak.setField(2,co2); //Update in ThingSpeak
  ThingSpeak.setField(3,TVOC); //Update in ThingSpeak
  ThingSpeak.setField(4,temp); //Update in ThingSpeak
  ThingSpeak.setField(5,ecg); //Update in ThingSpeak
  ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey); //Update in ThingSpeak
  delay(100);
  
}
