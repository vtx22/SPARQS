#include "SPARQS.hpp"

SPARQS::SPARQS(UART_HandleTypeDef *huart) : _huart(huart)
{
}

template <typename T>
void SPARQS::print(const uint8_t *ids, const T *values, uint8_t count)
{
    _message_buffer[0] = _signature;
    _message_buffer[1] = 0x00;
    _message_buffer[2] = count;
    _message_buffer[3] = xor8_cs(_message_buffer, 3);

    for (uint8_t i = 0; i < count; i++)
    {
        uint16_t offset = SPARQ_MESSAGE_HEADER_LENGTH + i * SPARQ_BYTES_PER_VALUE_PAIR;
        _message_buffer[offset] = ids[i];

        uint32_t v32 = *(uint32_t *)&values[i];
        _insert_to_buffer(offset + 1, v32, false);
    }

    _send_buffer(count);
}

void SPARQS::print(const std::initializer_list<uint8_t> &ids, const std::initializer_list<uint32_t> &values)
{
    if (ids.size() != values.size())
    {
        return;
    }

    _message_buffer[0] = _signature;
    _message_buffer[1] = 0x00;
    _message_buffer[2] = ids.size();
    _message_buffer[3] = xor8_cs(_message_buffer, 3);

    uint8_t i = 0;
    for (const auto &id : ids)
    {
        _message_buffer[SPARQ_MESSAGE_HEADER_LENGTH + i * SPARQ_BYTES_PER_VALUE_PAIR] = id;
        i++;
    }

    i = 0;
    for (const auto &value : values)
    {
        uint16_t offset = SPARQ_MESSAGE_HEADER_LENGTH + i * SPARQ_BYTES_PER_VALUE_PAIR;

        _insert_to_buffer(offset + 1, value, false);
        i++;
    }

    _send_buffer(ids.size());
}

void SPARQS::_insert_to_buffer(uint16_t offset, uint32_t value, bool big_endian)
{
    if (big_endian)
    {
        _message_buffer[offset] = (value >> 24);
        _message_buffer[offset + 1] = (value >> 16);
        _message_buffer[offset + 2] = (value >> 8);
        _message_buffer[offset + 3] = (value & 0xFF);
        return;
    }

    _message_buffer[offset + 3] = (value >> 24);
    _message_buffer[offset + 2] = (value >> 16);
    _message_buffer[offset + 1] = (value >> 8);
    _message_buffer[offset] = (value & 0xFF);
}

void SPARQS::_send_buffer(uint8_t count)
{
    uint16_t len = SPARQ_MESSAGE_HEADER_LENGTH + count * SPARQ_BYTES_PER_VALUE_PAIR;

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

template void SPARQS::print<int32_t>(const uint8_t *, const int32_t *, uint8_t);
template void SPARQS::print<uint32_t>(const uint8_t *, const uint32_t *, uint8_t);
template void SPARQS::print<float>(const uint8_t *, const float *, uint8_t);