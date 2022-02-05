#include "esp_heap_caps.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "esp_types.h"
#include "epd_driver.h"
#include "epd_highlevel.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <NTPClient.h>
#include <WiFi.h>
#include <stdlib.h>
#include <time.h>
#include <Wire.h>
#include <WiFiUdp.h>
#define I2C_SDA 14
#define I2C_SCL 15


#include <Adafruit_BME280.h>
#include <Adafruit_Sensor.h>
Adafruit_BME280 bme;

#define WLAN_SSID "MediumRecords"
#define WLAN_PASS "123456780"



TwoWire I2CBME = TwoWire(0);


unsigned long delayTime;

void setup() {
  Serial.begin(115200);
  Serial.println(F("BME280 test"));
  Wire.setClock(9766);

  I2CBME.begin(I2C_SDA, I2C_SCL, 9766);

  bool status;
  status = bme.begin(0x76, &I2CBME);
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }
  Serial.println(bme.readTemperature());
  Serial.println(bme.readPressure() / 100.0);
  Serial.println(bme.readHumidity());

}

void loop() {

}
