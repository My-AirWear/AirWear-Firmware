#include "SD_Logger.h"
#include "FS.h"
#include "SD_MMC.h"

#ifdef CONFIG_IDF_TARGET_ESP32S3
int clk = 16;
int cmd = 15;
int d0  = 17;
int d1  = 18;
int d2  = 13;
int d3  = 14;
#endif

static const char *LOG_FILE = "/airwear_log.csv";
static bool sd_ready = false;

bool SD_Log_Init(void)
{
  if(!SD_MMC.setPins(clk, cmd, d0, d1, d2, d3))
  {
    Serial.println("SD pin set failed");
    return false;
  }

  if(!SD_MMC.begin())
  {
    Serial.println("Card Mount Failed");
    return false;
  }

  if(SD_MMC.cardType() == CARD_NONE)
  {
    Serial.println("No SD card attached");
    return false;
  }

  sd_ready = true;
  Serial.println("SD card mounted");

  if(!SD_MMC.exists(LOG_FILE))
  {
    File f = SD_MMC.open(LOG_FILE, FILE_WRITE);
    if(!f)
    {
      Serial.println("Failed to create log file");
      return false;
    }

    f.println("millis,pm1_atm,pm25_atm,pm10_atm,particles_0_3,lat,lon,alt_m,speed_kmph,sats,gps_age_ms,year,month,day,hour,minute,second");
    f.close();
    Serial.println("CSV header written");
  }

  return true;
}

bool SD_Log_PM_GPS(const PMS_Record &pm, const GPS_Record &gps)
{
  if(!sd_ready) return false;

  File f = SD_MMC.open(LOG_FILE, FILE_APPEND);
  if(!f)
  {
    Serial.println("Failed to open log file for append");
    return false;
  }

  f.printf(
    "%lu,%u,%u,%u,%u,%.6f,%.6f,%.2f,%.2f,%lu,%lu,%u,%u,%u,%u,%u,%u\n",
    (unsigned long)pm.timestamp_ms,
    pm.pm1_0_atm,
    pm.pm2_5_atm,
    pm.pm10_atm,
    pm.particles_0_3um,
    gps.valid_location ? gps.latitude : 0.0,
    gps.valid_location ? gps.longitude : 0.0,
    gps.altitude_m,
    gps.speed_kmph,
    (unsigned long)gps.satellites,
    (unsigned long)gps.age_ms,
    gps.year,
    gps.month,
    gps.day,
    gps.hour,
    gps.minute,
    gps.second
  );

  f.close();
  return true;
}
