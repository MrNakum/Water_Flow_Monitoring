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
float totalMilliLitres;

// router detail
const char * ssid = "PY_LIN_";
const char * password = "ironman10";

//  Enter your Write API key from ThingSpeak
//  API contain flow data 
unsigned long myWriteChannelNumber = 571234;
const char * myWriteAPIKey = "12J4A6HIAJGAYVIJ";


//  Enter your Read API key from ThingSpeak
//  API contain vlave on and off status 
unsigned long myReadChannelNumber = 571676;
const char * myReadAPIKey = "MZ27W4SM5T56V0BT";

// Thingsspeak server id
const char* server = "api.thingspeak.com";

int status = WL_IDLE_STATUS;

// variable to save channel field reading
int readValue;


WiFiClient client;

void setup() {
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

    readValue = ThingSpeak.readIntField(myReadChannelNumber, 1, myReadAPIKey);

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
    float flowRate = ((1000.0 / (millis() - oldTime)) * pulseCount) / calibrationFactor;
    oldTime = millis();
    int flowMilliLitres = (flowRate / 60) * 1000;
    totalMilliLitres += flowMilliLitres;
    unsigned int frac;


    frac = (flowRate - int(flowRate)) * 10;

    Serial.print("flowRate : ");
    Serial.println(flowRate);
    Serial.print("flowMilliLitres : ");
    Serial.println(flowMilliLitres);
    Serial.print("totalMilliLitres : ");
    Serial.println(totalMilliLitres);
    Serial.print("Valve Status : ");
    Serial.println(readValue);
    Serial.println("");

    if (flowRate > 0 )
    {
      if (client.connect(server, 80))
      {

//        String postStr = apiKey;
//        postStr += "&field1=";
//        postStr += String(flowRate);
//        postStr += "&field2=";
//        postStr += String(flowMilliLitres);
//        postStr += "&field3=";
//        postStr += String(totalMilliLitres);
//        postStr += "&field4=";
//        postStr += String(readValue);
//        postStr += "\r\n\r\n";

        
        ThingSpeak.writeField(myWriteChannelNumber, 1, flowRate, myWriteAPIKey);
        ThingSpeak.writeField(myWriteChannelNumber, 2, flowMilliLitres, myWriteAPIKey);
        ThingSpeak.writeField(571234, 3, totalMilliLitres, "12J4A6HIAJGAYVIJ");
        ThingSpeak.writeField(myWriteChannelNumber, 4, readValue, myWriteAPIKey);
        
//        client.print("POST /update HTTP/1.1\n");
//        client.print("Host: api.thingspeak.com\n");
//        client.print("Connection: close\n");
//        client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
//        client.print("Content-Type: application/x-www-form-urlencoded\n");
//        client.print("Content-Length: ");
//        client.print(postStr.length());
//        client.print("\n\n");
//        client.print(postStr);
//

        Serial.println("%. Send to Thingspeak.");
      }
      client.stop();

      Serial.println("Waiting...");

      // thingspeak needs minimum 15 sec delay between updates, i've set it to 30 seconds
      delay(8000);

    }


    pulseCount = 0;

    // Enable the interrupt again now that we've finished sending output
    attachInterrupt(sensorInterrupt, pulseCounter, FALLING);
  }

  else if (WiFi.status() != WL_CONNECTED)
  {
    startWIFI();
  }
}



