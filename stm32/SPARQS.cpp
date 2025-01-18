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
    print(_default_id, value);
}
void SPARQS::print(uint8_t id, uint32_t value)
{
    _message_buffer[0] = _signature;
    _message_buffer[1] = 0x00;
    _message_buffer[2] = 1;
    _message_buffer[3] = xor8_cs(_message_buffer, 3);

    _message_buffer[4] = id;
    _message_buffer[5] = (value >> 24);
    _message_buffer[6] = (value >> 16);
    _message_buffer[7] = (value >> 8);
    _message_buffer[8] = (value & 0xFF);

    _message_buffer[9] = 0x00;
    _message_buffer[10] = xor8_cs(_message_buffer, 9);

    HAL_UART_Transmit(_huart, _message_buffer, SPARQ_MESSAGE_HEADER_LENGTH + SPARQ_BYTES_PER_VALUE_PAIR + 2, SPARQS_HAL_MAX_DELAY);
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
