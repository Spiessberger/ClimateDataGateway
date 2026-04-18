#include <stdint.h>

#include "stm32f4xx_hal.h"

typedef struct {
    I2C_HandleTypeDef* hi2c;
    uint16_t deviceAddress;
    float temperature;
    float humidity;
} Sht40Handle;

HAL_StatusTypeDef sht40_init(Sht40Handle* handle, I2C_HandleTypeDef* hi2c, uint16_t deviceAddress);

HAL_StatusTypeDef sht40_measure(Sht40Handle* handle);

HAL_StatusTypeDef sht40_read(Sht40Handle* handle);

HAL_StatusTypeDef sht40_get_temperature(Sht40Handle* handle, float* temperature);

HAL_StatusTypeDef sht40_get_humidity(Sht40Handle* handle, float* humidity);