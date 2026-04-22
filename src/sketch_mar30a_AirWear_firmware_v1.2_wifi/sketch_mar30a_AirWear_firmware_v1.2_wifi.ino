#include "Display_ST7789.h"
#include "LVGL_Driver.h"
#include "ui.h"

#include "PMS7003_PM.h"
#include "GPS_Module.h"
#include "SD_Logger.h"

#include <stdio.h>

#include "WiFi_Transmit.h"

static lv_coord_t pm25_history[10] = {0,0,0,0,0,0,0,0,0,0};

int PM25_To_AQHI_Proxy(uint16_t pm25)
{
  if (pm25 <= 6)  return 1;
  if (pm25 <= 12) return 2;
  if (pm25 <= 18) return 3;
  if (pm25 <= 25) return 4;
  if (pm25 <= 32) return 5;
  if (pm25 <= 39) return 6;
  if (pm25 <= 49) return 7;
  if (pm25 <= 59) return 8;
  if (pm25 <= 69) return 9;
  return 10;
}

void UI_Update_PM25(uint16_t pm25)
{
  if (ui_TextArea5)
  {
    char buf[16];
    snprintf(buf, sizeof(buf), "%u", pm25);
    lv_textarea_set_text(ui_TextArea5, buf);
  }

  if (ui_Arc2)
  {
    lv_arc_set_range(ui_Arc2, 1, 10);
    lv_arc_set_value(ui_Arc2, PM25_To_AQHI_Proxy(pm25));
  }

  for (int i = 0; i < 9; i++)
  {
    pm25_history[i] = pm25_history[i + 1];
  }

  pm25_history[9] = (pm25 > 40) ? 40 : pm25;

  if (ui_Chart1 && ui_Chart1_series_1)
  {
    lv_chart_set_point_count(ui_Chart1, 10);
    lv_chart_set_range(ui_Chart1, LV_CHART_AXIS_PRIMARY_Y, 0, 40);

    for (int i = 0; i < 10; i++)
    {
      ui_Chart1_series_1->y_points[i] = pm25_history[i];
    }

    lv_chart_refresh(ui_Chart1);
  }
}

void setup()
{
  Serial.begin(115200);
  delay(200);
  Serial.println("ESP32-S3 AirWear v1.2 boot");

  LCD_Init();
  Lvgl_Init();
  ui_init();
  Serial.println("UI initialized");

  PMS_Init();
  GPS_Init();
  SD_Log_Init();

  WiFi_Init(); 

  // TEMPORARY dummy json data and sd card test blocks — remove before final firmware
  {
    GPS_Record dummyGps = {};
    PMS_Record dummyPm  = {};
    dummyPm.pm1_0_atm   = 11;
    dummyPm.pm2_5_atm   = 22;
    dummyPm.pm10_atm    = 33;
    dummyPm.timestamp_ms = millis();
    WiFi_Transmit(dummyPm, dummyGps);
  }
  {
    GPS_Record dummyGps = {};
    PMS_Record dummyPm  = {};
    dummyPm.pm1_0_atm   = 11;
    dummyPm.pm2_5_atm   = 22;
    dummyPm.pm10_atm    = 33;
    dummyPm.timestamp_ms = millis();
    bool sdOk = SD_Log_PM_GPS(dummyPm, dummyGps);
    Serial.print("SD test write: ");
    Serial.println(sdOk ? "OK" : "FAIL");
  }
  //remove chunck above after testing complete

  Serial.println("Sensor + GPS + SD init complete");
}

void loop()
{
  Timer_Loop();
  GPS_Update();

  static unsigned long lastPmSample = 0;
  static unsigned long lastGpsMsg = 0;

  if (millis() - lastPmSample >= 1000)
  {
    lastPmSample = millis();

    PMS_Record pm;

    if (PMS_Read(pm))
    {
      GPS_Record gps;
      GPS_Get_Record(gps);

      Serial.printf(
        "PMSA003I PM1.0:%u PM2.5:%u PM10:%u particles(0.3):%u\n",
        pm.pm1_0_atm,
        pm.pm2_5_atm,
        pm.pm10_atm,
        pm.particles_0_3um
      );

      UI_Update_PM25(pm.pm2_5_atm);

      bool ok = SD_Log_PM_GPS(pm, gps);
      Serial.print("SD log: ");
      Serial.println(ok ? "OK" : "FAIL");

      WiFi_Transmit(pm, gps);
    }
    else
    {
      Serial.printf("Waiting for PMSA003I frame... last bytes: %d\n", PMS_Available());
    }
  }

  if (millis() - lastGpsMsg > 2000)
  {
    GPS_Record gps;
    bool gpsHasData = GPS_Get_Record(gps);

    if (gpsHasData)
    {
      Serial.printf(
        "GPS data lat: %.6f lon: %.6f sats:%lu age:%lu ms\n",
        gps.latitude,
        gps.longitude,
        (unsigned long)gps.satellites,
        (unsigned long)gps.age_ms
      );
    }
    else
    {
      Serial.println("GPS: no valid data yet");
    }

    lastGpsMsg = millis();
  }

  delay(5);
}