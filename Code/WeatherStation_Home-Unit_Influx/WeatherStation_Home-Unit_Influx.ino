
#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_Sensor.h>
#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>


#define INFLUXDB_URL "http://192.168.0.8:8086"
#define INFLUXDB_TOKEN "H4hYgXBEUTFRivBBLdo0Pk_ca1qCAmN8O-LRGjRLRoVitVGcSfervwQQOKa8Jka7rwDa2m7EDlqZn_MmNqsezQ=="
#define INFLUXDB_ORG "2d5652347a7565e5"
#define INFLUXDB_BUCKET "WeatherStation"
String query = "from(bucket: \"" INFLUXDB_BUCKET "\") |> range(start: -1h) |> last() ";

double SleepTime = 30; //v sekundách

// Set timezone string according to https://www.gnu.org/software/libc/manual/html_node/TZ-Variable.html
#define TZ_INFO "CET-1CEST,M3.5.0,M10.5.0/3"

// InfluxDB client instance with preconfigured InfluxCloud certificate
InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);

float dewpoint = 0;
float heatindex = 0;
bool heaterstatus = 0;
float humidity = 0;
float light = 0;
float precipitation = 0;
float pressure = 0;
float pressureraw = 0;
float wspeed = 0;
float temperature = 0;
float temperature2 = 0;
float uv = 0;
float voltage = 0;
float winddir = 0;
float windchill = 0;



const char *ssid     = "MediumRecords";
const char *password = "123456780";

const long utcOffsetInSeconds = 3600;

char daysOfTheWeek[7][12] = {"Nedele", "Pondeli", "Utery", "Streda", "Ctvrtek", "Patek", "Sobota"};
String months[12] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};


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

  Serial.begin(115200);


  WiFi.begin(ssid, password);

  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    Serial.print ( "." );
  }

  timeClient.begin();



  display.begin(i2c_Address, true); // Address 0x3C default
  // display.setContrast (50); // dim display
  if (!bme.begin(0x76, 0x58)) {
    Serial.println("Could not find a valid BMP280 sensor, check wiring!");
    while (1);
  }

  Serial.println(bme.readPressure());
  Serial.println(bme.readTemperature());

}


