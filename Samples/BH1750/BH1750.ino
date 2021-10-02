#include <Wire.h>
#include <BH1750.h>
 
 
BH1750 lightMeter;
float lux = 0; 
 
void setup(){
  Serial.begin(9600);
  lightMeter.begin();
  Serial.println("Running...");
}
 
 
void loop() {
  lux = lightMeter.readLightLevel();
  Serial.print("Light: ");
  Serial.print(lux);
  Serial.println(" lux");
  delay(1000);
}
