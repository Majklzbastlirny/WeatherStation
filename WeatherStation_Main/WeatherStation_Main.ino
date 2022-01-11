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
#include <Adafruit_BMP280.h>

/************************* WiFi Access Point *********************************/

#define WLAN_SSID         "Edma_UBNT"
#define WLAN_PASS         "pes_Fido"


/************************* MQTT Broker Setup *********************************/

#define MQTT_BROKER_IP    "bladyhel.serveminecraft.net"
#define MQTT_BROKER_PORT  21    //default port is 1883
#define MQTT_USERNAME     "admin"
#define MQTT_PASSWORD     "123456780"

/************ Global State (you don't need to change this!) ******************/

WiFiClient client;

Adafruit_MQTT_Client mqtt(&client, MQTT_BROKER_IP, MQTT_BROKER_PORT, MQTT_USERNAME, MQTT_PASSWORD);

/************************* MQTT Topics ***************************************/

Adafruit_MQTT_Publish temperature = Adafruit_MQTT_Publish(&mqtt, "WeatherStation/Temperature");
Adafruit_MQTT_Publish temperature2 = Adafruit_MQTT_Publish(&mqtt, "WeatherStation/Temperature2");
Adafruit_MQTT_Publish humidity = Adafruit_MQTT_Publish(&mqtt, "WeatherStation/Humidity");
Adafruit_MQTT_Publish light = Adafruit_MQTT_Publish(&mqtt, "WeatherStation/Light");
Adafruit_MQTT_Publish presss = Adafruit_MQTT_Publish(&mqtt, "WeatherStation/Pressure");
Adafruit_MQTT_Publish UV = Adafruit_MQTT_Publish(&mqtt, "WeatherStation/UV");
Adafruit_MQTT_Publish WV = Adafruit_MQTT_Publish(&mqtt, "WeatherStation/WV");
Adafruit_MQTT_Publish WS = Adafruit_MQTT_Publish(&mqtt, "WeatherStation/Speed");
Adafruit_MQTT_Publish BatVoltage = Adafruit_MQTT_Publish(&mqtt, "WeatherStation/Voltage");

/******************* Globální proměnné, definice a objekty **************************************/
//Seriový výstup zapnut
#define Sprintln(a) (Serial.println(a))
#define Sprint(a) (Serial.print(a))

//Seriový výstup vypnut
//#define Sprintln(a)
//#define Sprint(a)


#define DOBA_HIBERNACE 10 //v sekundách
#define mezera 10000 //mezera mezi měřením (v ms)
#define LEDDIAG 2
int rescnt = 0;


//Proměnné ke korouhvi
bool D1 = 0;  
bool D2 = 0;
bool D3 = 0;
bool D4 = 0;
bool D5 = 0;
float wv = 0;

//Proměnné k anemometru
float ws = 0; //V M/S
#define AnemoPIN  35

float AnemoTime = 1000; //doba měření rychlosti v ms
byte pulses = 0;
 
//Proměnné k senzoru intenzity UV zařízení
int UVOUT = 33; 
int REF_3V3 = 34; 
uint32_t x=0;

//Proměnné k senzoru vlhkosti a teploty DHT22
float hd = 0;
float td = 0;
#define DHTPIN 23 //4
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE); 

//Proměnné k senzoru světla BH1750
float lux = 0; 
BH1750 lightMeter;

//Proměnné k senzoru teploty a tlaku BMP180
Adafruit_BMP280 bmp;
#define NadmVys 333

//Proměnné k vyhřívacímu systému
RTC_DATA_ATTR bool HeatOn = 0;
int MinTemp = 3;
#define HeatPin 4

//Proměnné k senzoru napětí
#define voltmeas 32
float voltage = 0;

//Proměnné k odpojovači senzorů
#define SensorPWR 25
int wificount = 0;
/*************************** Vlastní kód ************************************/



