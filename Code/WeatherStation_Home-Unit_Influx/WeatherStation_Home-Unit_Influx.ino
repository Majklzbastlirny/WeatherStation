
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
#include <Fonts/FreeSans9pt7b.h>

//#define INFLUXDB_URL "http://192.168.0.8:8086"
#define INFLUXDB_URL "http://bladyhel.serveminecraft.net:8086"
#define INFLUXDB_TOKEN "H4hYgXBEUTFRivBBLdo0Pk_ca1qCAmN8O-LRGjRLRoVitVGcSfervwQQOKa8Jka7rwDa2m7EDlqZn_MmNqsezQ=="
#define INFLUXDB_ORG "2d5652347a7565e5"
#define INFLUXDB_BUCKET "WeatherStation"
String query = "from(bucket: \"" INFLUXDB_BUCKET "\") |> range(start: -1h) |> last() ";

double SleepTime = 20; //v sekundách

// Set timezone string according to https://www.gnu.org/software/libc/manual/html_node/TZ-Variable.html
#define TZ_INFO "CET-1CEST,M3.5.0,M10.5.0/3"

// InfluxDB client instance with preconfigured InfluxCloud certificate
InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);

double dewpoint = 0;
double heatindex = 0;
bool heaterstatus = 0;
double humidity = 0;
double light = 0;
double precipitation = 0;
double pressure = 0;
double pressureraw = 0;
double wspeed = 0;
double temperature = 0;
double temperature2 = 0;
double uv = 0;
double voltage = 0;
byte winddir = 0;
double windchill = 0;

char* winddirection = 0;

short Year = 0;
short Month = 0;
short Day = 0;
short WeekDay = 0;
short Hour = 0;
short Minute = 0;
short Second = 0;
char Seconds[16];
char Minutes[16];
char Hours[16];

const char *ssid     = "MediumRecords";
const char *password = "123456780";
short wificount = 0;

char daysOfTheWeek[7][12] = {"Nedele", "Pondeli", "Utery", "Streda", "Ctvrtek", "Patek", "Sobota"};
char months[12][12] = {"Prosinec", "Leden", "Unor", "Brezen", "Duben", "Kveten", "Cerven", "Cervenec", "Srpen", "Zari", "Rijen", "Listopad"};


#define i2c_Address 0x3c //initialize with the I2C addr 0x3C Typically eBay OLED's

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1   //   QT-PY / XIAO
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