void loop() {
  timeClient.update();
  Get_Data();
  WiFi.disconnect();

  unsigned long epochTime = timeClient.getEpochTime();
  struct tm *ptm = gmtime ((time_t *)&epochTime);
  Serial.print("Epoch Time: ");
  Serial.println(epochTime);
  int monthDay = ptm->tm_mday;
  Serial.print("Month day: ");
  Serial.println(monthDay);

  int currentMonth = ptm->tm_mon + 1;
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
  display.setTextColor(SH110X_BLACK, SH110X_WHITE);
  display.print(monthDay);
  display.print(".");
  display.print(currentMonth);
  display.print(".");
  display.setTextColor(SH110X_WHITE);
  display.setCursor(80, 0);
  display.println(timeClient.getFormattedTime());
  display.drawLine(0, 8, display.width(), 8, SH110X_WHITE);
  display.setCursor(0, 10);
  display.setTextColor(SH110X_WHITE);
  display.print("Teplota venku: ");
  display.setTextColor(SH110X_WHITE); // 'inverted' text
  display.print(temperature);
  display.setCursor(122, 10);
  display.drawCircle(119, 11, 1, SH110X_WHITE);
  display.println("C");
  display.setCursor(0, 19);
  display.print("Vlhkost venku: ");
  display.setTextColor(SH110X_WHITE); // 'inverted' text
  display.setCursor(84, 19);
  display.print(humidity);
  display.setCursor(122, 19);
  display.println("%");
  display.setCursor(0, 28);
  display.print("Tlak venku: ");
  display.setTextColor(SH110X_WHITE); // 'inverted' text
  display.setCursor(66, 28);
  display.print(pressure);
  display.setCursor(104, 28);
  display.println(" hPa");
  display.setCursor(0, 37);
  display.print("Svetlo je: ");
  display.setTextColor(SH110X_WHITE); // 'inverted' text
  display.print(light);

  display.setCursor(104, 37);
  display.println(" lux");
  display.setCursor(0, 47);
  display.print("UV je: ");
  display.setTextColor(SH110X_WHITE); // 'inverted' text
  display.print(uv);
  display.setCursor(86, 47);
  display.println(" mW/cm2");

  display.setCursor(0, 57);
  display.print("Vitr: ");
  display.drawCircle(126, 57, 1, SH110X_WHITE);
  display.setTextColor(SH110X_WHITE); // 'inverted' text
  display.print(wspeed);
  //display.setCursor(100, 18);
  display.print(" m/s  ");
  display.println(winddir);
  //display.setCursor(100, 26);





  // display.drawLine(0, 63, display.width(), 63, SH110X_WHITE);

  display.display();
  delay(10000);

















  display.clearDisplay();
  display.setCursor(0, 0);
  display.print(daysOfTheWeek[timeClient.getDay()]);
  // display.print("Ctvrtek");
  display.setCursor(43, 0);
  display.setTextColor(SH110X_BLACK, SH110X_WHITE);
  display.print(monthDay);
  display.print(".");
  display.print(currentMonth);
  display.print(".");
  display.setTextColor(SH110X_WHITE);
  display.setCursor(80, 0);
  display.println(timeClient.getFormattedTime());
  display.drawLine(0, 8, display.width(), 8, SH110X_WHITE);
  display.drawLine(0, 63, display.width(), 63, SH110X_WHITE);
  display.setCursor(0, 10);



  display.setCursor(0, 10);
  display.print("Teplota doma: ");
  // display.setTextColor(SH110X_BLACK, SH110X_WHITE); // 'inverted' text
  display.print(bme.readTemperature());
  display.drawCircle(119, 11, 1, SH110X_WHITE);
  display.setCursor(122, 10);
  display.setTextColor(SH110X_WHITE);
  display.println("C");
  display.setCursor(0, 19);
  /*display.print("Vlhkost doma: ");

    display.print((char *)humidity.lastread);
    display.setCursor(122, 19);
    display.println("%");
  */
  display.setCursor(0, 28);
  display.print("Tlak doma: ");
  display.print(((bme.readPressure()) / pow((1 - ((float)(333)) / 44330), 5.255)) / 100.0);
  display.setCursor(104, 28);
  display.println(" hPa");

  display.display();
  Serial.println("Jdu spát");
  ESP.deepSleep(SleepTime * 1000000);










}










void Get_Data() {
  FluxQueryResult result = client.query(query);

  // Iterate over rows. Even there is just one row, next() must be called at least once.
  while (result.next()) {
    // Get converted value for flux result column 'SSID'
    String meas = result.getValueByName("_measurement").getString();
    // Serial.print("");
    Serial.print(meas);

    Serial.print(" je ");

    double value = result.getValueByName("_value").getDouble();
    Serial.print(value, 3);


    if (meas == "DewPoint") {
      dewpoint = value;
    }
    else if (meas == "HeatIndex") {
      heatindex = value;
    }
    else if (meas == "HeaterStatus") {
      heaterstatus = value;
    }
    else if (meas == "Humidity") {
      humidity = value;
    }
    else if (meas == "Light") {
      light = value;
    }
    else if (meas == "Precipitation") {
      precipitation = value;
    }
    else if (meas == "Pressure") {
      pressure = value;
    }
    else if (meas == "PressureRAW") {
      pressureraw = value;
    }
    else if (meas == "Speed") {
      wspeed = value;
    }
    else if (meas == "Temperature") {
      temperature = value;
    }
    else if (meas == "Temperature2") {
      temperature2 = value;
    }
    else if (meas == "UV") {
      uv = value;
    }
    else if (meas == "Voltage") {
      voltage = value;
    }
    else if (meas == "WindDir") {
      winddir = value;
    }
    else if (meas == "WindChill") {
      windchill = value;
    }
    else {}



    Serial.println();
  }

  // Check if there was an error
  if (result.getError() != "") {
    Serial.print("Query result error: ");
    Serial.println(result.getError());
  }

  // Close the result
  result.close();
}
