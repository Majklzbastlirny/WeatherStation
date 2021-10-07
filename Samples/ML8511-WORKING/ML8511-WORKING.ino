 

//Hardware pin definitions
 int UVOUT = 32; //Output from the sensor
 int REF_3V3 = 33; //3.3V power on the Arduino board

void setup()
{
  Serial.begin(9600);

  pinMode(UVOUT, INPUT);
  pinMode(REF_3V3, INPUT);

  Serial.println("ML8511 example");
}

void loop()
{
  int uvLevel = averageAnalogRead(UVOUT);
  int refLevel = averageAnalogRead(REF_3V3);

  //Use the 3.3V power pin as a reference to get a very accurate output value from sensor
  float outputVoltage = 3.3 / refLevel * uvLevel;   // adjust outputVoltage to actual voltage in case you read negative values.

  float uvIntensity = mapfloat(outputVoltage, 0.99, 2.8, 0.0, 15.0); //Convert the voltage to a UV intensity level
 if (uvIntensity < 0) {
  uvIntensity = 0;
  }
  else { 
    uvIntensity = uvIntensity;
  }
  
  Serial.print("output: ");
  Serial.print(refLevel);

  Serial.print("ML8511 output: ");
  Serial.print(uvLevel);

  Serial.print(" / ML8511 voltage: ");
  Serial.print(outputVoltage);

  Serial.print(" / UV Intensity (mW/cm^2): ");
  Serial.print(uvIntensity);

  Serial.println();

  delay(100);
}

//Takes an average of readings on a given pin
//Returns the average
 int averageAnalogRead( int pinToRead)
{
  byte numberOfReadings = 8;
   int runningValue = 0; 

  for(int x = 0 ; x < numberOfReadings ; x++)
    runningValue += analogRead(pinToRead);
  runningValue /= numberOfReadings;

  return(runningValue);  
}

//The Arduino Map function but for floats
//From: http://forum.arduino.cc/index.php?topic=3922.0
float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;

}
