#include <ESP8266WiFi.h>

String apiKey = "3TAU2UJ268GI5MLJ";     //  Enter your Write API key from ThingSpeak

const char *ssid =  "PY_LIN_";     // replace with your wifi ssid and wpa2 key
const char *pass =  "ironman10";
const char* server = "api.thingspeak.com";
WiFiClient client;

void setup()
{
  Serial.begin(115200);
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

  if (Serial.available() > 0)
  {
    String flowrate = Serial.readStringUntil (',');
    Serial.read();

    String CurrentLiquidFlowing = Serial.readStringUntil (',');
    Serial.read();

    String OutLiquidQuantity = Serial.readStringUntil ('\0');
    //////////////////////////////////////////////////////////
    ////////Manual testing loop///////////////////////////////////

    //    Serial.print( flowrate );
    //    Serial.print( '\n' );
    //    Serial.print ( CurrentLiquidFlowing );
    //    Serial.print( '\n' );
    //    Serial.print (OutLiquidQuantity);
    //    Serial.print('\n' );
    //    if (isnan(flowrate) || isnan(CurrentLiquidFlowing) || isnan(OutLiquidQuantity) )
    //    {
    //      Serial.println("Failed to read from DHT sensor!");
    //      return;
    //    }

    if (client.connect(server, 80))  //   "184.106.153.149" or api.thingspeak.com
    {

      String postStr = apiKey;
      postStr += "&field1=";
      postStr += String(flowrate);
      postStr += "&field2=";
      postStr += String(CurrentLiquidFlowing);
      postStr += "&field3=";
      postStr += String(OutLiquidQuantity);
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

      //      Serial.print("Temperature: ");
      //      Serial.print(flowrate);
      //      Serial.print(" degrees Celcius, Humidity: ");
      //      Serial.print(CurrentLiquidFlowing);
      Serial.println("%. Send to Thingspeak.");
    }
    client.stop();

    Serial.println("Waiting...");

    // thingspeak needs minimum 15 sec delay between updates, i've set it to 30 seconds
    delay(10000);


    /////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////
  }
}