void setup() {
Wire.begin();
Wire.setClock(7500);
Serial.begin(115200);
/******** Nastavení pinů ***********/
pinMode(LEDDIAG, OUTPUT);  
pinMode(SensorPWR, OUTPUT);
delay(25);
digitalWrite(SensorPWR, HIGH);
//Nastavení pinů pro UV senzor
pinMode(UVOUT, INPUT);
pinMode(REF_3V3, INPUT);

//Nastavení pinů směrovky větru
pinMode(19, INPUT_PULLDOWN);
pinMode(18, INPUT_PULLDOWN);
pinMode(5, INPUT_PULLDOWN);
pinMode(17, INPUT_PULLDOWN);
pinMode(16, INPUT_PULLDOWN);

pinMode(HeatPin, OUTPUT);
digitalWrite(HeatPin, LOW);
//Připojení k WiFi
  Sprintln();
  Sprint("Připojuji se k: ");
  Sprintln(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Sprint(".");
    wificount++;
/*    if (wificount = 10){
      digitalWrite(SensorPWR, LOW);
      ESP.restart();
      }
    else {}
*/
    
  }
  Sprintln();

  Sprintln("Uspěšně připojeno");
  Sprintln("Moje IP adresa: "); Sprintln(WiFi.localIP());
  long rssi = WiFi.RSSI();
  Sprint("Síla WiFi signálu je: ");
  Sprint(rssi);
  Sprintln(" dBm");




  unsigned status;
  status = bmp.begin(0x76, 0x58);
  lightMeter.begin();
  delay(10);
 dht.begin(); 
 delay(250);


 
  






  

  
 
  
}


