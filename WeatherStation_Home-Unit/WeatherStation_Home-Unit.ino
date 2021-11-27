
#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"


#define MQTT_BROKER_IP    "bladyhel.serveminecraft.net"
#define MQTT_BROKER_PORT  21    //default port is 1883
#define MQTT_USERNAME     "admin"
#define MQTT_PASSWORD     "123456780"

/************ Global State (you don't need to change this!) ******************/

WiFiClient client;

Adafruit_MQTT_Client mqtt(&client, MQTT_BROKER_IP, MQTT_BROKER_PORT, MQTT_USERNAME, MQTT_PASSWORD);

/************************* MQTT Topics ***************************************/

Adafruit_MQTT_Subscribe temperature = Adafruit_MQTT_Subscribe(&mqtt, "weatherStation/temperature");
Adafruit_MQTT_Subscribe humidity = Adafruit_MQTT_Subscribe(&mqtt, "weatherStation/humidity");
Adafruit_MQTT_Subscribe light = Adafruit_MQTT_Subscribe(&mqtt, "weatherStation/light");
Adafruit_MQTT_Subscribe presss = Adafruit_MQTT_Subscribe(&mqtt, "weatherStation/pressure");
Adafruit_MQTT_Subscribe UV = Adafruit_MQTT_Subscribe(&mqtt, "weatherStation/UV");
Adafruit_MQTT_Subscribe WV = Adafruit_MQTT_Subscribe(&mqtt, "weatherStation/WV");
Adafruit_MQTT_Subscribe WS = Adafruit_MQTT_Subscribe(&mqtt, "weatherStation/Speed");




const char *ssid     = "MediumRecords";
const char *password = "123456780";

const long utcOffsetInSeconds = 3600;

char daysOfTheWeek[7][12] = {"Nedele", "Pondeli", "Utery", "Streda", "Ctvrtek", "Patek", "Sobota"};
String months[12]={"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};


// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);



/* Uncomment the initialize the I2C address , uncomment only one, If you get a totally blank screen try the other*/
#define i2c_Address 0x3c //initialize with the I2C addr 0x3C Typically eBay OLED's
//#define i2c_Address 0x3d //initialize with the I2C addr 0x3D Typically Adafruit OLED's

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1   //   QT-PY / XIAO
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


Adafruit_BMP280 bme;
void setup()   {

  Serial.begin(9600);
  mqtt.subscribe(&temperature);
  mqtt.subscribe(&humidity);
  mqtt.subscribe(&light);
  mqtt.subscribe(&presss);
  mqtt.subscribe(&UV);
  mqtt.subscribe(&WV);
  mqtt.subscribe(&WS);
 

WiFi.begin(ssid, password);

  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    Serial.print ( "." );
  }

  timeClient.begin();



  display.begin(i2c_Address, true); // Address 0x3C default
// display.setContrast (50); // dim display
 if (!bme.begin(0x76,0x58)) { 
Serial.println("Could not find a valid BMP280 sensor, check wiring!");
while (1);
}
 
 Serial.println(bme.readPressure());
Serial.println(bme.readTemperature());

}


void loop() {
MQTT_connect();
 timeClient.update();


 Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5))) {
    if (subscription == &temperature) {
      Serial.print(F("Got: "));
      Serial.println((char *)temperature.lastread);
    }
    else if (subscription == &humidity) {
      Serial.print(F("Got: "));
      Serial.println((char *)humidity.lastread);
    }
    else if (subscription == &presss) {
      Serial.print(F("Got: "));
      Serial.println((char *)presss.lastread);
    }
    else if (subscription == &light) {
      Serial.print(F("Got: "));
      Serial.println((char *)light.lastread);
    }
    else if (subscription == &UV) {
      Serial.print(F("Got: "));
      Serial.println((char *)UV.lastread);
    }



    
  }










 

unsigned long epochTime = timeClient.getEpochTime();
struct tm *ptm = gmtime ((time_t *)&epochTime);
  Serial.print("Epoch Time: ");
  Serial.println(epochTime);
   int monthDay = ptm->tm_mday;
  Serial.print("Month day: ");
  Serial.println(monthDay);

  int currentMonth = ptm->tm_mon+1;
  Serial.print("Month: ");
  Serial.println(currentMonth);






  Serial.print(daysOfTheWeek[timeClient.getDay()]);
  Serial.print(" ");
  //Serial.print(timeClient.getHours());
  //Serial.print(":");
  //Serial.print(timeClient.getMinutes());
  //Serial.print(":");
  //Serial.println(timeClient.getSeconds());
  Serial.println(timeClient.getFormattedTime());


//  testdrawline();
display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print(daysOfTheWeek[timeClient.getDay()]);
 // display.print("Ctvrtek");
display.setCursor(43, 0);
display.print(monthDay);
display.print(".");
display.print(currentMonth);
display.print(".");
display.setCursor(80, 0);
  display.println(timeClient.getFormattedTime());
  display.drawLine(0, 8, display.width(), 8, SH110X_WHITE);
  display.setCursor(0, 10);
  display.print("Teplota je: ");
  display.setTextColor(SH110X_BLACK, SH110X_WHITE); // 'inverted' text
  display.print(bme.readTemperature());
  display.println(" C");

display.setTextColor(SH110X_WHITE);
 display.print("Teplota je: ");
  display.setTextColor(SH110X_WHITE); // 'inverted' text
  display.print((char *)temperature.lastread);
  display.println(" C");
 display.print("Vlhkost je: ");
  display.setTextColor(SH110X_WHITE); // 'inverted' text
  display.print((char *)humidity.lastread);
  display.println(" %");
 display.print("Tlak je: ");
  display.setTextColor(SH110X_WHITE); // 'inverted' text
  display.print((char *)presss.lastread);
  display.println(" hPa");
 display.print("Svetlo je: ");
  display.setTextColor(SH110X_WHITE); // 'inverted' text
  display.print((char *)light.lastread);
  display.println(" lux");
 display.print("UV je: ");
  display.setTextColor(SH110X_WHITE); // 'inverted' text
  display.print((char *)UV.lastread);
  display.println(" mW/mm2");
  display.drawLine(0, 63, display.width(), 63, SH110X_WHITE);
  
  display.display();
    if(! mqtt.ping()) {
    mqtt.disconnect();
  }
  delay(500);


}


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
