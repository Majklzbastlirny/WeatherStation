#include <Wire.h> //Including wire library

#include <SFE_BMP180.h> //Including BMP180 library

#define ALTITUDE 333 //Altitude where I live (change this to your altitude)

SFE_BMP180 pressure; //Creating an object
  
void setup() {
  Serial.begin(9600); //Starting serial communication

  Serial.println("Program started");

  if (pressure.begin()) //If initialization was successful, continue
    Serial.println("BMP180 init success");
  else //Else, stop code forever
  {
    Serial.println("BMP180 init fail");
    while (1);
  }
}

void loop() {
  char status;
  double T, P, p0; //Creating variables for temp, pressure and relative pressure

  Serial.print("You provided altitude: ");
  Serial.print(ALTITUDE, 0);
  Serial.println(" meters");

  status = pressure.startTemperature();
  if (status != 0) {
    delay(status);

    status = pressure.getTemperature(T);
    if (status != 0) {
      Serial.print("Temp: ");
      Serial.print(T, 1);
      Serial.println(" deg C");

      status = pressure.startPressure(3);

      if (status != 0) {
        delay(status);

        status = pressure.getPressure(P, T);
        if (status != 0) {
          Serial.print("Pressure measurement: ");
          Serial.print(P);
          Serial.println(" hPa (Pressure measured using temperature)");

          p0 = pressure.sealevel(P, ALTITUDE);
          Serial.print("Relative (sea-level) pressure: ");
          Serial.print(p0);
          Serial.println("hPa");
        }
      }
    }
  }  
  delay(10000);
}
