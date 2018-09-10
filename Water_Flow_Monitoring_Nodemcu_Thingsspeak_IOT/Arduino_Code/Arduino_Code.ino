/*

Author : Sanjay Prajapati , Nakum Urvish
Date   : 8th Sept 2018

Arduino Code to sense the sensor data and send that data on the thingsspeak cloud
even it will read the data from thingsspeak channel and write the output of arduino
as per our requirments.

*/


// import all libraries
#include <Arduino.h>
#include <EEPROM.h>
#define USE_SERIAL Serial
#include <ESP8266WiFi.h>
#include "ThingSpeak.h"
#include <ESP8266HTTPClient.h>

// Valve Control
#define Valve D1

// Variable init
int addr = 0;
float flowRate;
String Sep1 = ",";
char push_data[200];
unsigned long oldTime;
volatile byte pulseCount;
const int buttonPin = D2;
byte sensorInterrupt = 0;
unsigned int flowMilliLitres;
float calibrationFactor = 4.5;
unsigned long totalMilliLitres;

// router detail
const char * ssid = "PY_LIN_";
const char * password = "ironman10";

//  Enter your Write API key from ThingSpeak
String apiKey = "";

// modify this with your own Channel Number
unsigned long myChannelNumber = ;
const char * myReadAPIKey = "";

// Thingsspeak server id
const char* server = "api.thingspeak.com";

// variable to save channel field reading
int readValue;

int status = WL_IDLE_STATUS;
WiFiClient client;

void setup()
{
  Serial.begin(115200); // Start the Serial communication to send messages to the computer
  delay(10);
  Serial.println('\n');
  ThingSpeak.begin(client);

  startWIFI();
  pinMode(buttonPin, INPUT);
  pinMode(Valve, OUTPUT);
  digitalWrite(Valve, LOW);

  pulseCount = 0;
  flowRate = 0.0;
  flowMilliLitres = 0;
  totalMilliLitres = 0;
  oldTime = 0;

  digitalWrite(buttonPin, HIGH);
  attachInterrupt(digitalPinToInterrupt(buttonPin), pulseCounter, RISING);

}

void loop() {
  if (WiFi.status() == WL_CONNECTED && (millis() - oldTime) > 1000) // Only process counters once per second
  {

    readValue = ThingSpeak.readIntField(myChannelNumber, 1, myReadAPIKey);

    if ( readValue == 1)
    {
      digitalWrite(Valve, HIGH); // please notice if you need to modify this to LOW
      //  if your board's port active low
    }
    else
    {
      digitalWrite(Valve, LOW); // please notice if you need to modify this to HIGH
      //  if your board's port active low
    }


    detachInterrupt(sensorInterrupt);
    flowRate = ((1000.0 / (millis() - oldTime)) * pulseCount) / calibrationFactor;
    oldTime = millis();
    flowMilliLitres = (flowRate / 60) * 1000;
    totalMilliLitres += flowMilliLitres;
    unsigned int frac;

    frac = (flowRate - int(flowRate)) * 10;

    Serial.print("flow(L/min) : ");
    Serial.print(flowRate);
    Serial.print(" flow(ml/sec) : ");
    Serial.print(flowMilliLitres);
    Serial.print(" Total ml : ");
    Serial.print(totalMilliLitres);
    Serial.print(" Valve Status : ");
    Serial.println(readValue);
    
    if (flowRate > 0 )
    {
      if (client.connect(server, 80))
      {

        String postStr = apiKey;
        postStr += "&field1=";
        postStr += String(flowRate);
        postStr += "&field2=";
        postStr += String(flowMilliLitres);
        postStr += "&field3=";
        postStr += String(totalMilliLitres);
        postStr += "&field4=";
        postStr += String(readValue);
        postStr += "\r\n\r\n";

        client.print("POST /update HTTP/1.1\n");
        client.print("Host: api.thingspeak.com\n");
        client.print("Connection: close\n");
        client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
        client.print("Content-Type: application/x-www-form-urlencoded\n");
        client.print("Content-Length: ");
        client.print(postStr.length());
        client.print("\n\n");
        client.print(postStr);

        Serial.println("%. Send to Thingspeak.");
      }
      client.stop();
      Serial.println("Waiting...");

      // thingspeak needs minimum 15 sec delay between updates, i've set it to 30 seconds
      //delay(8000);

    }
    pulseCount = 0;
    attachInterrupt(sensorInterrupt, pulseCounter, FALLING);
  }

  else if (WiFi.status() != WL_CONNECTED)
  {
    startWIFI();
  }
}





