
#include <ML8511.h>

#define ANALOGPIN     32 //32 až 35


ML8511 light(32);


void setup()
{
  Serial.begin(115200);
 
  
  // adjust to your ADC specification.
  light.setVoltsPerStep(3.3, 4095);      // 12 bit ADC
}


void loop()
{
  float UV = light.getUV();
  Serial.print(UV, 4);
  Serial.println(" mW cm^2");
  delay(1000);
}
