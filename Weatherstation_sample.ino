 #include <Wire.h>
#include <ESP8266WiFi.h>
#include "DHT.h"
//#include <Adafruit_BMP085.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

// https://bitluni.net/solar-powered-weather-station
// https://www.youtube.com/watch?v=at7wmm9t8UE

//comment the next line in if you like to publish voltage instead of light. It's an command line option.
//#define PUBLISH_VOLTAGE

#ifdef PUBLISH_VOLTAGE
ADC_MODE(ADC_VCC);
#endif

// -------- MQTT broker ---------- //
#define HOST        "192.168.0.0"
#define PORT        1883
#define USERNAME    "admin"
#define PASSWORD    "mqttpass"

// ---------- DHT -------- //
// capacitive humidity and thermistor
#define DHTPIN 0 //D5 
DHT dht(DHTPIN, DHT11); // dht object of DHT class

// --------- Time ------------- //
const int intervall = 60000;  //milliseconds to sleep
const int timeout = 200;  //time out loop count

// ---------- LAN ------------- //
#define WLAN_SSID   "wifissid"
#define WLAN_PASS   "wifipass"
WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, HOST, PORT, USERNAME, PASSWORD);
Adafruit_MQTT_Publish temperature = Adafruit_MQTT_Publish(&mqtt, "weatherStation/temperaturesklenik"); // These relate to node red
//Adafruit_MQTT_Publish pressure = Adafruit_MQTT_Publish(&mqtt, "weatherStation/pressure");
Adafruit_MQTT_Publish humidity = Adafruit_MQTT_Publish(&mqtt, "weatherStation/humiditysklenik");
//Adafruit_MQTT_Publish light = Adafruit_MQTT_Publish(&mqtt, "weatherStation/light");
//Adafruit_MQTT_Publish voltage = Adafruit_MQTT_Publish(&mqtt, "weatherStation/voltage");
void MQTT_connect();

//Adafruit_BMP085 bmp;  // barometric pressure sensor object bmp

void setup() 
{
  WiFi.forceSleepWake();  // wakes up hardware
  delay(1);
  WiFi.mode(WIFI_STA);  

  //Serial.begin(115200);
  //Serial.print("Connecting to ");
  //Serial.println(WLAN_SSID);

  // Attempt Network Connection
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  int i = 0;
  for (; i < timeout; i++)
  {
    if(WiFi.status() == WL_CONNECTED) break;
    delay(100);
    //Serial.print(".");
  }
  if(i == timeout) {deepSleep();} // go to deep sleep if connection fails

  //Serial.println("IP address: "); Serial.println(WiFi.localIP());

 // bmp.begin();
  dht.begin();

  MQTT_connect();
  delay(100);
  temperature.publish(dht.readTemperature());
  delay(100);
 //pressure.publish(bmp.readPressure());
  humidity.publish(dht.readHumidity());
  delay(250);

//#ifdef PUBLISH_VOLTAGE
//  voltage.publish(ESP.getVcc());
//#else
//  light.publish(analogRead(A0) / 1023.0f);
//#endif
  
  //Serial.println("deep sleep");
  deepSleep();
}

void loop() 
{
}

// --------- Functinons -------- //

void deepSleep()
{
  //https://github.com/esp8266/Arduino/issues/644
  WiFi.mode(WIFI_OFF);
  WiFi.forceSleepBegin();
  delay(1);
  ESP.deepSleep(intervall * 1000/*, WAKE_RF_DISABLED*/);
}


void MQTT_connect() 
{
  int8_t ret;
  if (mqtt.connected()) {return;}

  Serial.print("Connecting to MQTT... ");
  uint8_t retries = 10;                 // attempts to connect
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       //Serial.println(mqtt.connectErrorString(ret));
       //Serial.println("Retrying MQTT connection in 1 second...");
       mqtt.disconnect();
       delay(1000);
       retries--;
       if (retries == 0)  // if can't connect, go to deep sleep
         deepSleep();
  }
  //Serial.println("MQTT Connected!");
}
