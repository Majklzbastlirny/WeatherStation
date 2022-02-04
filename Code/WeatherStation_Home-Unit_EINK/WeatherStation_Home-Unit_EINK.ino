#include <Arduino.h>            // In-built
#include <esp_task_wdt.h>       // In-built
#include "freertos/FreeRTOS.h"  // In-built
#include "freertos/task.h"      // In-built
#include "epd_driver.h"         // https://github.com/Xinyuan-LilyGO/LilyGo-EPD47
#include "esp_adc_cal.h"        // In-built
#include "opensans10b.h"
#include "logo.h"
#include <ArduinoJson.h>        // https://github.com/bblanchon/ArduinoJson
#include <HTTPClient.h>         // In-built
#include <NTPClient.h>
#include <WiFi.h>               // In-built
#include <stdlib.h>                // In-built
#include <time.h>               // In-built
#include <Wire.h>
#include <WiFiUdp.h>
#define TZ_INFO "CET-1CEST,M3.5.0,M10.5.0/3"
#define WLAN_SSID "MediumRecords"
#include <Adafruit_BME280.h>
#include <Adafruit_Sensor.h>
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 3600);
Adafruit_BME280 bme;


#define WLAN_PASS "123456780"
double wificount = 0;
uint8_t *framebuffer;

void setup()
{
    Serial.begin(115200);
    Wire.begin(14,15);
/*status = bme.begin();  
    // You can also pass in a Wire library object like &Wire2
    // status = bme.begin(0x76, &Wire2)
    if (!status) {
        Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
        Serial.print("SensorID was: 0x"); Serial.println(bme.sensorID(),16);
        Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
        Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
        Serial.print("        ID of 0x60 represents a BME 280.\n");
        Serial.print("        ID of 0x61 represents a BME 680.\n");
        while (1) delay(10);
    }
*/    


    
WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(400);
    Serial.print(".");
    wificount++;
    //Sprint(wificount);
    delay(100);
   if (wificount < 40){
      
      }
    else {
      
    //  ESP.restart();
      
      }

    // disable Core 0 WDT
  //  TaskHandle_t idle_0 = xTaskGetIdleTaskHandleForCPU(0);
  //  esp_task_wdt_delete(idle_0);

    epd_init();

    framebuffer = (uint8_t *)heap_caps_malloc(EPD_WIDTH * EPD_HEIGHT / 2, MALLOC_CAP_SPIRAM);
    memset(framebuffer, 0xFF, EPD_WIDTH * EPD_HEIGHT / 2);
}
}
void loop()
{
timeClient.update();
    epd_poweron();
    volatile uint32_t t1 = millis();
    epd_clear();
    volatile uint32_t t2 = millis();
    printf("EPD clear took %dms.\n", t2 - t1);
    epd_poweroff();

/*    Rect_t area = {
        .x = 230,
        .y = 20,
        .width = logo_width,
        .height = logo_height,
    };*/
    epd_poweron();
//   epd_draw_grayscale_image(area, (uint8_t *)logo_data);
    epd_poweroff();

    int cursor_x = 0;
    int cursor_y = 18;

String str(timeClient.getFormattedTime());
char str_array[str.length()];
str.toCharArray(str_array, str.length());
char* token = strtok(str_array, " ");

char str1 = [0];
dtostrf(bme.readPressure(), 8, 2, str1);
//vw_send(str1, strlen(str1));

char str2 = [0];
dtostrf(bme.readTemperature(), 3, 2, str2);
//vw_send(str2, strlen(str2));

char str3 = [0];
dtostrf(bme.readHumidity(), 2, 2, str3);
//vw_send(str3, strlen(str3));


    char *string1 = token;
    char *string2 = str1;
    char *string3 = str2;
    char *string4 = str3;

    epd_poweron();
    writeln((GFXfont *)&OpenSans10B, string1, &cursor_x, &cursor_y, NULL);
    delay(500);
    cursor_x = 0;
    cursor_y += 50;
    writeln((GFXfont *)&OpenSans10B, string2, &cursor_x, &cursor_y, NULL);
    delay(500);
    cursor_x = 0;
    cursor_y += 50;
    writeln((GFXfont *)&OpenSans10B, string3, &cursor_x, &cursor_y, NULL);
    delay(500);
    cursor_x = 0;
    cursor_y += 50;
    writeln((GFXfont *)&OpenSans10B, string4, &cursor_x, &cursor_y, NULL);
    delay(500);

    epd_poweroff();
    delay(1000);
    
    epd_poweroff_all();

    esp_sleep_enable_ext1_wakeup(GPIO_SEL_39, ESP_EXT1_WAKEUP_ALL_LOW);
    esp_deep_sleep_start();

}
