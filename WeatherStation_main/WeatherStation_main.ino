//Majkl's Weather Station with ESP32, MQTT, BMP180, BH1750, DHT22, ML8511, Wind vane and Anemometer

//Required libraries
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include <BH1750.h>
#include "DHT.h"
#include <ML8511.h>
#include <SFE_BMP180.h>
#include <WiFi.h>
#include <Wire.h>


//MQTT broker settings
#define HOST        "192.168.0.0"
#define PORT        1883
#define USERNAME    "yourusername"
#define PASSWORD    "yourpass"

//Wi-Fi settings (2.4GHz ONLY)
const char* ssid     = "your-ssid";
const char* password = "your-password";


//MQTT
Adafruit_MQTT_Client mqtt(&client, HOST, PORT, USERNAME, PASSWORD);
WiFiClient client;
//weather data
Adafruit_MQTT_Publish temperature = Adafruit_MQTT_Publish(&mqtt, "weatherStation/tempM");
Adafruit_MQTT_Publish pressure = Adafruit_MQTT_Publish(&mqtt, "weatherStation/presM");
Adafruit_MQTT_Publish humidity = Adafruit_MQTT_Publish(&mqtt, "weatherStation/humiM");
Adafruit_MQTT_Publish temperaturebackup = Adafruit_MQTT_Publish(&mqtt, "weatherStation/tempM2");
Adafruit_MQTT_Publish lightintensity = Adafruit_MQTT_Publish(&mqtt, "weatherStation/lighM");
Adafruit_MQTT_Publish uvintensity = Adafruit_MQTT_Publish(&mqtt, "weatherStation/uvinM");
Adafruit_MQTT_Publish winddirection = Adafruit_MQTT_Publish(&mqtt, "weatherStation/wdirM");
Adafruit_MQTT_Publish windspeed = Adafruit_MQTT_Publish(&mqtt, "weatherStation/wspeM");

//system data

Adafruit_MQTT_Publish heating = Adafruit_MQTT_Publish(&mqtt, "weatherStation/heatM");


//Wifi connect code
void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
}

//Sensor setup 
//DHT
#define DHTPIN 4 
#define DHTTYPE DHT22 
DHT dht(DHTPIN, DHTTYPE);

//ML8511
#define ANALOGPIN     A0
ML8511 light(ANALOGPIN);

//BH1750
BH1750 lightMeter;

//BMP180
SFE_BMP180 pressure;


void setup() {

  
Serial.begin(9600);
initWiFi();





}

void loop() {
  // put your main code here, to run repeatedly:

}
