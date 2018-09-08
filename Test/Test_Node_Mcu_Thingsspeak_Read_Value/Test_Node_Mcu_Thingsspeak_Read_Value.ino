#include "ThingSpeak.h"
#include <ESP8266WiFi.h>

#define LED D1

char ssid[] = "PY_LIN_";          //  your network SSID (name) 
char pass[] = "ironman10";   // your network password

int status = WL_IDLE_STATUS;
WiFiClient  client;
int readValue; // variable to save channel field reading
    
unsigned long myChannelNumber = 569034; // modify this with your own Channel Number
const char * myReadAPIKey = "5H218WOV9MLD1ZAH";

void setup() {
  Serial.begin(9600); // for debugging reasons
  WiFi.begin(ssid, pass);
  ThingSpeak.begin(client);
  pinMode(LED, OUTPUT);

  digitalWrite(LED, LOW);
}

void loop() {
  // Read the latest value from field 1 of your channel
   readValue = ThingSpeak.readIntField(myChannelNumber, 1, myReadAPIKey);
  Serial.print("readValue = "); // debugging instrument
  Serial.println(readValue);    // debugging instrument

  if ( readValue == 1){
    digitalWrite(LED, HIGH); // please notice if you need to modify this to LOW 
                            //  if your board's port active low
  }
  else{
    digitalWrite(LED, LOW); // please notice if you need to modify this to HIGH 
                            //  if your board's port active low   
  }
}
