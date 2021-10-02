
#include "DHT.h"
#define DHTPIN 4  
#define DHTTYPE DHT22 

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  

  dht.begin();
   delay(250);

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float hd = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float td = dht.readTemperature();
  

  // Check if any reads failed and exit early (to try again).
  if (isnan(hd) || isnan(td)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(td, hd, false);

  Serial.print(F("Humidity: "));
  Serial.print(hd);
  Serial.println("%");
  Serial.print(F("Temperature: "));
  Serial.print(td);
  Serial.println(F(" °C "));
  Serial.print("Heat index: ");
  Serial.print(hic);
  Serial.println(F(" °C "));
  
}

void loop() {
  // Wait a few seconds between measurements.
 
}
