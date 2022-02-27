#include <WiFi.h>
#include <Arduino.h>
#include <esp_task_wdt.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "epd_driver.h"
#include "firasans.h"
#include "esp_adc_cal.h"
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include "logo.h"
#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>

#include <Adafruit_BME280.h>
#include <Adafruit_Sensor.h>


#define I2C_SDA 14
#define I2C_SCL 15
#define DOBA_HIBERNACE 60 //v sekundách


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
char DEWPOINT[64];
double heatindex = 0;
char HEATINDEX[64];
bool heaterstatus = 0;
char HEATERSTATUS[64];
double humidity = 0;
char HUMIDITY[64];
double light = 0;
char LIGHT[64];
double precipitation = 0;
char PRECIPITATION[64];
double pressure = 0;
char PRESSURE[64];
double pressureraw = 0;
char PRESSURERAW[64];
double wspeed = 0;
char WSPEED[64];
double temperature = 0;
char TEMPERATURE[64];
double temperature2 = 0;
char TEMPERATURE2[64];
double uv = 0;
char UV[64];
double voltage = 0;
char VOLTAGE[64];
byte winddir = 0;
char WINDDIR[64];
double windchill = 0;
char WINDCHILL[64];

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






TwoWire I2CBME = TwoWire(0);

Adafruit_BME280 bme;

#define BATT_PIN            36
#define SD_MISO             12
#define SD_MOSI             13
#define SD_SCLK             14
#define SD_CS               15

uint8_t *framebuffer;
int vref = 1100;

void setup()
{
  Serial.begin(115200);

  // Correct the ADC reference voltage
  Serial.println(" ");
  uint8_t percentage = 100;
  esp_adc_cal_characteristics_t adc_chars;
  esp_adc_cal_value_t val_type = esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 1100, &adc_chars);
  if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF) {
    Serial.printf("eFuse Vref:%u mV", adc_chars.vref);
    vref = adc_chars.vref;
  }
  float voltageb = analogRead(36) / 4096 * 6.566 * (vref / 1000);
  if (voltageb > 1 ) {
    Serial.println("\nvoltage = " + String(voltageb));
    percentage = 2836.9625 * pow(voltageb, 4) - 43987.4889 * pow(voltageb, 3) + 255233.8134 * pow(voltageb, 2) - 656689.7123 * voltageb + 632041.7303;
    if (voltageb >= 4.2) percentage = 100;
    if (voltageb <= 3.8) percentage = 0;

    Serial.print((String(percentage) + "%  " + String(voltageb, 1) + "v"));
  }
  Serial.println(" ");
  Serial.println(analogRead(36));
  Serial.print(voltageb);
  Serial.println(" V");
  Serial.print(percentage);
  Serial.println(" %");



  epd_init();

  framebuffer = (uint8_t *)ps_calloc(sizeof(uint8_t), EPD_WIDTH * EPD_HEIGHT / 2);
  if (!framebuffer) {
    Serial.println("alloc memory failed !!!");
    while (1);
  }
  memset(framebuffer, 0xFF, EPD_WIDTH * EPD_HEIGHT / 2);

  Rect_t area = {
    .x = 230,
    .y = 20,
    .width = logo_width,
    .height = logo_height,
  };

  epd_poweron();
  epd_clear();
  // epd_draw_grayscale_image(area, (uint8_t *)logo_data);
  epd_poweroff();


  I2CBME.begin(I2C_SDA, I2C_SCL, 9766);

  WiFi_Connect();
  Get_Data();
  WiFi_Disconnect();
  TranslateData();







  bool status;
  status = bme.begin(0x76, &I2CBME);
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }
  Serial.print(bme.readTemperature());
  Serial.println(" °C");
  Serial.print(bme.readPressure() / 100.0);
  Serial.println(" hPa");

  Serial.print(bme.readHumidity());
  Serial.println(" %");

  int cursor_x = 10;
  int cursor_y = 35;

  char temp[64];
  dtostrf(bme.readTemperature(), 5, 2, temp);

  char hum[64];
  dtostrf(bme.readHumidity(), 5, 2, hum);

  char pressraw[64];
  dtostrf((bme.readPressure() / 100), 6, 2, pressraw);



  epd_poweron();

  cursor_x = 100;
  cursor_y = 100;


  /*-----------------VENKU------------------*/
  cursor_x = 50;
  cursor_y = 55;
  writeln((GFXfont *)&FiraSans, "Venku", &cursor_x, &cursor_y, NULL);



  cursor_x = 100;
  cursor_y = 100;
  writeln((GFXfont *)&FiraSans, TEMPERATURE, &cursor_x, &cursor_y, NULL);
  cursor_x += 50;
  writeln((GFXfont *)&FiraSans, "°C", &cursor_x, &cursor_y, NULL);

  cursor_x = 100;
  cursor_y += 50;
  writeln((GFXfont *)&FiraSans, HUMIDITY, &cursor_x, &cursor_y, NULL);
  cursor_x += 50;
  writeln((GFXfont *)&FiraSans, "%", &cursor_x, &cursor_y, NULL);

  cursor_x = 100;
  cursor_y += 50;
  writeln((GFXfont *)&FiraSans, PRESSURE, &cursor_x, &cursor_y, NULL);
  cursor_x += 50;
  writeln((GFXfont *)&FiraSans, "hPa", &cursor_x, &cursor_y, NULL);

  cursor_x = 100;
  cursor_y += 50;
  writeln((GFXfont *)&FiraSans, LIGHT, &cursor_x, &cursor_y, NULL);
  cursor_x += 50;
  writeln((GFXfont *)&FiraSans, "lux", &cursor_x, &cursor_y, NULL);



  cursor_x = 100;
  cursor_y += 50;
  writeln((GFXfont *)&FiraSans, HEATINDEX, &cursor_x, &cursor_y, NULL);
  cursor_x += 50;
  writeln((GFXfont *)&FiraSans, "°C", &cursor_x, &cursor_y, NULL);

  cursor_x = 100;
  cursor_y += 40;
  writeln((GFXfont *)&FiraSans,"Fouka ", &cursor_x, &cursor_y, NULL);
  cursor_x += 40;
  writeln((GFXfont *)&FiraSans, winddirection, &cursor_x, &cursor_y, NULL);
  cursor_x += 40;
  writeln((GFXfont *)&FiraSans, " rychlosti ", &cursor_x, &cursor_y, NULL);
  cursor_x += 40;
  writeln((GFXfont *)&FiraSans, WSPEED, &cursor_x, &cursor_y, NULL);
