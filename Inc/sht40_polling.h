#pragma once

#include <stdbool.h>

#include "sht_40.h"
#include "software_timer.h"

typedef struct {
  Sht40Handle* sensor;
  SoftwareTimer timer;
  bool waitingForRead;
} Sht40PollingHandle;

HAL_StatusTypeDef sht40_polling_init(Sht40PollingHandle* handle,
                                     Sht40Handle* sensor);
bool sht40_polling_start(Sht40PollingHandle* handle);
