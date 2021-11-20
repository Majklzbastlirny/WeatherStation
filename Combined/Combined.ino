/*
Michal Basler E4
Hlavní kód pro meteostanici.
Dodatky zde: https://github.com/Majklzbastlirny/WeatherStation
https://youtu.be/LY-1DHTxRAk?t=355
*/

/************************* Nutné knihovny ************************************/
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include <WiFi.h>
#include <BH1750.h>
#include <Wire.h>
#include "DHT.h"
#include <SFE_BMP180.h>

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
Adafruit_MQTT_Publish WV = Adafruit_MQTT_Publish(&mqtt, "weatherStation/WV");
Adafruit_MQTT_Publish WS = Adafruit_MQTT_Publish(&mqtt, "weatherStation/Speed");

/******************* Globální proměnné, definice a objekty **************************************/
//Seriový výstup zapnut
//#define Sprintln(a) (Serial.println(a))
//#define Sprint(a) (Serial.print(a))

//Seriový výstup vypnut
#define Sprintln(a)
#define Sprint(a)


#define DOBA_HIBERNACE 10 //v sekundách
#define LEDDIAG 2
int rescnt = 0;
uint32_t x=0;

//Proměnné ke korouhvi
bool D1 = 0;  
bool D2 = 0;
bool D3 = 0;
bool D4 = 0;
bool D5 = 0;
float wv = 0;

//Proměnné k anemometru
float ws = 1; //V M/S

//Proměnné k senzoru intenzity UV zařízení
int UVOUT = 34; 
int REF_3V3 = 35; 

//Proměnné k senzoru vlhkosti a teploty DHT22
float hd = 0;
float td = 0;
#define DHTPIN 4  
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE); 

//Proměnné k senzoru světla BH1750
float lux = 0; 
BH1750 lightMeter;

//Proměnné k senzoru teploty a tlaku BMP180
SFE_BMP180 pressure;
#define NadmVys 333

/*************************** Vlastní kód ************************************/



void setup() {
Serial.begin(115200);
/******** Nastavení pinů ***********/
pinMode(LEDDIAG, OUTPUT);  

//Nastavení pinů pro UV senzor
pinMode(UVOUT, INPUT);
pinMode(REF_3V3, INPUT);

//Nastavení pinů
pinMode(27, INPUT_PULLDOWN);
pinMode(26, INPUT_PULLDOWN);
pinMode(25, INPUT_PULLDOWN);
pinMode(14, INPUT_PULLDOWN);
pinMode(12, INPUT_PULLDOWN);

D5 = digitalRead(12);
D4 = digitalRead(13);
D3 = digitalRead(25);
D2 = digitalRead(26);
D1 = digitalRead(27);

//Připojení k WiFi
  Sprintln();
  Sprint("Připojuji se k: ");
  Sprintln(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Sprint(".");
  }
  Sprintln();

  Sprintln("Uspěšně připojeno");
  Sprintln("Moje IP adresa: "); Sprintln(WiFi.localIP());
  long rssi = WiFi.RSSI();
  Sprint("Síla WiFi signálu je: ");
  Sprint(rssi);
  Sprintln(" dBm");




  
  lightMeter.begin();
  delay(10);
 dht.begin(); 
 delay(250);
hd = dht.readHumidity();
  // Read temperature as Celsius (the default)
td = dht.readTemperature();



if (isnan(hd) || isnan(td)) {
    Sprintln(F("Failed to read from DHT sensor!"));
    return;
  }
  lux = lightMeter.readLightLevel();





  
 if (pressure.begin()) //If initialization was successful, continue
    Sprintln("BMP180 init success");
  else 
  {
    Sprintln("BMP180 init fail");
    ESP.restart();
  }
  

  
  // Setup MQTT subscription for onoff feed.
  
}


