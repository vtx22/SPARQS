#include "SPARQS.hpp"

SPARQS::SPARQS(UART_HandleTypeDef *huart) : _huart(huart)
{
}

void SPARQS::print(float value)
{
}

void SPARQS::print(uint8_t id, float value)
{
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

void SPARQS::_send_message(uint8_t *buffer, uint32_t length)
{
    HAL_UART_Transmit(_huart, buffer, length, SPARQS_HAL_MAX_DELAY);
}