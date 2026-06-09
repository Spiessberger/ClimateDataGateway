#include "sht40_polling.h"

#include <stddef.h>

#define SHT40_POLLING_START_DELAY_MS 1000U
#define SHT40_POLLING_READ_DELAY_MS 100U
#define SHT40_POLLING_PERIOD_MS 60000U
#define SHT40_POLLING_RETRY_DELAY_MS 5000U

static void timer_elapsed(void* userData);
static void schedule_measurement(Sht40PollingHandle* handle, uint32_t delayMs);
static void schedule_read(Sht40PollingHandle* handle);

HAL_StatusTypeDef sht40_polling_init(Sht40PollingHandle* handle,
                                     Sht40Handle* sensor)
{
  if (handle == NULL || sensor == NULL)
  {
    return HAL_ERROR;
  }

  handle->sensor = sensor;
  handle->waitingForRead = false;

  software_timer_init(&handle->timer);
  software_timer_set_callback(&handle->timer, timer_elapsed, handle);

  return HAL_OK;
}

bool sht40_polling_start(Sht40PollingHandle* handle)
{
  if (handle == NULL)
  {
    return false;
  }

  schedule_measurement(handle, SHT40_POLLING_START_DELAY_MS);
  return true;
}

static void timer_elapsed(void* userData)
{
  Sht40PollingHandle* handle = userData;
  if (handle == NULL || handle->sensor == NULL)
  {
    return;
  }

  if (handle->waitingForRead)
  {
    if (sht40_read(handle->sensor) == HAL_OK)
    {
      schedule_measurement(handle, SHT40_POLLING_PERIOD_MS);
    }
    else
    {
      schedule_measurement(handle, SHT40_POLLING_RETRY_DELAY_MS);
    }
    return;
  }

  if (sht40_measure(handle->sensor) == HAL_OK)
  {
    schedule_read(handle);
  }
  else
  {
    schedule_measurement(handle, SHT40_POLLING_RETRY_DELAY_MS);
  }
}

static void schedule_measurement(Sht40PollingHandle* handle, uint32_t delayMs)
{
  handle->waitingForRead = false;
  software_timer_start_oneshot(&handle->timer, delayMs);
}

static void schedule_read(Sht40PollingHandle* handle)
{
  handle->waitingForRead = true;
  software_timer_start_oneshot(&handle->timer, SHT40_POLLING_READ_DELAY_MS);
}
