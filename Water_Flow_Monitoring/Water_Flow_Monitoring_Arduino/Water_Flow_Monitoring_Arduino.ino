////////////////////////////use RX TX pin of both the modules///////////////
//connection arduino RX to ESP TX and  arduino TX to ESP RX

byte statusLed    = 13;

byte sensorInterrupt = 0;  // 0 = digital pin 2
byte sensorPin       = 2;

// The hall-effect flow sensor outputs approximately 4.5 pulses per second per
// litre/minute of flow.
float calibrationFactor = 4.5;

volatile byte pulseCount;  

float flowRate;
unsigned int flowMilliLitres;
unsigned long totalMilliLitres;

unsigned long oldTime;
///////////////////////////
///////////////////////////
String flowrate;
String Sep1 = ",";
///////////////////////////
//////////////////////////

void setup()
{
  Serial.begin(115200);
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

/**
   Main program loop
*/
void loop()
{

  if ((millis() - oldTime) > 1000)   // Only process counters once per second
  {
    detachInterrupt(sensorInterrupt);
    flowRate = ((1000.0 / (millis() - oldTime)) * pulseCount) / calibrationFactor;
    oldTime = millis();
    flowMilliLitres = (flowRate / 60) * 1000;
    totalMilliLitres += flowMilliLitres;

    unsigned int frac;
    frac = (flowRate - int(flowRate)) * 10;
    String Sep = "." ;
    flowrate = flowRate + Sep + frac ;
   
    String FLowData = flowRate + Sep1 + flowMilliLitres + Sep1 + totalMilliLitres;
    Serial.println(FLowData);
    pulseCount = 0;
    attachInterrupt(sensorInterrupt, pulseCounter, FALLING);
  }
}

/*
  Insterrupt Service Routine
*/
void pulseCounter()
{
  // Increment the pulse counter
  pulseCount++;
}
