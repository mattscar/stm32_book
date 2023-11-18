#ifndef PRINT_DEBUG_H_
#define PRINT_DEBUG_H_

#include <string.h>
#include <stm32g4xx.h>
#include <stm32g4xx_hal.h>

void config_debug();
void print_debug(uint8_t* msg);

#endif /* PRINT_DEBUG_H_ */
