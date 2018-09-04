byte statusLed    = 13;
byte sensorInterrupt = 0;  // 0 = digital pin 2
byte sensorPin       = 2;
float calibrationFactor = 4.5;

volatile byte pulseCount;

float flowRate;
unsigned int flowMilliLitres;
unsigned long totalMilliLitres;

unsigned long oldTime;

void setup()
{
  Serial.begin(38400);
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

    Serial.print("Flow rate: ");
    Serial.print(int(flowRate));  // Print the integer part of the variable
    Serial.print(".");             // Print the decimal point
    frac = (flowRate - int(flowRate)) * 10;
    Serial.print(frac, DEC) ;      // Print the fractional part of the variable
    Serial.print("L/min");
    Serial.print("  Current Liquid Flowing: ");             // Output separator
    Serial.print(flowMilliLitres);
    Serial.print("mL/Sec");

    Serial.print("  Output Liquid Quantity: ");             // Output separator
    Serial.print(totalMilliLitres);
    Serial.println("mL");

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
