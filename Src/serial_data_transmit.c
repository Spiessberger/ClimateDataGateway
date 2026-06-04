#include "serial_data_transmit.h"

#include <stdbool.h>
#include <math.h>

#include "usbd_cdc_if.h"

#define SEND_BUFFER_SIZE 1024
#define START_BYTE '$'
#define END_BYTE '\r'

#define CRC_INITIAL_VALUE 0xFFFF
#define CRC_POLYNOMIAL 0x1021

static uint16_t SequenceNumber = 0;

static uint16_t crc16(const uint8_t *data, uint16_t length);
static int16_t float_to_int16(float value, int decimalPlaces);
static void write_uint8_to_buffer(uint8_t* buffer, int* offset, uint8_t value);
static void write_int16_to_buffer(uint8_t* buffer, int* offset, int16_t value);
static void write_uint16_to_buffer(uint8_t* buffer, int* offset, uint16_t value);
static void write_uint32_to_buffer(uint8_t* buffer, int* offset, uint32_t value);


void reset_sequence_number()
{
  SequenceNumber = 0;
}

void send_serial_data(SerialDataType type, const uint8_t *data, uint16_t length)
{
  uint8_t sendBuffer[SEND_BUFFER_SIZE];
  int offset = 0;

  write_uint8_to_buffer(sendBuffer, &offset, START_BYTE);
  write_uint16_to_buffer(sendBuffer, &offset, SequenceNumber++);
  write_uint8_to_buffer(sendBuffer, &offset, type);
  write_uint16_to_buffer(sendBuffer, &offset, length);

  // Add actual data to the buffer
  memcpy(sendBuffer + offset, data, length);
  offset += length;

  // Append CRC-16 over the entire frame exluding the start byte (sequence number, type, length, data)
  const uint16_t crc = crc16(sendBuffer, offset);
  write_uint16_to_buffer(sendBuffer, &offset, crc);

  write_uint8_to_buffer(sendBuffer, &offset, END_BYTE);

  CDC_Transmit_FS(sendBuffer, offset);
}

void send_serial_data_indoor_data(float temperature, float humidity)
{
  uint8_t data[4];

  const int decimalPlaces = 1;
  int offset = 0;

  write_int16_to_buffer(data, &offset, float_to_int16(temperature, decimalPlaces));
  write_int16_to_buffer(data, &offset, float_to_int16(humidity, decimalPlaces));

  send_serial_data(SERIAL_DATA_INDOOR_DATA, data, offset);
}

static uint16_t crc16(const uint8_t *data, uint16_t length)
{
  uint16_t crc = CRC_INITIAL_VALUE;

  // augment original message with zeros
  // number of zeros is the size of the crc value (2 bytes)
  const int augmentedLength = length + sizeof(crc);
  for (int byteIndex = 0; byteIndex < augmentedLength; byteIndex++) {
    uint8_t b = 0;
    if (byteIndex < length)
    {
      b = data[byteIndex];
    }

    for (int i = 0; i < 8; i++)
    {
      // check if '1' is shifted out
      const bool performXor = (crc & 0x8000) != 0;
      crc = crc << 1;

      // check if '1' is shifted in from message data
      if (b & 0x80)
      {
        crc |= 1;
      }
      // shift message data
      b = b << 1;

      if (performXor) {
        crc = crc ^ CRC_POLYNOMIAL;
      }
    }
  }

  return crc;
}

static int16_t float_to_int16(float value, int decimalPlaces)
{
  for (int i = 0; i < decimalPlaces; i++)
  {
    value *= 10;
  }
  return (int16_t)value;
}

static void write_uint8_to_buffer(uint8_t* buffer, int* offset, uint8_t value)
{
  buffer[*offset] = value;
  *offset += 1;
}

static void write_uint16_to_buffer(uint8_t* buffer, int* offset, uint16_t value)
{
  // little endian
  buffer[*offset] = value & 0xFF;            // Low byte
  buffer[*offset + 1] = (value >> 8) & 0xFF; // High byte
  *offset += 2;
}

static void write_int16_to_buffer(uint8_t* buffer, int* offset, int16_t value)
{
  // little endian
  buffer[*offset] = value & 0xFF;            // Low byte
  buffer[*offset + 1] = (value >> 8) & 0xFF; // High byte
  *offset += 2;
}

static void write_uint32_to_buffer(uint8_t* buffer, int* offset, uint32_t value)
{
  // little endian
  buffer[*offset] = value & 0xFF;             // Low byte
  buffer[*offset + 1] = (value >> 8)  & 0xFF;
  buffer[*offset + 2] = (value >> 16) & 0xFF;
  buffer[*offset + 3] = (value >> 24) & 0xFF; // High byte
  *offset += 4;
}