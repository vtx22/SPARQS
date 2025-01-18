#include "SPARQS.hpp"

SPARQS::SPARQS()
{
}

void SPARQS::set_default_id(uint8_t id)
{
    _default_id = id;
}

void SPARQS::set_signature(uint8_t signature)
{
    _message_buffer[0] = signature;
}

void SPARQS::print(float value)
{
    return print(_default_id, value);
}

void SPARQS::print(uint8_t id, float value)
{
    return print(&id, &value, 1);
}

void SPARQS::print(const uint8_t *ids, const float *values, uint8_t length)
{
    uint32_t offset = SPARQ_MESSAGE_HEADER_LENGTH;
    for (uint8_t i = 0; i < length; i++)
    {
        _message_buffer[offset] = ids[i];
        uint32_t value = *(uint32_t *)&values[i];
        _message_buffer[offset + 1] = (value >> 24);
        _message_buffer[offset + 2] = (value >> 16);
        _message_buffer[offset + 3] = (value >> 8);
        _message_buffer[offset + 4] = (value & 0xFF);

        offset += SPARQ_BYTES_PER_VALUE_PAIR;
    }

    _message_buffer[SPARQ_MESSAGE_HEADER_LENGTH - 1] = cs_xor8(_message_buffer, SPARQ_MESSAGE_HEADER_LENGTH - 1);
    _message_buffer[SPARQ_MESSAGE_HEADER_LENGTH + SPARQ_BYTES_PER_VALUE_PAIR * length + 1] = cs_xor8(_message_buffer, SPARQ_MESSAGE_HEADER_LENGTH + SPARQ_BYTES_PER_VALUE_PAIR * length);

    for (uint8_t i = 0; i < SPARQ_MIN_MESSAGE_LENGTH; i++)
    {
        std::cout << (int)_message_buffer[i] << " ";
    }
}

uint8_t SPARQS::cs_xor8(const uint8_t *data, uint32_t length)
{
    uint8_t cs = 0x00;

    for (uint32_t i = 0; i < length; i++)
    {
        cs ^= data[i];
    }

    return cs;
}