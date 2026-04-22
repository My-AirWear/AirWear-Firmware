#ifndef SD_LOGGER_H
#define SD_LOGGER_H

#include <Arduino.h>
#include "PMS7003_PM.h"
#include "GPS_Module.h"

bool SD_Log_Init(void);
bool SD_Log_PM_GPS(const PMS_Record &pm, const GPS_Record &gps);

#endif
