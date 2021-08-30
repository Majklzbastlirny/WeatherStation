//Majkl's Weather Station with ESP32, MQTT, BMP180, BH1750, DHT22, ML8511, Wind vane, Rain gauge and Anemometer

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
WiFiClient client;

//MQTT topic list
Adafruit_MQTT_Client mqtt(&client, HOST, PORT, USERNAME, PASSWORD);
//weather data
Adafruit_MQTT_Publish temperature = Adafruit_MQTT_Publish(&mqtt, "weatherStation/tempM");
Adafruit_MQTT_Publish pressure = Adafruit_MQTT_Publish(&mqtt, "weatherStation/presM");
Adafruit_MQTT_Publish humidity = Adafruit_MQTT_Publish(&mqtt, "weatherStation/humiM");
Adafruit_MQTT_Publish temperaturebackup = Adafruit_MQTT_Publish(&mqtt, "weatherStation/tempM2");
Adafruit_MQTT_Publish lightintensity = Adafruit_MQTT_Publish(&mqtt, "weatherStation/lighM");
Adafruit_MQTT_Publish uvintensity = Adafruit_MQTT_Publish(&mqtt, "weatherStation/uvinM");
Adafruit_MQTT_Publish winddirection = Adafruit_MQTT_Publish(&mqtt, "weatherStation/wdirM");
Adafruit_MQTT_Publish windspeed = Adafruit_MQTT_Publish(&mqtt, "weatherStation/wspeM");
Adafruit_MQTT_Publish raingauge = Adafruit_MQTT_Publish(&mqtt, "weatherStation/rainM");
//system data
Adafruit_MQTT_Publish batterytemp = Adafruit_MQTT_Publish(&mqtt, "weatherStation/battempM");
Adafruit_MQTT_Publish batteryperc = Adafruit_MQTT_Publish(&mqtt, "weatherStation/batpercM");
Adafruit_MQTT_Publish heating = Adafruit_MQTT_Publish(&mqtt, "weatherStation/heatM");
Adafruit_MQTT_Publish fanrmp = Adafruit_MQTT_Publish(&mqtt, "weatherStation/fanM");



void setup() {
Serial.begin(9600);





}

void loop() {
  // put your main code here, to run repeatedly:

}
