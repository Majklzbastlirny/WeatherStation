/*
Michal Basler E4
Hlavní kód pro meteostanici.
Dodatky zde: https://github.com/Majklzbastlirny/WeatherStation
*/
#include <WiFi.h>
#include <BH1750.h>
float hd = 0;
float td = 0;
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include <Wire.h> //Including wire library
#include "DHT.h"
#define DHTPIN 4  
#define DHTTYPE DHT22 
#include <SFE_BMP180.h> //Including BMP180 library
#define ALTITUDE 333 //Altitude where I live (change this to your altitude)
DHT dht(DHTPIN, DHTTYPE);
SFE_BMP180 pressure; //Creating an object
BH1750 lightMeter;
float lux = 0; 
#define LEDDIAG 2
int UVOUT = 32; //Output from the sensor
int REF_3V3 = 33; //3.3V power on the Arduino board
/************************* WiFi Access Point *********************************/

#define WLAN_SSID         "MediumRecords"
#define WLAN_PASS         "123456780"

/************************* MQTT Broker Setup *********************************/

#define MQTT_BROKER_IP    "bladyhel.serveminecraft.net"
#define MQTT_BROKER_PORT  21    //default port is 1883
#define MQTT_USERNAME     "admin"
#define MQTT_PASSWORD     "123456780"

/************ Global State (you don't need to change this!) ******************/

WiFiClient client;

Adafruit_MQTT_Client mqtt(&client, MQTT_BROKER_IP, MQTT_BROKER_PORT, MQTT_USERNAME, MQTT_PASSWORD);

/************************* MQTT Topics ***************************************/

Adafruit_MQTT_Publish temperature = Adafruit_MQTT_Publish(&mqtt, "weatherStation/temperature");
Adafruit_MQTT_Publish humidity = Adafruit_MQTT_Publish(&mqtt, "weatherStation/humidity");
Adafruit_MQTT_Publish light = Adafruit_MQTT_Publish(&mqtt, "weatherStation/light");
Adafruit_MQTT_Publish presss = Adafruit_MQTT_Publish(&mqtt, "weatherStation/pressure");
Adafruit_MQTT_Publish UV = Adafruit_MQTT_Publish(&mqtt, "weatherStation/UV");


/*************************** Sketch Code ************************************/

// Bug workaround for Arduino 1.6.6, it seems to need a function declaration


void setup() {
pinMode(LEDDIAG, OUTPUT);  
//WiFi.setHostname(hostname.c_str()); //define hostname

  Serial.begin(9600);
  lightMeter.begin();
  delay(10);
 dht.begin(); 
 delay(250);
hd = dht.readHumidity();
  // Read temperature as Celsius (the default)
td = dht.readTemperature();

pinMode(UVOUT, INPUT);
pinMode(REF_3V3, INPUT);

if (isnan(hd) || isnan(td)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  lux = lightMeter.readLightLevel();


  
 if (pressure.begin()) //If initialization was successful, continue
    Serial.println("BMP180 init success");
  else //Else, stop code forever
  {
    Serial.println("BMP180 init fail");
    while (1);
  }
  Serial.println(F("Adafruit MQTT demo"));

  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());

  // Setup MQTT subscription for onoff feed.
  
}
int rescnt = 0;
uint32_t x=0;

void loop() {
  // Ensure the connection to the MQTT server is alive (this will make the first
  // connection and automatically reconnect when disconnected).  See the MQTT_connect
  // function definition further below.
digitalWrite(LEDDIAG, LOW);  
  MQTT_connect();

  // this is our 'wait for incoming subscription packets' busy subloop
  // try to spend your time here

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
  delay(1);
lux = lightMeter.readLightLevel();

if (isnan(hd) || isnan(td)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

 
  // Compute heat index in Celsius (isFahreheit = false)


  Serial.print(F("Humidity: "));
  Serial.print(hd);
  Serial.println("%");
  Serial.print(F("Temperature: "));
  Serial.print(td);
  Serial.println(F(" °C "));
  Serial.print("Heat index: ");

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



  // Now we can publish stuff!
  Serial.print(F("\nSending temperature val "));
  Serial.print(T);
  Serial.print("...");
  if (! temperature.publish(T)) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
  }
delay(150);  
  if (! humidity.publish(hd)) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
  }
delay(150);
if (! light.publish(lux)) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
  }
delay(150);
 if (! presss.publish(P)) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
  } 

delay(150);
 if (! UV.publish(uvIntensity)) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
  }  
  // ping the server to keep the mqtt connection alive
  // NOT required if you are publishing once every KEEPALIVE seconds
  /*
  if(! mqtt.ping()) {
    mqtt.disconnect();
  }
  */
rescnt++;
if (rescnt == 360) {
  ESP.restart();
} else {
  Serial.println(rescnt);
}
delay(10000);
digitalWrite(LEDDIAG, HIGH ); 
delay(100);   
}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}

 int averageAnalogRead( int pinToRead)
{
  byte numberOfReadings = 8;
   int runningValue = 0; 

  for(int x = 0 ; x < numberOfReadings ; x++)
    runningValue += analogRead(pinToRead);
  runningValue /= numberOfReadings;

  return(runningValue);  
}

float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;

}
