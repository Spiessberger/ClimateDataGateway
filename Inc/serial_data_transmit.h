#pragma once

#include <stdint.h>

typedef enum {
    SERIAL_DATA_INDOOR_DATA = 0x20,
    SERIAL_DATA_MAX         = 0xFF
} SerialDataType;

void reset_sequence_number();

/**
 * Sends serial data in format
 *
 * Payload:
 *
 * char start byte '$'
 * uint16 sequence number of message. incremented for every sent message
 * uint8 message type of enum SerialDataType
 * uint16 message length
 * message data with variable length
 * uint16 checksum CRC-CCITT polynomial 0x1021, initial value 0xFFFF
 * char end byte '\r'
 */
void send_serial_data(SerialDataType type, const uint8_t* data, uint16_t length);

/**
 * Sends indoor data as int16 with 1 decimal digit
 * 30.123 % -> 301
 *
 * Payload:
 *
 * int16 temperature * 10
 * int16 humidity * 10
 */
void send_serial_data_indoor_data(float temperature, float humidity);

