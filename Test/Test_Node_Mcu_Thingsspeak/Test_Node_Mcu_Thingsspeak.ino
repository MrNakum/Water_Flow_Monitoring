/* 

Script to write nodemcu sensor data into thingsspeak cloud

*/


// all import
#include <ESP8266WiFi.h>

// testing data value
int h = 0;
int t = 0;

//  Enter your Write API key from ThingSpeak
String apiKey = "P78Q5MQS1WZJ3DML";     

// Thingsspeak server id
const char* server = "api.thingspeak.com";

// replace with your wifi ssid and wpa2 key
const char *ssid =  "PY_LIN_";     
const char *pass =  "ironman10";

WiFiClient client;

void setup()
{
  Serial.begin(115200);
  delay(10);

  Serial.println("Connecting to ");
  Serial.println(ssid);


  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

}

void loop()
{

  // incremement data to write
  h += 1;
  t += 2;

  if (isnan(h) || isnan(t))
  {
    Serial.println("Failed to read test data!");
    return;
  }

  if (client.connect(server, 80))  //   "184.106.153.149" or api.thingspeak.com
  {

    String postStr = apiKey;
    postStr += "&field1=";
    postStr += String(t);
    postStr += "&field2=";
    postStr += String(h);
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

    Serial.print("Test 1: ");
    Serial.print(t);
    Serial.print("Test 2:");
    Serial.print(h);
    Serial.println("%. Send to Thingspeak.");
  }
  client.stop();

  Serial.println("Waiting...");

  // thingspeak needs minimum 15 sec delay between updates, i've set it to 30 seconds
  delay(10000);
}

