#include "PMS7003_PM.h"
#include <Wire.h>

static const uint8_t PMS_ADDR = 0x12;
static const int PMS_SDA_PIN = 5;
static const int PMS_SCL_PIN = 6;

static uint8_t frame[32];
static int last_i2c_bytes = 0;

static uint16_t read16(const uint8_t *buf, int idx)
{
  return (uint16_t(buf[idx]) << 8) | buf[idx + 1];
}

void PMS_Init(void)
{
  Wire.begin(PMS_SDA_PIN, PMS_SCL_PIN);
  delay(100);
  Serial.println("PMSA003I I2C started on Wire");
}

void PMS_Sleep(bool sleepEnable)
{
  (void)sleepEnable;
}

int PMS_Available(void)
{
  return last_i2c_bytes;
}

bool PMS_Read(PMS_Record &outRecord)
{
  last_i2c_bytes = 0;

  Wire.requestFrom((int)PMS_ADDR, 32);
  if (Wire.available() != 32)
  {
    last_i2c_bytes = Wire.available();
    while (Wire.available()) Wire.read();
    return false;
  }

  for (int i = 0; i < 32; i++)
  {
    frame[i] = Wire.read();
  }

  last_i2c_bytes = 32;

  if (frame[0] != 0x42 || frame[1] != 0x4D) return false;
  if (read16(frame, 2) != 28) return false;

  uint16_t sum = 0;
  for (int i = 0; i < 30; i++) sum += frame[i];
  if (sum != read16(frame, 30)) return false;

  outRecord.pm1_0_cf1        = read16(frame, 4);
  outRecord.pm2_5_cf1        = read16(frame, 6);
  outRecord.pm10_cf1         = read16(frame, 8);
  outRecord.pm1_0_atm        = read16(frame, 10);
  outRecord.pm2_5_atm        = read16(frame, 12);
  outRecord.pm10_atm         = read16(frame, 14);
  outRecord.particles_0_3um  = read16(frame, 16);
  outRecord.particles_0_5um  = read16(frame, 18);
  outRecord.particles_1_0um  = read16(frame, 20);
  outRecord.particles_2_5um  = read16(frame, 22);
  outRecord.particles_5_0um  = read16(frame, 24);
  outRecord.particles_10_0um = read16(frame, 26);
  outRecord.timestamp_ms = millis();

  return true;
}