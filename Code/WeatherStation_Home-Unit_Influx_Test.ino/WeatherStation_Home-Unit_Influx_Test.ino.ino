
#include <ESP8266WiFiMulti.h>
ESP8266WiFiMulti wifiMulti;
#define DEVICE "ESP8266"


#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>

// WiFi AP SSID
#define WIFI_SSID "MediumRecords"
// WiFi password
#define WIFI_PASSWORD "123456780"
#define INFLUXDB_URL "http://192.168.0.8:8086"
#define INFLUXDB_TOKEN "H4hYgXBEUTFRivBBLdo0Pk_ca1qCAmN8O-LRGjRLRoVitVGcSfervwQQOKa8Jka7rwDa2m7EDlqZn_MmNqsezQ=="
#define INFLUXDB_ORG "MajklovaBastlirna"
#define INFLUXDB_BUCKET "WeatherStation"

// Set timezone string according to https://www.gnu.org/software/libc/manual/html_node/TZ-Variable.html

#define TZ_INFO "CET-1CEST,M3.5.0,M10.5.0/3"

// InfluxDB client instance with preconfigured InfluxCloud certificate
InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN);

// Data point
//Point sensor("wifi_status");

void setup() {
  Serial.begin(115200);

  // Setup wifi
  WiFi.mode(WIFI_STA);
  wifiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting to wifi");
  while (wifiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }
  Serial.println();

  // Add tags
//  sensor.addTag("device", DEVICE);
//  sensor.addTag("SSID", WiFi.SSID());

  // Accurate time is necessary for certificate validation and writing in batches
  // For the fastest time sync find NTP servers in your area: https://www.pool.ntp.org/zone/
  // Syncing progress and the time will be printed to Serial.
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
  // Construct a Flux query
  // Query will find the worst RSSI for last hour for each connected WiFi network with this device
  String query = "from(bucket: \"WeatherStation\") |> range(start: -1h) |> last()";

  // Print composed query
  Serial.print("Querying with: ");
  Serial.println(query);

  // Print ouput header
  //Serial.print("==========");
  // Send query to the server and get result
  FluxQueryResult result = client.query(query);
//Serial.print(result);
  // Iterate over rows. Even there is just one row, next() must be called at least once.
  while (result.next()) {
    // Get converted value for flux result column 'SSID'
//    String Field = result.getValueByName("_measurement").getString();
//    Serial.print(Field);
//    Serial.print(" is: ");

    String Fields = result.getValues().getString();
    Serial.print(Fields);
    Serial.print(" is: ");
    

//    Serial.print("' with RSSI ");
    // Get converted value for flux result column '_value' where there is RSSI value
//    String value = result.getValueByName("_value").getString();
//    Serial.print(value);

    // Get converted value for the _time column
    FluxDateTime time = result.getValueByName("_time").getDateTime();

    // Format date-time for printing
    // Format string according to http://www.cplusplus.com/reference/ctime/strftime/
    String timeStr = time.format("%F %T");

    Serial.print(" at ");
    Serial.print(timeStr);

    Serial.println();
  }

  // Check if there was an error
  if(result.getError() != "") {
    Serial.print("Query result error: ");
    Serial.println(result.getError());
  }

  // Close the result
  result.close();

  Serial.println("Wait 10s");
  delay(10000);
}
