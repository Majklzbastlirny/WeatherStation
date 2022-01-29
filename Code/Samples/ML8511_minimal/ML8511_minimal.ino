//
//    FILE: ML8511_minimal.ino
//  AUTHOR: Rob Tillaart
// VERSION: 0.1.1
// PURPOSE: demo UV sensor
//    DATE: 2020-02-17
//     URL: https://github.com/RobTillaart/ML8511

//        BREAKOUT
//      +-------+--+
//  VIN |o      +-+| mounting hole
//  3V3 |o      +-+|
//  GND |o         |
//  OUT |o         |
//   EN |o       S |  Sensor
//      +----------+
//   EN = ENABLE



#include <ML8511.h>

#define ANALOGPIN     A0
float UV = 0;
// Connect EN to 3V3 to explicitly 
// enable the sensor continuously
ML8511 light(ANALOGPIN);


void setup()
{
  Serial.begin(9600);
  Serial.println("UV UltraViolet ML8511");
  light.setVoltsPerStep(3.3, 4095);      // 12 bit DAC

}


void loop()
{
  UV = light.getUV();
  Serial.print(UV, 4);
  Serial.println(" mW cm^2");
  delay(1000);
}


// -- END OF FILE --
