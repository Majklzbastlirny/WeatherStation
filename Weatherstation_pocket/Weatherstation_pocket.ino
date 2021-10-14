#include <ESP8266WiFi.h>
#include "DHT.h"
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"



// -------- MQTT broker ---------- //
#define HOST        "192.168.0.8"
#define PORT        1883
#define USERNAME    "admin"
#define PASSWORD    "123456780"

// ---------- DHT -------- //
// capacitive humidity and thermistor
#define DHTPIN 0 //pin on ESP8266 board
DHT dht(DHTPIN, DHT11); // dht object of DHT class

// --------- Time ------------- //
const int intervall = 60000;  //milliseconds to sleep
const int timeout = 200;  //time out loop count

// ---------- WLAN ------------ //
#define WLAN_SSID   "Edma"
#define WLAN_PASS   "pes_Fido"

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, HOST, PORT, USERNAME, PASSWORD);
Adafruit_MQTT_Publish temperature = Adafruit_MQTT_Publish(&mqtt, "weatherStation/temp1");
Adafruit_MQTT_Publish humidity = Adafruit_MQTT_Publish(&mqtt, "weatherStation/hum1");

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

  dht.begin();

  MQTT_connect();
  delay(100);
  temperature.publish(dht.readTemperature()-2);
  delay(100);
  humidity.publish(dht.readHumidity());
  delay(250);

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
