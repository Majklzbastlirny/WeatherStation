#include "esp_heap_caps.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "esp_types.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "epd_driver.h"
#include "epd_highlevel.h"
#include "opensans12b.h"

#define WAVEFORM EPD_BUILTIN_WAVEFORM
EpdiyHighlevelState hl;
int temperature = 20;
uint8_t *fb;
enum EpdDrawError err;
int show_cnt;

void delay(uint32_t millis) { vTaskDelay(millis / portTICK_PERIOD_MS); }

void display_message_test(int cursor_x, int cursor_y, char * text) {
  epd_poweron();
  epd_clear();
  epd_poweroff();
  epd_hl_set_all_white(&hl);
  EpdFontProperties font_props = epd_font_properties_default();
  font_props.flags = EPD_DRAW_ALIGN_LEFT;
  epd_write_string(&OpenSans12B, text, &cursor_x, &cursor_y, fb, &font_props);
  epd_poweron();
  err = epd_hl_update_screen(&hl, MODE_GC16, temperature);
  epd_poweroff();
  delay(1000);
}

void loop()
{
if (show_cnt>=3) return;

show_cnt = show_cnt + 1;
printf("show %d time\n",show_cnt);
if (show_cnt == 1)
{
display_message_test(20, 50, "Hello to you 1");
}
else if (show_cnt == 2)
{
display_message_test(20, 50, "Hello to you 1\nHello to you 2");
}
else if(show_cnt == 3)
{
display_message_test(20, 50, "Hello to you 1\nHello to you 2\nHello to you 3");
}
delay(500);
}

void app_main() {
  epd_init(EPD_OPTIONS_DEFAULT);
  hl = epd_hl_init(WAVEFORM);
  fb = epd_hl_get_framebuffer(&hl);
  show_cnt = 0;
  epd_clear();
  while (true) {
    loop();
  }
}
