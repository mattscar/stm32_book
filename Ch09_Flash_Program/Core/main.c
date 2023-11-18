#include "main.h"

void config_clocks();

int main() {

    /* Set priority */
    HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
    HAL_InitTick(TICK_INT_PRIORITY);

    /* Configure device clocks */
    config_clocks();

    /* Configure printing */
    config_debug();

    /* Unlock Flash registers */
    HAL_FLASH_Unlock();

    /* Creature structure for erase operation */
    FLASH_EraseInitTypeDef erase_struct;
    erase_struct.TypeErase = FLASH_TYPEERASE_PAGES;
    erase_struct.Page = 28;
    erase_struct.NbPages = 1;
    erase_struct.Banks = FLASH_BANK_1;

    /* Perform erase operation */
    uint32_t page_error = 0;
    CHECK_DEBUG(HAL_FLASHEx_Erase(&erase_struct, &page_error),
        HAL_OK, "Couldn't erase Page 28");

    /* Write value to Page 28 */
    uint32_t page28_addr = 0x0800E000;
    uint64_t page28_data = 0x0123456789ABCDEF;
    CHECK_DEBUG(HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, page28_addr, page28_data),
        HAL_OK, "Couldn't write to Page 28");

    /* Lock Flash registers */
    HAL_FLASH_Lock();

    /* Check value in Page 28 */
    uint32_t test_addr = 0x0800E000;
    __IO uint64_t val = *(__IO uint64_t *)test_addr;
    if(val == page28_data) {
        print_debug("Page 28 check succeeded");
    } else {
        print_debug("Page 28 check failed");
    }

    /* Infinite loop */
    while(1) {}
}

/* Assign sources to system clocks */
void config_clocks() {

    /* Data structures */
    RCC_OscInitTypeDef osc_struct;
    RCC_ClkInitTypeDef clk_struct;

    /* Configure HSI oscillator */
    osc_struct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    osc_struct.HSIState = RCC_HSI_ON;
    osc_struct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    HAL_RCC_OscConfig(&osc_struct);

    /* Configure clocks */
    clk_struct.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK |
                          RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    clk_struct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
    clk_struct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    clk_struct.APB1CLKDivider = RCC_HCLK_DIV1;
    clk_struct.APB2CLKDivider = RCC_HCLK_DIV1;
    HAL_RCC_ClockConfig(&clk_struct, FLASH_LATENCY_4);
}