Adafruit_BMP280 bme;
void setup()   {

  Serial.begin(115200);
  Serial.println();

  display.begin(i2c_Address, true); // Address 0x3C default
  if (!bme.begin(0x76, 0x58)) {
    Serial.println("Could not find a valid BMP280 sensor, check wiring!");
    while (1);
  }
  display.clearDisplay();
  display.display();

  WiFi.begin(ssid, password);

  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 400 );
    Serial.print ( "." );
    wificount++;
    delay(100);
    if (wificount < 40) {}
    else {
      ESP.restart();
    }
  }
  Serial.println();



  Serial.println(bme.readPressure());
  Serial.println(bme.readTemperature());



  timeSync(TZ_INFO, "pool.ntp.org", "time.nis.gov");


  // Show time
  time_t tnow = time(nullptr);
  Serial.print("Čas nyní: ");
  Serial.println(ctime(&tnow));

  time_t now;
  struct tm * timeinfo;
  time(&now);
  timeinfo = localtime(&now);
  Year  = ((timeinfo->tm_year) + 1900);
  Month = ((timeinfo->tm_mon) + 1);
  Day = (timeinfo->tm_mday);
  WeekDay = (timeinfo->tm_wday);
  Hour = (timeinfo->tm_hour);
  Minute = (timeinfo->tm_min);
  Second = (timeinfo->tm_sec);
  Serial.println(Year);
  Serial.println(Month);
  Serial.println(Day);
  Serial.println(WeekDay);
  Serial.println(Hour);
  Serial.println(Minute);
  Serial.println(Second);

  Get_Data();
  WiFi.disconnect();
  WiFi.forceSleepBegin();
  delay(1);
  display.setFont();
  display.setTextColor(SH110X_WHITE);
  display.clearDisplay();
  // display.drawLine(28, 0, 28, 64, SH110X_WHITE);
  //display.drawLine(100, 0, 100, 64, SH110X_WHITE);
  char bufferdate[32];
  sprintf(bufferdate, "%s, %d.%d. %d", daysOfTheWeek[WeekDay], Day, Month, Year);
  drawCentreString(bufferdate, 75, 0);

  display.setFont(&FreeSans9pt7b);
  char buffertime[32];
  sprintf(Hours, "%02i", Hour);
  sprintf(Minutes, "%02i", Minute);
  sprintf(Seconds, "%02i", Second);
  sprintf(buffertime, "%s : %s : %s", Hours, Minutes, Seconds);
  //display.print(buffertime);
  drawCentreString(buffertime, 80, 25);
  display.display();

  delay(5000);
  display.clearDisplay();
  display.setFont(&FreeSans9pt7b);
  display.setCursor(0, 15);
  display.println("Teplota");
  display.setCursor(0, 40);
  display.setFont();
  display.println("Doma            Venku");
  display.setFont(&FreeSans9pt7b);
  display.setCursor(0, 62);
  display.print(bme.readTemperature());
  display.drawCircle(48, 50, 1, SH110X_WHITE);

  display.setCursor(90, 62);
  display.print(temperature);
  display.drawCircle(126, 50, 1, SH110X_WHITE);
  display.display();

  delay(5000);
  display.clearDisplay();
  display.setFont(&FreeSans9pt7b);
  display.setCursor(0, 15);
  display.print("Teploty");
  display.setFont();

  display.setCursor(0, 29);
  display.print("Pocitova:      ");
  display.print(heatindex);
  display.drawCircle(126, 30, 1, SH110X_WHITE);

  display.setCursor(0, 39);
  display.print("Vetrny chlad:  ");

  display.print(windchill);
  display.drawCircle(126, 40, 1, SH110X_WHITE);

  display.setCursor(0, 49);
  display.print("Rosny bod:     ");

  display.print(dewpoint);
  display.drawCircle(126, 50, 1, SH110X_WHITE);



  display.display();


  delay(5000);
  display.clearDisplay();
  display.setFont(&FreeSans9pt7b);
  display.setCursor(0, 15);
  display.println("Tlak");
  display.setCursor(0, 40);
  display.setFont();
  display.println("Absolutni   Relativni");

  display.setFont(&FreeSans9pt7b);
  display.setCursor(0, 62);
  display.print(pressureraw);

  display.setCursor(70, 62);
  display.print(pressure);
  display.display();

  delay(5000);
  display.clearDisplay();
  display.setFont(&FreeSans9pt7b);
  display.setCursor(0, 15);
  display.println("Svetlo");
  display.setCursor(0, 40);
  display.setFont();
  display.println("Svetlo   UV intenzita");
  display.setFont(&FreeSans9pt7b);
  display.setCursor(0, 62);
  display.print(light);
  //display.print(lx);
  display.setCursor(70, 62);
  display.print(uv);
  //  display.print(mW/cm2);
  display.display();


  delay(5000);
  display.clearDisplay();
  display.setFont(&FreeSans9pt7b);
  display.setCursor(0, 15);
  display.println("Vitr");
  display.setCursor(0, 40);
  display.setFont();
  display.println("Rychlos  Smer (odkud)");
  display.setFont(&FreeSans9pt7b);
  display.setCursor(0, 62);
  display.print(wspeed);
  display.print(" m/s");


  display.setCursor(90, 62);
  display.print(winddirection);
  //display.print(winddir);
  //display.drawCircle(126, 50, 1, SH110X_WHITE);
  display.display();


  delay(5000);
  display.clearDisplay();
  display.setFont(&FreeSans9pt7b);
  display.setCursor(0, 15);
  display.print("Vlhkost:");
  display.setCursor(110, 15);
  display.print("%");
  display.setCursor(65, 15);
  display.print(humidity);
  display.setCursor(0, 35);
  display.print("Srazky: ");
  display.print(precipitation);
  display.setCursor(102, 35);
  display.setFont();
  display.print("mm/h");
  display.display();
  delay(5000);
  display.clearDisplay();
  display.display();



  ESP.deepSleep(SleepTime * 1000000);
}
void loop() {

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

    if (winddir > 22.5 && winddir < 67.5) {
      winddirection = "SV";
    }
    else if (winddir > 67.5 && winddir < 112.5) {
      winddirection = "V";
    }
    else if (winddir > 112.5 && winddir < 157.5) {
      winddirection = "JV";
    }
    else if (winddir > 157.5 && winddir < 202.5) {
      winddirection = "J";
    }
    else if (winddir > 202.5 && winddir < 247.5) {
      winddirection = "JZ";
    }
    else if (winddir > 247.5 && winddir < 292.5) {
      winddirection = "Z";
    }
    else if (winddir > 292.5 && winddir < 337.5) {
      winddirection = "SZ";
    }
    else {
      winddirection = "S";
    }
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

void drawCentreString(const char *buf, int x, int y)
{
  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(buf, x, y, &x1, &y1, &w, &h); //calc width of new string
  display.setCursor(x - w / 2, y);
  display.print(buf);
}
