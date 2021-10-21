/***************************************************
  Adafruit MQTT Library ESP8266 Example

  Must use ESP8266 Arduino from:
    https://github.com/esp8266/Arduino

  Works great with Adafruit's Huzzah ESP board & Feather
  ----> https://www.adafruit.com/product/2471
  ----> https://www.adafruit.com/products/2821

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Tony DiCola for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 float hd = 0;
float td = 0;
 ****************************************************/
#include <WiFi.h>
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
/************************* WiFi Access Point *********************************/

#define WLAN_SSID       "MediumRecords"
#define WLAN_PASS       "123456780"

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "bladyhel.serveminecraft.net"
#define AIO_SERVERPORT  21                  // use 8883 for SSL
#define AIO_USERNAME    "admin"
#define AIO_KEY         "123456780"

/************ Global State (you don't need to change this!) ******************/

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;
// or... use WiFiClientSecure for SSL
//WiFiClientSecure client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

/****************************** Feeds ***************************************/

// Setup a feed called 'photocell' for publishing.
// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>
Adafruit_MQTT_Publish photocell = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/photocell");
Adafruit_MQTT_Publish uwu = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/uwu");
// Setup a feed called 'onoff' for subscribing to changes.
Adafruit_MQTT_Subscribe onoffbutton = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/onoff");

/*************************** Sketch Code ************************************/

// Bug workaround for Arduino 1.6.6, it seems to need a function declaration
// for some reason (only affects ESP8266, likely an arduino-builder bug).

void setup() {
  
  Serial.begin(115200);
  delay(10);
 dht.begin(); 
 delay(250);
hd = dht.readHumidity();
  // Read temperature as Celsius (the default)
td = dht.readTemperature();



if (isnan(hd) || isnan(td)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }


  
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
  mqtt.subscribe(&onoffbutton);
}

uint32_t x=0;

void loop() {
  // Ensure the connection to the MQTT server is alive (this will make the first
  // connection and automatically reconnect when disconnected).  See the MQTT_connect
  // function definition further below.
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

  

  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5000))) {
    if (subscription == &onoffbutton) {
      Serial.print(F("Got: "));
      Serial.println((char *)onoffbutton.lastread);
    }
  }

  // Now we can publish stuff!
  Serial.print(F("\nSending photocell val "));
  Serial.print(T);
  Serial.print("...");
  if (! photocell.publish(T)) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
  }
  if (! uwu.publish(hd)) {
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
