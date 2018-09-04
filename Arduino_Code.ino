#include <SoftwareSerial.h>

// arduino RX pin=2  arduino TX pin=3
// connect the arduino RX pin to esp8266 module TX pin   -  connect the arduino TX pin to esp8266 module RX pin
SoftwareSerial espSerial =  SoftwareSerial(2, 3);

String apiKey = "3TAU2UJ268GI5MLJ";
String ssid = "WIFI NAME" ;
String password = "WIFI PASSWORD "  ;
boolean DEBUG = true;
byte statusLed    = 13;
byte sensorInterrupt = 0;  // 0 = digital pin 2
byte sensorPin       = 2;
float calibrationFactor = 4.5;
volatile byte pulseCount;
float flowRate;
unsigned int flowMilliLitres;
unsigned long totalMilliLitres;
unsigned long oldTime;

void setup() {
  // enable debug serial
  DEBUG = true;
  Serial.begin(9600);
  espSerial.begin(115200);

  // Enable this line to reset the module;
  //espSerial.println("AT+RST");
  //showResponse(1000);

  // Enable this line to set esp8266 serial speed to 9600 bps
  //espSerial.println("AT+UART_CUR=9600,8,1,0,0");
  //showResponse(1000);

  espSerial.println("AT+CWMODE=1");   // set esp8266 as client
  espSerial.println("AT+CWJAP=\"" + ssid + "\",\"" + password + "\""); // set your home router SSID and password
  if (DEBUG)  Serial.println("Setup completed");

  pinMode(statusLed, OUTPUT);
  pinMode(sensorPin, INPUT);
  digitalWrite(statusLed, HIGH);
  digitalWrite(sensorPin, HIGH);

  pulseCount        = 0;
  flowRate          = 0.0;
  flowMilliLitres   = 0;
  totalMilliLitres  = 0;
  oldTime           = 0;

  attachInterrupt(sensorInterrupt, pulseCounter, FALLING);
}



// ====================================================================== loop
void loop() {

  if ((millis() - oldTime) > 1000)   // Only process counters once per second
  {
    detachInterrupt(sensorInterrupt);

    flowRate = ((1000.0 / (millis() - oldTime)) * pulseCount) / calibrationFactor;
    oldTime = millis();
    flowMilliLitres = (flowRate / 60) * 1000;
    totalMilliLitres += flowMilliLitres;

    unsigned int frac;

    //    Serial.print("Flow rate: ");
    //    Serial.print(int(flowRate));  // Print the integer part of the variable
    //    Serial.print(".");             // Print the decimal point
    //    frac = (flowRate - int(flowRate)) * 10;
    //    Serial.print(frac, DEC) ;      // Print the fractional part of the variable
    //    Serial.print("L/min");
    //    Serial.print("  Current Liquid Flowing: ");             // Output separator
    //    Serial.print(flowMilliLitres);
    //    Serial.print("mL/Sec");
    //
    //    Serial.print("  Output Liquid Quantity: ");             // Output separator
    //    Serial.print(totalMilliLitres);
    //    Serial.println("mL");
    //
    thingSpeakWrite(flowMilliLitres, totalMilliLitres);
    pulseCount = 0;

    attachInterrupt(sensorInterrupt, pulseCounter, FALLING);
  }
}




