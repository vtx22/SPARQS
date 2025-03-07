#pragma once

#include <cstdint>

// === DONT CHANGE ANYTHING BELOW ===

#if defined(STM32F0)
#include "stm32f0xx_hal.h"
#elif defined(STM32F1)
#include "stm32f1xx_hal.h"
#elif defined(STM32F2)
#include "stm32f2xx_hal.h"
#elif defined(STM32F3)
#include "stm32f3xx_hal.h"
#elif defined(STM32F4)
#include "stm32f4xx_hal.h"
#elif defined(STM32F7)
#include "stm32f7xx_hal.h"
#elif defined(STM32H7)
#include "stm32h7xx_hal.h"
#else
#error "Unsupported STM32 microcontroller. Make sure you build with -D STM32F1 for example!"
#endif

#include <initializer_list>
#include <type_traits>

#define SPARQ_MESSAGE_HEADER_LENGTH 5
#define SPARQ_BYTES_PER_VALUE_PAIR 5
#define SPARQ_CHECKSUM_LENGTH 1
#define SPARQ_MAX_PAYLOAD_LENGTH 0xFFFF
#define SPARQ_MAX_MESSAGE_LENGTH (SPARQ_MESSAGE_HEADER_LENGTH + SPARQ_MAX_PAYLOAD_LENGTH + SPARQ_CHECKSUM_LENGTH)

#define SPARQS_HAL_MAX_DELAY 100

constexpr bool SPARQ_PLATFORM_LITTLE_ENDIAN = true;

enum class sparq_header_control_t : uint8_t
{
    LSB_FIRST = (1 << 7),
    CS_EN = (1 << 6),
    MSG_TYPE = (1 << 2) + (1 << 3),
    SIGNED = (1 << 1),
    INTEGER = (1 << 0),
};

enum class sparq_message_type_t : uint8_t
{
    ID_PAIR = 0b00,
    STRING = 0b01,
    BULK_SINGLE_ID = 0b10,
};

class SPARQS
{
public:
    SPARQS(UART_HandleTypeDef *huart);

    void print(const char *message);

    template <typename T>
    void print(T value);

    template <typename T>
    void print(uint8_t id, T value);

    template <typename T>
    void print(const uint8_t *ids, const T *values, uint8_t count);

    template <typename T>
    void print(const std::initializer_list<uint8_t> &ids, const std::initializer_list<T> &values);

    static uint8_t xor8_cs(const uint8_t *data, uint32_t length);

    void set_default_id(uint8_t id);

private:
    void _insert_header(uint8_t control, uint16_t payload_length);
    void _insert_to_buffer(uint16_t offset, uint32_t value);
    void _send_buffer(uint16_t payload_length);

    void _transmit_array(const uint8_t *data, uint32_t length);

    uint16_t _strlen(const char *str) const;

    uint8_t _message_buffer[SPARQ_MAX_MESSAGE_LENGTH];

    uint8_t _signature = 0xFF;
    uint8_t _default_id = 0x00;

    UART_HandleTypeDef *_huart;
};