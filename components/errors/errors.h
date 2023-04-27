// Copyright 2022 PWrInSpace, Kuba
#ifndef ERRORS_H
#define ERRORS_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define MAX_NUMBER_OF_ERRORS CONFIG_MAX_NUMBER_OF_ERRORS

/**
 * @brief error types should be enum, due to shitty hash function
 *
 */
typedef uint8_t error_type_t;
typedef uint8_t error_code_t;
typedef uint32_t error_data_t;

/**
 * @brief initialize errors lib
 *
 * @return true :D
 * @return false :C
 */
bool errors_init(error_type_t *error_types, size_t number_of_errors);

/**
 * @brief Set error, uses = operator;
 *
 * @param type error type
 * @param code error code
 * @return true  :D
 * @return false  :C
 */
bool errors_set(error_type_t type, error_code_t code, uint32_t timeout);

/**
 * @brief Set error, uses |= operator;
 *
 * @param type error type
 * @param code error code
 * @return true :D
 * @return false :C
 */
bool errors_add(error_type_t type, error_code_t code, uint32_t timeout);

/**
 * @brief Get error data
 *
 * @param type error type
 * @return error_data_t error data
 */
error_data_t errors_get(error_type_t type);

/**
 * @brief Reset specific error code
 *
 * @param type error type
 * @return true :D
 * @return false :C
 */
bool errors_reset_code(error_type_t type, error_code_t code, uint32_t timeout);

/**
 * @brief Reset specific type errors
 *
 * @param type error type
 * @return true :D
 * @return false :C
 */
bool errors_reset(error_type_t type, uint32_t timeout);

/**
 * @brief Reset all errors
 *
 * @return true :D
 * @return false :C
 */
bool errors_reset_all(uint32_t timeout);

#endif