void loop() {
digitalWrite(SensorPWR, HIGH);  
  Sprintln("");
  Sprintln("");
digitalWrite(LEDDIAG, LOW);  
 MQTT_connect();


D5 = digitalRead(19);
D4 = digitalRead(18);
D3 = digitalRead(5);
D2 = digitalRead(17);
D1 = digitalRead(16);

voltage = (((analogRead(voltmeas)*8.158)/4095)+0.13);
 Sprint("Napětí na baterce: ");
 Sprint(voltage);
 Sprintln(" V");
delay(10);

lux = lightMeter.readLightLevel();
Sprint("Světelnost: ");
Sprint(lux);
Sprintln(" lux");
char status;
  double T, P, p0; //Creating variables for temp, pressure and relative pressure

  Sprint("Nadmořská výška stanice: ");
  Sprint(NadmVys);
  Sprintln(" m.n.m.");


Sprint(F("Teplota: "));
    T = bmp.readTemperature();
    Sprint(T);
    Sprintln(" *C");

    Sprint(F("Absolutní tlak: "));
    P = (bmp.readPressure()/100);
    Sprint(P);
    Sprintln(" hPa");

    Sprint(F("Relativní tlak: "));
    delay(5);
    p0 = (((bmp.readPressure())/pow((1-((float)(NadmVys))/44330), 5.255))/100.0);
    delay(5);
    if (p0 < 950) {
      delay(100);
      bmp.readPressure();
      p0 = (((bmp.readPressure())/pow((1-((float)(NadmVys))/44330), 5.255))/100.0);
      delay(10);
      }
//    else{}

    Sprint(p0);
    Sprintln(" hPa");

    

  delay(1);

if (T < MinTemp) {HeatOn = 1;}
else if (T >= MinTemp) {HeatOn = 0;}
else {}

pinMode(AnemoPIN, INPUT);
attachInterrupt(AnemoPIN, ISR, FALLING);
delay(AnemoTime);

detachInterrupt(AnemoPIN);
ws = 2*PI*0.08*(pulses/AnemoTime/2*1000);
Sprint("Rychlost větru: ");
Sprint(ws);
Sprintln(" m/s");


if (D1 ==  1 && D2 == 0  && D3 == 0 && D4 == 0 && D5 == 0 ) { wv = 0; }
else if (D1 ==  1 && D2 == 1  && D3 == 0 && D4 == 0 && D5 == 0 ) { wv = 12; }
else if (D1 ==  1 && D2 == 1  && D3 == 0 && D4 == 1 && D5 == 0 ) { wv = 24; }
else if (D1 ==  1 && D2 == 1  && D3 == 0 && D4 == 1 && D5 == 1 ) { wv = 36; }
else if (D1 ==  1 && D2 == 1  && D3 == 0 && D4 == 0 && D5 == 1 ) { wv = 48; }
else if (D1 ==  0 && D2 == 1  && D3 == 0 && D4 == 0 && D5 == 1 ) { wv = 60; }
else if (D1 ==  0 && D2 == 1  && D3 == 0 && D4 == 0 && D5 == 0 ) { wv = 72; }
else if (D1 ==  0 && D2 == 1  && D3 == 1 && D4 == 0 && D5 == 0 ) { wv = 84; }
else if (D1 ==  0 && D2 == 1  && D3 == 1 && D4 == 0 && D5 == 1 ) { wv = 96; }
else if (D1 ==  1 && D2 == 1  && D3 == 1 && D4 == 0 && D5 == 1 ) { wv = 108; }
else if (D1 ==  1 && D2 == 1  && D3 == 1 && D4 == 0 && D5 == 0 ) { wv = 120; }
else if (D1 ==  1 && D2 == 0  && D3 == 1 && D4 == 0 && D5 == 0 ) { wv = 132; }
else if (D1 ==  0 && D2 == 0  && D3 == 1 && D4 == 0 && D5 == 0 ) { wv = 144; }
else if (D1 ==  0 && D2 == 0  && D3 == 1 && D4 == 1 && D5 == 0 ) { wv = 156; }
else if (D1 ==  1 && D2 == 0  && D3 == 1 && D4 == 1 && D5 == 0 ) { wv = 168; }
else if (D1 ==  1 && D2 == 1  && D3 == 1 && D4 == 1 && D5 == 0 ) { wv = 180; }
else if (D1 ==  0 && D2 == 1  && D3 == 1 && D4 == 1 && D5 == 0 ) { wv = 192; }
else if (D1 ==  0 && D2 == 1  && D3 == 0 && D4 == 1 && D5 == 0 ) { wv = 204; }
else if (D1 ==  0 && D2 == 0  && D3 == 0 && D4 == 1 && D5 == 0 ) { wv = 216; }
else if (D1 ==  0 && D2 == 0  && D3 == 0 && D4 == 1 && D5 == 1 ) { wv = 228; }
else if (D1 ==  0 && D2 == 1  && D3 == 0 && D4 == 1 && D5 == 1 ) { wv = 240; }
else if (D1 ==  0 && D2 == 1  && D3 == 1 && D4 == 1 && D5 == 1 ) { wv = 252; }
else if (D1 ==  0 && D2 == 0  && D3 == 1 && D4 == 1 && D5 == 1 ) { wv = 264; }
else if (D1 ==  0 && D2 == 0  && D3 == 1 && D4 == 0 && D5 == 1 ) { wv = 276; }
else if (D1 ==  0 && D2 == 0  && D3 == 0 && D4 == 0 && D5 == 1 ) { wv = 288; }
else if (D1 ==  1 && D2 == 0  && D3 == 0 && D4 == 0 && D5 == 1 ) { wv = 300; }
else if (D1 ==  1 && D2 == 0  && D3 == 1 && D4 == 0 && D5 == 1 ) { wv = 312; }
else if (D1 ==  1 && D2 == 0  && D3 == 1 && D4 == 1 && D5 == 1 ) { wv = 324; }
else if (D1 ==  1 && D2 == 0  && D3 == 0 && D4 == 1 && D5 == 1 ) { wv = 336; }
else if (D1 ==  1 && D2 == 0  && D3 == 0 && D4 == 1 && D5 == 0 ) { wv = 348; }


else {
  wv=0;
  }
Sprint("Směr větru je: ");  
Sprint(wv);
Sprint("°   ");
//Sprintln(D1, D2, D3, D4, D5);
Sprint(D5);
Sprint(" ");
Sprint(D4);
Sprint(" ");
Sprint(D3);
Sprint(" ");
Sprint(D2);
Sprint(" ");
Sprint(D1);
Sprintln("");

hd = dht.readHumidity();
  
td = dht.readTemperature();


if (isnan(hd) || isnan(td)) {
    Sprintln(F("Failed to read from DHT sensor!"));
 //   return;
  }

Sprint(F("Vlhkost: "));
  Sprint(hd);
  Sprintln("%");
  Sprint(F("Teplota DHT22: "));
  Sprint(td);
  Sprintln(F(" °C "));

 int uvLevel = averageAnalogRead(UVOUT);
  int refLevel = averageAnalogRead(REF_3V3);

  //Use the 3.3V power pin as a reference to get a very accurate output value from sensor
  float outputVoltage = 3.3 / 4095 * uvLevel;   // adjust outputVoltage to actual voltage in case you read negative values.

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


digitalWrite(SensorPWR, LOW);
/************** Poslání dat přes MQTT *******************/  
  Sprintln(F("Probíhá odesílání dat na server"));
  Sprint(F("Probíhá odesílání teploty:"));
  if (! temperature.publish(T)) {
    Sprintln(F(" Failed"));
  } else {
    Sprintln(F(" OK!"));
  }
delay(150); 

 Sprint(F("Probíhá odesílání teploty (záloha):"));
  if (! temperature2.publish(td)) {
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
 
Sprint(F("Probíhá odesílání napětí baterie:"));
 if (! BatVoltage.publish(voltage)) {
    Sprintln(F(" Failed"));
  } else {
    Sprintln(F(" OK!"));
  }    
delay(150);
 
rescnt++;
if (rescnt == 6) {
  //ESP.restart();
  Hibernace();
} else {
  
  Sprint("Počet provedených měření od restartu: ");
  Sprintln(rescnt);
 
}
delay(mezera);
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

void ISR(){
    pulses++;
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
