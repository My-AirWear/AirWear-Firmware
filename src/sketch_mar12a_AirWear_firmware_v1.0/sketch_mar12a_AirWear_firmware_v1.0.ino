#include "Display_ST7789.h"
#include "LVGL_Driver.h"
#include "ui.h"

void setup()
{
  Serial.begin(115200);
  Serial.println("ESP32-S3 SquareLine UI boot");

  LCD_Init();
  Lvgl_Init();
  ui_init();

  Serial.println("UI initialized");
}

void loop()
{
  Timer_Loop();
  delay(5);
}