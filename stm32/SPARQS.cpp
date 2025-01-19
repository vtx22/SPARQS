#include "SPARQS.hpp"

SPARQS::SPARQS(UART_HandleTypeDef *huart) : _huart(huart)
{
}

void SPARQS::print(const char *message)
{
    uint16_t message_length = _strlen(message);

    if (message_length == 0)
    {
        return;
    }

    // We have to add padding zeros because the message length must be a multiple of SPARQ_BYTES_PER_VALUE_PAIR
    uint8_t padding_zeros = 0;
    if (message_length % SPARQ_BYTES_PER_VALUE_PAIR != 0)
    {
        padding_zeros = SPARQ_BYTES_PER_VALUE_PAIR - (message_length % SPARQ_BYTES_PER_VALUE_PAIR);
    };

    uint8_t nval = (message_length + padding_zeros) / SPARQ_BYTES_PER_VALUE_PAIR;
    _insert_header(0x10, nval);

    for (uint16_t i = 0; i < message_length; i++)
    {
        _message_buffer[SPARQ_MESSAGE_HEADER_LENGTH + i] = message[i];
    }
    for (uint8_t p = 0; p < padding_zeros; p++)
    {
        _message_buffer[SPARQ_MESSAGE_HEADER_LENGTH + message_length + p] = 0x00;
    }

    _send_buffer(nval);
}

template <typename T>
void SPARQS::print(T value)
{
    print(&_default_id, &value, 1);
}

template <typename T>
void SPARQS::print(uint8_t id, T value)
{
    print(&id, &value, 1);
}

template <typename T>
void SPARQS::print(const uint8_t *ids, const T *values, uint8_t count)
{
    _insert_header(0x00, count);

    for (uint8_t i = 0; i < count; i++)
    {
        uint16_t offset = SPARQ_MESSAGE_HEADER_LENGTH + i * SPARQ_BYTES_PER_VALUE_PAIR;
        _message_buffer[offset] = ids[i];

        uint32_t v32 = *(uint32_t *)&values[i];
        _insert_to_buffer(offset + 1, v32, false);
    }

    _send_buffer(count);
}

template <typename T>
void SPARQS::print(const std::initializer_list<uint8_t> &ids, const std::initializer_list<T> &values)
{
    if (ids.size() != values.size())
    {
        return;
    }

    _insert_header(0x00, ids.size());

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

        uint32_t v32 = *(uint32_t *)&value;
        _insert_to_buffer(offset + 1, v32, false);
        i++;
    }

    _send_buffer(ids.size());
}

void SPARQS::_insert_header(uint8_t control, uint8_t count)
{
    control = control & 0x0F;

    if (SPARQ_PLATFORM_LITTLE_ENDIAN)
    {
        control |= (uint8_t)SPARQ_CONTROL::BYTE_ORDER;
    }

    _message_buffer[0] = _signature;
    _message_buffer[1] = control;
    _message_buffer[2] = count;
    _message_buffer[3] = xor8_cs(_message_buffer, 3);
}

void SPARQS::_insert_to_buffer(uint16_t offset, uint32_t value)
{
    if (SPARQ_PLATFORM_LITTLE_ENDIAN)
    {
        _message_buffer[offset + 3] = (value >> 24);
        _message_buffer[offset + 2] = (value >> 16);
        _message_buffer[offset + 1] = (value >> 8);
        _message_buffer[offset] = (value & 0xFF);
        return;
    }

    _message_buffer[offset] = (value >> 24);
    _message_buffer[offset + 1] = (value >> 16);
    _message_buffer[offset + 2] = (value >> 8);
    _message_buffer[offset + 3] = (value & 0xFF);
}

void SPARQS::_send_buffer(uint8_t count)
{
    uint16_t len = SPARQ_MESSAGE_HEADER_LENGTH + count * SPARQ_BYTES_PER_VALUE_PAIR;

    _message_buffer[len] = 0x00;
    _message_buffer[len + 1] = xor8_cs(_message_buffer, len);

    HAL_UART_Transmit(_huart, _message_buffer, len, SPARQS_HAL_MAX_DELAY);
}

uint16_t SPARQS::_strlen(const char *str) const
{
    uint16_t len = 0;
    while (str[len] != '\0' && len < SPARQ_MAX_VALUES * SPARQ_BYTES_PER_VALUE_PAIR)
    {
        len++;
    }
    return len;
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

template void SPARQS::print<int>(int);
template void SPARQS::print<int32_t>(int32_t);
template void SPARQS::print<uint32_t>(uint32_t);
template void SPARQS::print<float>(float);

template void SPARQS::print<int>(uint8_t, int);
template void SPARQS::print<int32_t>(uint8_t, int32_t);
template void SPARQS::print<uint32_t>(uint8_t, uint32_t);
template void SPARQS::print<float>(uint8_t, float);

template void SPARQS::print<int>(const uint8_t *, const int *, uint8_t);
template void SPARQS::print<int32_t>(const uint8_t *, const int32_t *, uint8_t);
template void SPARQS::print<uint32_t>(const uint8_t *, const uint32_t *, uint8_t);
template void SPARQS::print<float>(const uint8_t *, const float *, uint8_t);

template void SPARQS::print<int>(const std::initializer_list<uint8_t> &, const std::initializer_list<int> &);
template void SPARQS::print<int32_t>(const std::initializer_list<uint8_t> &, const std::initializer_list<int32_t> &);
template void SPARQS::print<uint32_t>(const std::initializer_list<uint8_t> &, const std::initializer_list<uint32_t> &);
template void SPARQS::print<float>(const std::initializer_list<uint8_t> &, const std::initializer_list<float> &);