//replaces the pms70003 with a pmsa0031

#ifndef PMS7003_PM_H
#define PMS7003_PM_H

#include <Arduino.h>

struct PMS_Record
{
  uint16_t pm1_0_cf1;
  uint16_t pm2_5_cf1;
  uint16_t pm10_cf1;

  uint16_t pm1_0_atm;
  uint16_t pm2_5_atm;
  uint16_t pm10_atm;

  uint16_t particles_0_3um;
  uint16_t particles_0_5um;
  uint16_t particles_1_0um;
  uint16_t particles_2_5um;
  uint16_t particles_5_0um;
  uint16_t particles_10_0um;

  uint32_t timestamp_ms;
};

void PMS_Init(void);
int  PMS_Available(void);
bool PMS_Read(PMS_Record &outRecord);
void PMS_Sleep(bool sleepEnable);

#endif