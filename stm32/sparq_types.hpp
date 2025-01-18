#pragma once

#include <cstdint>

#define SPARQ_MAX_VALUES 255

// === DONT CHANGE ANYTHING BELOW ===

#define SPARQ_MESSAGE_HEADER_LENGTH 4
#define SPARQ_BYTES_PER_VALUE_PAIR 5
#define SPARQ_MAX_MESSAGE_LENGTH (SPARQ_MESSAGE_HEADER_LENGTH + SPARQ_MAX_VALUES * SPARQ_BYTES_PER_VALUE_PAIR + 2)

struct sparq_message_header_t
{
    uint8_t signature;
    uint8_t control;
    uint8_t nval;
    uint8_t checksum;

    sparq_message_header_t()
    {
    }

    sparq_message_header_t(const uint8_t *buffer)
    {
        from_array(buffer);
    }

    void from_array(const uint8_t *buffer)
    {
        signature = buffer[0];
        control = buffer[1];
        nval = buffer[2];
        checksum = buffer[3];
    }

    void to_array(uint8_t *buffer)
    {
        buffer[0] = signature;
        buffer[1] = control;
        buffer[2] = nval;
        buffer[3] = checksum;
    }
};

struct sparq_message_t
{
    sparq_message_header_t header;
    uint8_t ids[SPARQ_MAX_VALUES];
    float values[SPARQ_MAX_VALUES];
    uint16_t checksum = 0;

    void to_array(uint8_t *buffer)
    {
        header.to_array(buffer);
        buffer += SPARQ_MESSAGE_HEADER_LENGTH;

        for (uint8_t i = 0; i < header.nval; i++)
        {
            uint16_t off = i * SPARQ_BYTES_PER_VALUE_PAIR;

            uint32_t value = *(uint32_t *)&values[i];
            buffer[off] = ids[i];
            buffer[off + 1] = (value >> 24);
            buffer[off + 2] = (value >> 16);
            buffer[off + 3] = (value >> 8);
            buffer[off + 4] = (value & 0xFF);
        }
    }
};
