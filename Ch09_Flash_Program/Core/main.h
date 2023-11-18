#include <stdio.h>
#include <stdlib.h>

#include <stm32g4xx.h>
#include <stm32g4xx_hal.h>

#include "print_debug.h"
#define CHECK_DEBUG(res, val, msg) if(res != val) { print_debug(msg); }

// The system clock frequency
uint32_t SystemCoreClock = HSI_VALUE;

const uint8_t  AHBPrescTable[16] = {0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 1U, 2U, 3U, 4U, 6U, 7U, 8U, 9U};
const uint8_t  APBPrescTable[8] =  {0U, 0U, 0U, 0U, 1U, 2U, 3U, 4U};
