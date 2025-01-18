#pragma once

#include <cstdint>
#include <iostream>

#define SPARQ_MESSAGE_HEADER_LENGTH 4
#define SPARQ_BYTES_PER_VALUE_PAIR 5
#define SPARQ_MAX_MESSAGE_LENGTH (SPARQ_MESSAGE_HEADER_LENGTH + 255 * SPARQ_BYTES_PER_VALUE_PAIR)
#define SPARQ_MIN_MESSAGE_LENGTH 11

class SPARQS
{
public:
    SPARQS();

    void print(float value);
    void print(uint8_t id, float value);
    void print(const uint8_t *ids, const float *values, uint8_t length);

    void set_default_id(uint8_t id);
    void set_signature(uint8_t signature);

    static uint8_t cs_xor8(const uint8_t *data, uint32_t length);

private:
    uint8_t _default_id = 0;
    uint8_t _control = 0;

    uint8_t _message_buffer[SPARQ_MAX_MESSAGE_LENGTH];
};