#include "sht_40.h"

#define SHT40_MEASURE_CMD 0xFD
#define SHT40_READ_DATA_LENGTH 6
#define TEMPERATURE_INITIAL_VALUE -273.15f // absolute zero
#define HUMIDITY_INITIAL_VALUE -1.0f

HAL_StatusTypeDef sht40_init(Sht40Handle* handle, I2C_HandleTypeDef* hi2c, uint16_t deviceAddress)
{
  if (handle == NULL || hi2c == NULL)
  {
    return HAL_ERROR;
  }

  handle->hi2c = hi2c;
  handle->deviceAddress = deviceAddress << 1; // Shift left for 7-bit addressing
  handle->temperature = TEMPERATURE_INITIAL_VALUE;
  handle->humidity = HUMIDITY_INITIAL_VALUE;
  handle->dataReady = false;
  return HAL_OK;
}

HAL_StatusTypeDef sht40_measure(Sht40Handle* handle)
{
  if (handle == NULL)
  {
    return HAL_ERROR;
  }

  return HAL_I2C_Mem_Write(handle->hi2c,
                           handle->deviceAddress,
                           SHT40_MEASURE_CMD,
                           I2C_MEMADD_SIZE_8BIT,
                           NULL,
                           0,
                           HAL_MAX_DELAY);
}

HAL_StatusTypeDef sht40_read(Sht40Handle* handle)
{
  if (handle == NULL)
  {
    return HAL_ERROR;
  }

  uint8_t data[SHT40_READ_DATA_LENGTH];

  HAL_StatusTypeDef ret = HAL_I2C_Master_Receive(handle->hi2c, handle->deviceAddress, data, SHT40_READ_DATA_LENGTH, HAL_MAX_DELAY);
  if (ret != HAL_OK)
  {
    return ret;
  }

  const float t_ticks = data[0] * 256 + data[1];
  const float rh_ticks = data[3] * 256 + data[4];
  const float temperature = -45.0f + 175.0f * t_ticks / 65535.0f;
  float humidity = -6.0f + 125.0f * rh_ticks / 65535.0f;

  if (humidity < 0.0f)
  {
    humidity = 0.0f;
  }
  else if (humidity > 100.0f)
  {
    humidity = 100.0f;
  }

  handle->temperature = temperature;
  handle->humidity = humidity;
  handle->dataReady = true;

  return HAL_OK;
}

HAL_StatusTypeDef sht40_get_temperature(Sht40Handle* handle, float* temperature)
{
  if (handle == NULL || temperature == NULL || !handle->dataReady)
  {
    return HAL_ERROR;
  }

  *temperature = handle->temperature;
  return HAL_OK;
}

HAL_StatusTypeDef sht40_get_humidity(Sht40Handle* handle, float* humidity)
{
  if (handle == NULL || humidity == NULL || !handle->dataReady)
  {
    return HAL_ERROR;
  }

  *humidity = handle->humidity;
  return HAL_OK;
}
