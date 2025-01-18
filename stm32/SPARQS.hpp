#pragma once

#include "sparq_types.hpp"

#include <cstdint>

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
    void _send_message(uint8_t *buffer, uint32_t length);

    uint8_t _message_buffer[SPARQ_MAX_MESSAGE_LENGTH];

    UART_HandleTypeDef *_huart;
};