cursor_x += 40;
  writeln((GFXfont *)&FiraSans," m/s", &cursor_x, &cursor_y, NULL);


/*------------------DOMA-----------------*/
  cursor_x = 50;
  cursor_y = 470;
  writeln((GFXfont *)&FiraSans, "Doma", &cursor_x, &cursor_y, NULL);



  cursor_x = 100;
  cursor_y = 510;
  writeln((GFXfont *)&FiraSans, temp, &cursor_x, &cursor_y, NULL);
  cursor_x += 15;
  writeln((GFXfont *)&FiraSans, "°C", &cursor_x, &cursor_y, NULL);
  cursor_x += 40;
  writeln((GFXfont *)&FiraSans, hum, &cursor_x, &cursor_y, NULL);
  cursor_x += 15;
  writeln((GFXfont *)&FiraSans, "%", &cursor_x, &cursor_y, NULL);


  epd_poweroff();
  Hibernace();
}

void loop()
{

}



void WiFi_Connect() {
  double wificount = 0;
  Serial.println(" ");
  Serial.print("Připojuji se k: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(400);
    Serial.print(".");
    wificount++;
    delay(100);
    if (wificount < 40) {

    }
    else {
      ESP.restart();
    }
  }
  Serial.println("Uspěšně připojeno");
  Serial.print("Moje IP adresa: ");
  Serial.println(WiFi.localIP());
  long rssi = WiFi.RSSI();
  Serial.print("Síla WiFi signálu je: ");
  Serial.print(rssi);
  Serial.println(" dBm");

}

void WiFi_Disconnect() {
  WiFi.disconnect(true);  // Disconnect from the network
  WiFi.mode(WIFI_OFF);    // Switch WiFi off
}

void Hibernace() {


  Serial.println("Jdu do režimu hibernace");
  esp_sleep_enable_timer_wakeup(DOBA_HIBERNACE * 1000000);
  esp_deep_sleep_start();
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
      dtostrf(dewpoint, 6, 2, DEWPOINT);
    }
    else if (meas == "HeatIndex") {
      heatindex = value;
      dtostrf(heatindex, 6, 2, HEATINDEX);
    }
    else if (meas == "HeaterStatus") {
      heaterstatus = value;
      dtostrf(heaterstatus, 6, 2, HEATERSTATUS);
    }
    else if (meas == "Humidity") {
      humidity = value;
      dtostrf(humidity, 6, 2, HUMIDITY);
    }
    else if (meas == "Light") {
      light = value;
      dtostrf(light, 8, 2, LIGHT);
    }
    else if (meas == "Precipitation") {
      precipitation = value;
      dtostrf(precipitation, 6, 2, PRECIPITATION);
    }
    else if (meas == "Pressure") {
      pressure = value;
      dtostrf(pressure, 6, 2, PRESSURE);
    }
    else if (meas == "PressureRAW") {
      pressureraw = value;
      dtostrf(pressureraw, 6, 2, PRESSURERAW);
    }
    else if (meas == "Speed") {
      wspeed = value;
      dtostrf(wspeed, 6, 2, WSPEED);
    }
    else if (meas == "Temperature") {
      temperature = value;
      dtostrf(temperature, 6, 2, TEMPERATURE);
    }
    else if (meas == "Temperature2") {
      temperature2 = value;
      dtostrf(temperature2, 6, 2, TEMPERATURE2);
    }
    else if (meas == "UV") {
      uv = value;
      dtostrf(uv, 6, 2, UV);
    }
    else if (meas == "Voltage") {
      voltage = value;
      dtostrf(voltage, 6, 2, VOLTAGE);
    }
    else if (meas == "WindDir") {
      winddir = value;
      dtostrf(winddir, 6, 2, WINDDIR);
    }
    else if (meas == "WindChill") {
      windchill = value;
      dtostrf(windchill, 6, 2, WINDCHILL);
    }
    else {}
    delay(10);
    if (winddir >= 22.5 && winddir < 67.5) {
      winddirection = "ze severovychodu";
    }
    else if (winddir >= 67.5 && winddir < 112.5) {
      winddirection = "z vychodu";
    }
    else if (winddir >= 112.5 && winddir < 157.5) {
      winddirection = "z jihovychodu";
    }
    else if (winddir >= 157.5 && winddir < 202.5) {
      winddirection = "z jihu";
    }
    else if (winddir >= 202.5 && winddir < 247.5) {
      winddirection = "z jihozapadu";
    }
    else if (winddir >= 247.5 && winddir < 292.5) {
      winddirection = "ze zapadu";
    }
    else if (winddir >= 292.5 && winddir <= 337.5) {
      winddirection = "ze severozapadu";
    }
    else {
      winddirection = "ze severu";
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
void TranslateData() {

}
