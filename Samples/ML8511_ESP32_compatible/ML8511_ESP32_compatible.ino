
#include <ML8511.h>

#define ANALOGPIN     32 //až 35
#define ENABLEPIN     7

ML8511 light(ANALOGPIN);


void setup()
{
  Serial.begin(9600);
 
  
  // adjust to your ADC specification.
  light.setVoltsPerStep(3.3, 4096);      // 12 bit ADC
}


void loop()
{
  float UV = light.getUV();
  Serial.print(UV, 4);
  Serial.println(" mW cm^2");
  delay(1000);
}
