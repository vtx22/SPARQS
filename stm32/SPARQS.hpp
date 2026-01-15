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
#include <algorithm>
#include <cstring>

#define SPARQ_MESSAGE_HEADER_LENGTH 5
#define SPARQ_BYTES_PER_VALUE_PAIR 5
#define SPARQ_CHECKSUM_LENGTH 1
#define SPARQ_MAX_PAYLOAD_LENGTH 0xFFFF
#define SPARQ_MAX_MESSAGE_LENGTH (SPARQ_MESSAGE_HEADER_LENGTH + SPARQ_MAX_PAYLOAD_LENGTH + SPARQ_CHECKSUM_LENGTH)

#define SPARQS_HAL_MAX_DELAY 100

constexpr bool sparq_is_little_endian()
{
    constexpr uint32_t value = 0x01020304;
    return reinterpret_cast<const uint8_t *>(&value)[0] == 0x04;
}

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
    SENDER_COMMAND = 0b11,
};

enum class sparq_sender_command_t : uint8_t
{
    CLEAR_CONSOLE,         // Remote clear the console
    CLEAR_ALL_DATASETS,    // Remote clear all datasets data but keep the settings
    CLEAR_SINGLE_DATASET,  // Remote clear a single datasets data but keep the settings
    DELETE_ALL_DATASETS,   // Remote delete all datasets
    DELETE_SINGLE_DATASET, // Remote delete single dataset
    SET_DATASET_NAME,      // Remote set the name of a given dataset
    SWITCH_PLOT_TYPE,      // Remote switch the plot type (e.g. line, heatmap, etc.)
};

enum class sparq_plot_t : uint8_t
{
    LINE,
    HEATMAP,
};

class SPARQS
{
public:
    SPARQS(UART_HandleTypeDef *huart);

    void print(const char *message);
    void print(char *message);

    template <typename T>
    void print(T value);

    template <typename T>
    void print(uint8_t id, T value);

    template <typename T>
    void print(const uint8_t *ids, const T *values, uint8_t count);

    template <typename T>
    void print(const std::initializer_list<uint8_t> &ids, const std::initializer_list<T> &values);

    template <typename T>
    void print(const std::initializer_list<T> &values);

    static uint8_t xor8_cs(const uint8_t *data, uint32_t length);

    void remote_clear_console();
    void remote_set_dataset_name(uint8_t ds_id, const char *name);
    void remote_delete_all_datasets();
    void remote_delete_dataset(uint8_t id);
    void remote_clear_all_datasets();
    void remote_clear_dataset(uint8_t id);
    void remote_set_plot_type(sparq_plot_t plot_type);

    void set_default_id(uint8_t id);

private:
    void _insert_header(uint8_t control, uint16_t payload_length);
    void _send_buffer(uint16_t payload_length);

    void _transmit_array(const uint8_t *data, uint32_t length);
    void _send_command(sparq_sender_command_t command);
    void _send_command(sparq_sender_command_t command, const uint8_t *payload, uint32_t payload_length);

    uint16_t _strlen(const char *str) const;

    uint8_t _message_buffer[SPARQ_MAX_MESSAGE_LENGTH];

    uint8_t _signature = 0xFF;
    uint8_t _default_id = 0x00;

    UART_HandleTypeDef *_huart;
};