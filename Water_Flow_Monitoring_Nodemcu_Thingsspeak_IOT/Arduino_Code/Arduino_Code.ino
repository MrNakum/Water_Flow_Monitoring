#include <Arduino.h>
#include <EEPROM.h>
#define USE_SERIAL Serial
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

// Variable init
int addr = 0;
float flowRate;
String Sep1 = ",";
char push_data[200];
unsigned long oldTime;
const int ledPin = D7;
volatile byte pulseCount;
const int buttonPin = D2;
byte sensorInterrupt = 0;
unsigned int flowMilliLitres;
float calibrationFactor = 4.5;
unsigned long totalMilliLitres;


const char * ssid = "PY_LIN_";
const char * password = "ironman10";

//  Enter your Write API key from ThingSpeak
String apiKey = "P78Q5MQS1WZJ3DML";

// Thingsspeak server id
const char* server = "api.thingspeak.com";

//HTTP client init
HTTPClient http;

WiFiClient client;

void setup() {
  Serial.begin(115200); // Start the Serial communication to send messages to the computer
  delay(10);
  Serial.println('\n');

  startWIFI();
  pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);

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


    // Disable the interrupt while calculating flow rate and sending the value to
    // the host
    detachInterrupt(sensorInterrupt);

    // Because this loop may not complete in exactly 1 second intervals we calculate
    // the number of milliseconds that have passed since the last execution and use
    // that to scale the output. We also apply the calibrationFactor to scale the output
    // based on the number of pulses per second per units of measure (litres/minute in
    // this case) coming from the sensor.
    flowRate = ((1000.0 / (millis() - oldTime)) * pulseCount) / calibrationFactor;

    // Note the time this processing pass was executed. Note that because we've
    // disabled interrupts the millis() function won't actually be incrementing right
    // at this point, but it will still return the value it was set to just before
    // interrupts went away.
    oldTime = millis();

    // Divide the flow rate in litres/minute by 60 to determine how many litres have
    // passed through the sensor in this 1 second interval, then multiply by 1000 to
    // convert to millilitres.
    flowMilliLitres = (flowRate / 60) * 1000;

    // Add the millilitres passed in this second to the cumulative total
    totalMilliLitres += flowMilliLitres;

    unsigned int frac;

    // Print the flow rate for this second in litres / minute
    Serial.print("Flow rate: ");
    Serial.print(int(flowRate)); // Print the integer part of the variable
    Serial.print("."); // Print the decimal point
    // Determine the fractional part. The 10 multiplier gives us 1 decimal place.
    frac = (flowRate - int(flowRate)) * 10;
    Serial.print(frac, DEC); // Print the fractional part of the variable
    Serial.print("L/min");
    // Print the number of litres flowed in this second
    Serial.print("  Current Liquid Flowing: "); // Output separator
    Serial.print(flowMilliLitres);
    Serial.print("mL/Sec");

    // Print the cumulative total of litres flowed since starting
    Serial.print("  Output Liquid Quantity: "); // Output separator
    Serial.print(totalMilliLitres);
    Serial.println("mL");
    //String FLowData = flowRate + Sep1 + flowMilliLitres + Sep1 + totalMilliLitres;
    //Serial.println(FLowData);

    if (flowRate > 0)
    {
      if (client.connect(server, 80))  //   "184.106.153.149" or api.thingspeak.com
      {

        String postStr = apiKey;
        postStr += "&field1=";
        postStr += String(flowRate);
        postStr += "&field2=";
        postStr += String(flowMilliLitres);
        postStr += "&field3=";
        postStr += String(totalMilliLitres);
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

        Serial.print(" flowRate : ");
        Serial.print(flowRate);
        Serial.print(" flowMilliLitres : ");
        Serial.print(flowMilliLitres);
        Serial.print(" totalMilliLitres : ");
        Serial.print(totalMilliLitres);
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