void loop() {
  Sprintln("");
  Sprintln("");

  
digitalWrite(LEDDIAG, LOW);  
 MQTT_connect();
  
char status;
  double T, P, p0; //Creating variables for temp, pressure and relative pressure

  Sprint("Nadmořská výška stanice: ");
  Sprint(NadmVys);
  Sprintln(" m.n.m.");

  status = pressure.startTemperature();
  if (status != 0) {
    delay(status);

    status = pressure.getTemperature(T);
    if (status != 0) {
      Sprint("Teplota: ");
      Sprint(T);
      Sprintln(" °C");

      status = pressure.startPressure(3);

      if (status != 0) {
        delay(status);

        status = pressure.getPressure(P, T);
        if (status != 0) {
          Sprint("Absolutní tlak: ");
          Sprint(P);
          Sprintln(" hPa");

          p0 = pressure.sealevel(P, NadmVys);
          Sprint("Relativní tlak: ");
          Sprint(p0);
          Sprintln(" hPa");
        }
      }
    }
  }  
  delay(1);
lux = lightMeter.readLightLevel();

if (D1 == 0 && D2 == 0 && D3 == 0 && D4 == 0 && D5 == 0) {
  wv = 0;
  }
else if (D1 == 1 && D2 == 0 && D3 == 0 && D4 == 0 && D5 == 0) {
  wv = 11.25;
  }
else if (D1 == 1 && D2 == 1 && D3 == 0 && D4 == 0 && D5 == 0) {
  wv = 22.5;
  }
else if (D1 == 0 && D2 == 1 && D3 == 0 && D4 == 0 && D5 == 0) {
  wv = 33.75;
  }
else if (D1 == 0 && D2 == 1 && D3 == 1 && D4 == 0 && D5 == 0) {
  wv = 45;
  }
else if (D1 == 1 && D2 == 1 && D3 == 1 && D4 == 0 && D5 == 0) {
  wv = 56.25;
  }
else if (D1 == 1 && D2 == 0 && D3 == 1 && D4 == 0 && D5 == 0) {
  wv = 67.5;
  }
else if (D1 == 0 && D2 == 0 && D3 == 1 && D4 == 0 && D5 == 0) {
  wv = 78.75;
  }
else if (D1 == 0 && D2 == 0 && D3 == 1 && D4 == 1 && D5 == 0) {
  wv = 90;
  }
else if (D1 == 1 && D2 == 0 && D3 == 1 && D4 == 1 && D5 == 0) {
  wv = 101.25;
  }
else if (D1 == 1 && D2 == 1 && D3 == 1 && D4 == 1 && D5 == 0) {
  wv = 112.5;
  }
else if (D1 == 0 && D2 == 1 && D3 == 1 && D4 == 1 && D5 == 0) {
  wv = 123.75;
  }
else if (D1 == 0 && D2 == 1 && D3 == 0 && D4 == 1 && D5 == 0) {
  wv = 135;
  }
else if (D1 == 1 && D2 == 1 && D3 == 0 && D4 == 1 && D5 == 0) {
  wv = 146.25;//not
  }
else if (D1 == 1 && D2 == 0 && D3 == 0 && D4 == 1 && D5 == 0) {
  wv = 157.5;//not
  }
else if (D1 == 0 && D2 == 0 && D3 == 0 && D4 == 1 && D5 == 0) {//15
  wv = 168.75;
  }
else if (D1 == 0 && D2 == 0 && D3 == 0 && D4 == 1 && D5 == 1) {
  wv = 180;
  }
else if (D1 == 1 && D2 == 0 && D3 == 0 && D4 == 1 && D5 == 1) {
  wv = 191.25;//not
  }
else if (D1 == 1 && D2 == 1 && D3 == 0 && D4 == 1 && D5 == 1) {
  wv = 202.5;//not
  }
else if (D1 == 0 && D2 == 1 && D3 == 0 && D4 == 1 && D5 == 1) {
  wv = 213.75;
  }
else if (D1 == 0 && D2 == 1 && D3 == 1 && D4 == 1 && D5 == 1) {
  wv = 225;
  }
else if (D1 == 1 && D2 == 1 && D3 == 1 && D4 == 1 && D5 == 1) {
  wv = 236.25;//not
  }
else if (D1 == 1 && D2 == 0 && D3 == 1 && D4 == 1 && D5 == 1) {
  wv = 247.5;//not
  }
else if (D1 == 0 && D2 == 0 && D3 == 1 && D4 == 1 && D5 == 1) {
  wv = 258.75;
  }
else if (D1 == 0 && D2 == 0 && D3 == 1 && D4 == 0 && D5 == 1) {
  wv = 270;
  }
else if (D1 == 1 && D2 == 0 && D3 == 1 && D4 == 0 && D5 == 1) {
  wv = 281.25;
  }
else if (D1 == 1 && D2 == 1 && D3 == 1 && D4 == 0 && D5 == 1) {
  wv = 292.5;
  }
else if (D1 == 0 && D2 == 1 && D3 == 1 && D4 == 0 && D5 == 1) {
  wv = 303.75;
  }
else if (D1 == 0 && D2 == 1 && D3 == 0 && D4 == 0 && D5 == 1) {
  wv = 315;
  }
else if (D1 == 1 && D2 == 1 && D3 == 0 && D4 == 0 && D5 == 1) {
  wv = 326.25;
  }
else if (D1 == 1 && D2 == 0 && D3 == 0 && D4 == 0 && D5 == 1) {
  wv = 337.5;
  }
else if (D1 == 0 && D2 == 0 && D3 == 0 && D4 == 0 && D5 == 1) {
  wv = 348.75;
  }

else {
  wv=0;
  }
Sprint("Směr větru je: ");  
Sprint(wv);
Sprint("°   ");
//Sprintln(D1, D2, D3, D4, D5);
Sprint(D1);
Sprint(" ");
Sprint(D2);
Sprint(" ");
Sprint(D3);
Sprint(" ");
Sprint(D4);
Sprint(" ");
Sprint(D5);
Sprintln("");




 
 


  Sprint(F("Vlhkost: "));
  Sprint(hd);
  Sprintln("%");
  Sprint(F("Teplota DHT22: "));
  Sprint(td);
  Sprintln(F(" °C "));
 

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
  
 

  Sprint("UV intenzita (mW/cm^2): ");
  Sprint(uvIntensity);
  Sprintln("");
  Sprintln("");


  delay(100);  



/************** Poslání dat přes MQTT *******************/  
  Sprintln(F("Probíhá odesílání dat na server"));
  Sprint(F("Probíhá odesílání teploty:"));
  if (! temperature.publish(T)) {
    Sprintln(F(" Failed"));
  } else {
    Sprintln(F(" OK!"));
  }
delay(150); 

  Sprint(F("Probíhá odesílání vlhkosti:"));
  if (! humidity.publish(hd)) {
    Sprintln(F(" Failed"));
  } else {
    Sprintln(F(" OK!"));
  }
delay(150);

  Sprint(F("Probíhá odesílání světelnosti:"));
if (! light.publish(lux)) {
    Sprintln(F(" Failed"));
  } else {
    Sprintln(F(" OK!"));
  }
delay(150);

  Sprint(F("Probíhá odesílání tlaku:"));
 if (! presss.publish(p0)) {
    Sprintln(F(" Failed"));
  } else {
    Sprintln(F(" OK!"));
  } 
delay(150);

  Sprint(F("Probíhá odesílání UV intenzity:"));
 if (! UV.publish(uvIntensity)) {
    Sprintln(F(" Failed"));
  } else {
    Sprintln(F(" OK!"));
  }  
delay(150);

  Sprint(F("Probíhá odesílání směru větru:"));
 if (! WV.publish(wv)) {
    Sprintln(F(" Failed"));
  } else {
    Sprintln(F(" OK!"));
  }  
delay(150);

Sprint(F("Probíhá odesílání rychlosti větru:"));
 if (! WS.publish(ws)) {
    Sprintln(F(" Failed"));
  } else {
    Sprintln(F(" OK!"));
  }  
delay(150);
 
rescnt++;
if (rescnt == 10) {
  //ESP.restart();
  Hibernace();
} else {
  
  Sprint("Počet provedených měření od restartu: ");
  Sprintln(rescnt);
 
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

  Sprintln("Připojuji se k MQTT serveru ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Sprintln(mqtt.connectErrorString(ret));
       Sprintln("Nepodařilo se mi připojit k MQTT serveru. Zkusím to znovu za 5 sekund.");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for user to reset me
         ESP.restart();
       }
  }
  Sprintln("Úspěšně připojeno!");
}

void Hibernace(){
  Sprintln("Jdu do režimu hibernace");
  esp_sleep_enable_timer_wakeup(DOBA_HIBERNACE * 1000000);
  esp_deep_sleep_start();
  
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
