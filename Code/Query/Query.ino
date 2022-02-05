/**
   QueryAggregated Example code for InfluxDBClient library for Arduino.

   This example demonstrates querying basic aggregated statistic parameters of WiFi signal level measured and stored in BasicWrite and SecureWrite examples.

   Demonstrates connection to any InfluxDB instance accesible via:
    - unsecured http://...
    - secure https://... (appropriate certificate is required)
    - InfluxDB 2 Cloud at https://cloud2.influxdata.com/ (certificate is preconfigured)

    Enter WiFi and InfluxDB parameters below
 **/

#include <WiFiMulti.h>
WiFiMulti wifiMulti;
#define DEVICE "ESP32"


#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>


#define WIFI_SSID "MediumRecords"
#define WIFI_PASSWORD "123456780"

#define INFLUXDB_URL "http://192.168.0.8:8086"
#define INFLUXDB_TOKEN "H4hYgXBEUTFRivBBLdo0Pk_ca1qCAmN8O-LRGjRLRoVitVGcSfervwQQOKa8Jka7rwDa2m7EDlqZn_MmNqsezQ=="
#define INFLUXDB_ORG "2d5652347a7565e5"
#define INFLUXDB_BUCKET "WeatherStation"
String query = "from(bucket: \"" INFLUXDB_BUCKET "\") |> range(start: -1h) |> last() ";

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



void setup() {
  Serial.begin(115200);

  // Setup wifi
  WiFi.mode(WIFI_STA);
  wifiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting to wifi");
  while (wifiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();


  timeSync(TZ_INFO, "pool.ntp.org", "time.nis.gov");

  // Check server connection
  if (client.validateConnection()) {
    Serial.print("Connected to InfluxDB: ");
    Serial.println(client.getServerUrl());
  } else {
    Serial.print("InfluxDB connection failed: ");
    Serial.println(client.getLastErrorMessage());
  }
}


void loop() {

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


if (meas == "DewPoint") {dewpoint = value;}
else if (meas == "HeatIndex") {heatindex = value;}
else if (meas == "HeaterStatus") {heaterstatus = value;}
else if (meas == "Humidity") {humidity = value;}
else if (meas == "Light") {light = value;}
else if (meas == "Precipitation") {precipitation = value;}
else if (meas == "Pressure") {pressure = value;}
else if (meas == "PressureRAW") {pressureraw = value;}
else if (meas == "Speed") {wspeed = value;}
else if (meas == "Temperature") {temperature = value;}
else if (meas == "Temperature2") {temperature2 = value;}
else if (meas == "UV") {uv = value;}
else if (meas == "Voltage") {voltage = value;}
else if (meas == "WindDir") {winddir = value;}
else if (meas == "WindChill") {windchill = value;}
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

  esp_deep_sleep_start();
}
