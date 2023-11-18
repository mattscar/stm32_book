#include "main.h"

void config_clocks();

int main() {

    /* Set priority */
	HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
    HAL_InitTick(TICK_INT_PRIORITY);

    /* Configure device clocks */
    config_clocks();

    /* Enable the AHB2 clock for GPIO Ports A and C */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

    /* Configure the LED pin - PA5 */
    GPIO_InitTypeDef gpio_led;
    gpio_led.Pin = GPIO_PIN_5;
    gpio_led.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_led.Pull = GPIO_PULLUP;
    gpio_led.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(GPIOA, &gpio_led);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);

    /* Configure the Button pin - PC13 */
    GPIO_InitTypeDef gpio_button;
    gpio_button.Pin = GPIO_PIN_13;
    gpio_button.Mode = GPIO_MODE_IT_FALLING;
    gpio_button.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOC, &gpio_button);

    /* Enable interrupt for button pin */
    HAL_NVIC_SetPriority(EXTI15_10_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

    /* Infinite loop */
    while(1) {}
}

/* Callback invoked when button pressed */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
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
