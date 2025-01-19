#include "SPARQS.hpp"

SPARQS::SPARQS(UART_HandleTypeDef *huart) : _huart(huart)
{
}

void SPARQS::print(float value)
{
    print(_default_id, value);
}

void SPARQS::print(uint8_t id, float value)
{
    uint32_t v = *(uint32_t *)&value;
    print(id, v);
}

void SPARQS::print(uint32_t value)
{
    print({_default_id}, {value});
}
void SPARQS::print(uint8_t id, uint32_t value)
{
    print({id}, {value});
}

template <uint8_t N>
void SPARQS::print(const std::array<uint8_t, N> &ids, const std::array<uint32_t, N> &values)
{
    _message_buffer[0] = _signature;
    _message_buffer[1] = 0x00;
    _message_buffer[2] = ids.size();
    _message_buffer[3] = xor8_cs(_message_buffer, 3);

    for (uint8_t i = 0; i < ids.size(); i++)
    {
        uint16_t offset = SPARQ_MESSAGE_HEADER_LENGTH + i * SPARQ_BYTES_PER_VALUE_PAIR;

        _message_buffer[offset] = ids[i];
        _message_buffer[offset + 1] = (values[i] >> 24);
        _message_buffer[offset + 2] = (values[i] >> 16);
        _message_buffer[offset + 3] = (values[i] >> 8);
        _message_buffer[offset + 4] = (values[i] & 0xFF);
    }

    uint16_t len = SPARQ_MESSAGE_HEADER_LENGTH + ids.size() * SPARQ_BYTES_PER_VALUE_PAIR;

    _message_buffer[len] = 0x00;
    _message_buffer[len + 1] = xor8_cs(_message_buffer, len);

    HAL_UART_Transmit(_huart, _message_buffer, len, SPARQS_HAL_MAX_DELAY);
}

uint8_t SPARQS::xor8_cs(const uint8_t *data, uint32_t length)
{
    uint8_t cs = 0;

    for (uint32_t i = 0; i < length; i++)
    {
        cs ^= data[i];
    }

    return cs;
}
