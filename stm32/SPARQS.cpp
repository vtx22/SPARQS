#include "SPARQS.hpp"

SPARQS::SPARQS(UART_HandleTypeDef *huart) : _huart(huart)
{
}

void SPARQS::set_default_id(uint8_t id)
{
    _default_id = id;
}

void SPARQS::print(const char *message)
{
    uint16_t message_length = _strlen(message);

    if (message_length == 0)
    {
        return;
    }

    _insert_header((uint8_t)sparq_message_type_t::STRING << 2, message_length);

    std::copy(message, message + message_length, _message_buffer + SPARQ_MESSAGE_HEADER_LENGTH);

    _send_buffer(message_length);
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
    uint8_t control = 0x00;
    if (!std::is_same<T, float>::value)
    {
        control |= (uint8_t)sparq_header_control_t::INTEGER;
    }

    if (std::is_same<T, int>::value || std::is_same<T, int32_t>::value)
    {
        control |= (uint8_t)sparq_header_control_t::SIGNED;
    }

    uint16_t payload_length = count * SPARQ_BYTES_PER_VALUE_PAIR;

    _insert_header(control, payload_length);

    for (uint8_t i = 0; i < count; i++)
    {
        uint16_t offset = SPARQ_MESSAGE_HEADER_LENGTH + i * SPARQ_BYTES_PER_VALUE_PAIR;
        uint32_t v32 = *(uint32_t *)&values[i];
        _message_buffer[offset] = ids[i];
        _message_buffer[offset + 1] = v32 >> 24;
        _message_buffer[offset + 2] = v32 >> 16;
        _message_buffer[offset + 3] = v32 >> 8;
        _message_buffer[offset + 4] = v32 & 0xFF;
    }

    _send_buffer(payload_length);
}

template <typename T>
void SPARQS::print(const std::initializer_list<uint8_t> &ids, const std::initializer_list<T> &values)
{
    if (ids.size() != values.size())
    {
        return;
    }

    uint8_t control = 0x00;
    if (!std::is_same<T, float>::value)
    {
        control |= (uint8_t)sparq_header_control_t::INTEGER;
    }

    if (std::is_same<T, int>::value || std::is_same<T, int32_t>::value)
    {
        control |= (uint8_t)sparq_header_control_t::SIGNED;
    }

    uint16_t payload_length = ids.size() * SPARQ_BYTES_PER_VALUE_PAIR;

    _insert_header(control, payload_length);

    uint8_t i = 0;
    for (const auto &id : ids)
    {
        _message_buffer[SPARQ_MESSAGE_HEADER_LENGTH + i++ * SPARQ_BYTES_PER_VALUE_PAIR] = id;
    }

    i = 0;
    for (const auto &value : values)
    {
        uint16_t offset = SPARQ_MESSAGE_HEADER_LENGTH + i++ * SPARQ_BYTES_PER_VALUE_PAIR + 1;
        uint32_t v32 = *(uint32_t *)&value;
        _message_buffer[offset] = v32 >> 24;
        _message_buffer[offset + 1] = v32 >> 16;
        _message_buffer[offset + 2] = v32 >> 8;
        _message_buffer[offset + 3] = v32 & 0xFF;
    }

    _send_buffer(payload_length);
}

template <typename T>
void SPARQS::print(const std::initializer_list<T> &values)
{

    uint8_t control = 0x00;
    if (!std::is_same<T, float>::value)
    {
        control |= (uint8_t)sparq_header_control_t::INTEGER;
    }

    if (std::is_same<T, int>::value || std::is_same<T, int32_t>::value)
    {
        control |= (uint8_t)sparq_header_control_t::SIGNED;
    }

    uint16_t payload_length = values.size() * SPARQ_BYTES_PER_VALUE_PAIR;

    _insert_header(control, payload_length);

    uint8_t i = 0;
    for (i = 0; i < values.size(); i++)
    {
        _message_buffer[SPARQ_MESSAGE_HEADER_LENGTH + i * SPARQ_BYTES_PER_VALUE_PAIR] = i + 1;
    }

    i = 0;
    for (const auto &value : values)
    {
        uint16_t offset = SPARQ_MESSAGE_HEADER_LENGTH + i++ * SPARQ_BYTES_PER_VALUE_PAIR + 1;
        uint32_t v32 = *(uint32_t *)&value;
        _message_buffer[offset] = v32 >> 24;
        _message_buffer[offset + 1] = v32 >> 16;
        _message_buffer[offset + 2] = v32 >> 8;
        _message_buffer[offset + 3] = v32 & 0xFF;
    }

    _send_buffer(payload_length);
}

void SPARQS::remote_clear_console()
{
    _send_command(sparq_sender_command_t::CLEAR_CONSOLE);
}

void SPARQS::remote_set_dataset_name(uint8_t ds_id, const char *name)
{
    uint8_t buffer[64];

    buffer[0] = ds_id;

    uint16_t name_length = _strlen(name);
    if (name_length > 63)
    {
        name_length = 63;
    }

    std::memcpy(buffer + 1, name, name_length);

    _send_command(sparq_sender_command_t::SET_DATASET_NAME, buffer, name_length + 1);
}

