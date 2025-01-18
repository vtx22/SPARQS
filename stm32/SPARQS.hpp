#pragma once

#include <cstdint>

// === Lower the max values for a smaller message buffer ===
constexpr uint8_t SPARQ_MAX_VALUES = 255;

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

#define SPARQ_MESSAGE_HEADER_LENGTH 4
#define SPARQ_BYTES_PER_VALUE_PAIR 5
#define SPARQ_MAX_MESSAGE_LENGTH (SPARQ_MESSAGE_HEADER_LENGTH + SPARQ_MAX_VALUES * SPARQ_BYTES_PER_VALUE_PAIR + 2)

#define SPARQS_HAL_MAX_DELAY 100

class SPARQS
{
public:
    SPARQS(UART_HandleTypeDef *huart);

    void print(float value);
    void print(uint8_t id, float value);
    void print(const char *message);

    static uint8_t xor8_cs(const uint8_t *data, uint32_t length);

private:
    uint8_t _message_buffer[SPARQ_MAX_MESSAGE_LENGTH];

    uint8_t _signature = 0xFF;
    uint8_t _default_id = 0x00;

    UART_HandleTypeDef *_huart;
};