void SPARQS::remote_delete_all_datasets()
{
    _send_command(sparq_sender_command_t::DELETE_ALL_DATASETS);
}

void SPARQS::remote_delete_dataset(uint8_t id)
{
    _send_command(sparq_sender_command_t::DELETE_SINGLE_DATASET, &id, 1);
}

void SPARQS::remote_clear_all_datasets()
{
    _send_command(sparq_sender_command_t::CLEAR_ALL_DATASETS);
}

void SPARQS::remote_clear_dataset(uint8_t id)
{
    _send_command(sparq_sender_command_t::CLEAR_SINGLE_DATASET, &id, 1);
}

void SPARQS::remote_set_plot_type(sparq_plot_t plot_type)
{
    _send_command(sparq_sender_command_t::SWITCH_PLOT_TYPE, (uint8_t *)&plot_type, 1);
}

void SPARQS::_send_command(sparq_sender_command_t command)
{
    _send_command(command, nullptr, 0);
}

void SPARQS::_send_command(sparq_sender_command_t command, const uint8_t *payload, uint32_t payload_length)
{
    uint8_t control = (uint8_t)sparq_message_type_t::SENDER_COMMAND << 2;
    _insert_header(control, payload_length + 1);

    _message_buffer[SPARQ_MESSAGE_HEADER_LENGTH] = (uint8_t)command;
    std::memcpy(_message_buffer + SPARQ_MESSAGE_HEADER_LENGTH + 1, payload, payload_length);

    _send_buffer(payload_length + 1);
}

void SPARQS::_insert_header(uint8_t control, uint16_t payload_length)
{
    control = control & 0x0F;
    control |= (uint8_t)sparq_header_control_t::CS_EN;

    if (sparq_is_little_endian())
    {
        control |= (uint8_t)sparq_header_control_t::LSB_FIRST;
    }

    _message_buffer[0] = _signature;
    _message_buffer[1] = control;
    _message_buffer[2] = payload_length >> 8;
    _message_buffer[3] = payload_length & 0xFF;
    _message_buffer[4] = xor8_cs(_message_buffer, 4);
}

void SPARQS::_send_buffer(uint16_t payload_length)
{
    uint32_t len = SPARQ_MESSAGE_HEADER_LENGTH + payload_length + SPARQ_CHECKSUM_LENGTH;

    _message_buffer[len - 1] = xor8_cs(_message_buffer, len - 1);

    _transmit_array(_message_buffer, len);
}

void SPARQS::_transmit_array(const uint8_t *data, uint32_t length)
{
    HAL_UART_Transmit(_huart, data, length, SPARQS_HAL_MAX_DELAY);
}

uint16_t SPARQS::_strlen(const char *str) const
{
    uint16_t len = 0;
    while (str[len] != '\0' && len < SPARQ_MAX_PAYLOAD_LENGTH)
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
template void SPARQS::print<unsigned int>(unsigned int);
template void SPARQS::print<int32_t>(int32_t);
template void SPARQS::print<uint32_t>(uint32_t);
template void SPARQS::print<float>(float);

template void SPARQS::print<int>(uint8_t, int);
template void SPARQS::print<unsigned int>(uint8_t, unsigned int);
template void SPARQS::print<int32_t>(uint8_t, int32_t);
template void SPARQS::print<uint32_t>(uint8_t, uint32_t);
template void SPARQS::print<float>(uint8_t, float);

template void SPARQS::print<int>(const uint8_t *, const int *, uint8_t);
template void SPARQS::print<unsigned int>(const uint8_t *, const unsigned int *, uint8_t);
template void SPARQS::print<int32_t>(const uint8_t *, const int32_t *, uint8_t);
template void SPARQS::print<uint32_t>(const uint8_t *, const uint32_t *, uint8_t);
template void SPARQS::print<float>(const uint8_t *, const float *, uint8_t);

template void SPARQS::print<int>(const std::initializer_list<uint8_t> &, const std::initializer_list<int> &);
template void SPARQS::print<unsigned int>(const std::initializer_list<uint8_t> &, const std::initializer_list<unsigned int> &);
template void SPARQS::print<int32_t>(const std::initializer_list<uint8_t> &, const std::initializer_list<int32_t> &);
template void SPARQS::print<uint32_t>(const std::initializer_list<uint8_t> &, const std::initializer_list<uint32_t> &);
template void SPARQS::print<float>(const std::initializer_list<uint8_t> &, const std::initializer_list<float> &);

template void SPARQS::print<int>(const std::initializer_list<int> &);
template void SPARQS::print<unsigned int>(const std::initializer_list<unsigned int> &);
template void SPARQS::print<int32_t>(const std::initializer_list<int32_t> &);
template void SPARQS::print<uint32_t>(const std::initializer_list<uint32_t> &);
template void SPARQS::print<float>(const std::initializer_list<float> &);