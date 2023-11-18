#include "main.h"

void config_clocks();
void config_gpio();

#define DBG 1

int main() {

    /* Set priority */
    HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
    HAL_InitTick(TICK_INT_PRIORITY);

    /* Configure device clocks */
    config_clocks();

    /* Configure the pin for PWM */
    config_gpio();

    /* Configure printing */
#ifdef DBG
    config_debug();
#endif

    __HAL_RCC_TIM1_CLK_ENABLE();

    /* Configure the TIM1 peripheral */
    TIM_HandleTypeDef timer1;
    timer1.Instance = TIM1;
    timer1.Init.Prescaler = 160 - 1;
    timer1.Init.CounterMode = TIM_COUNTERMODE_UP;
    timer1.Init.Period = 1000 - 1;
    timer1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    timer1.Init.RepetitionCounter = 0;
    timer1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    CHECK_DEBUG(HAL_TIM_PWM_Init(&timer1),
        HAL_OK, "Couldn't initialize TIM1 peripheral");

    /* Configure output capture */
    TIM_OC_InitTypeDef oc_config;
    oc_config.OCMode = TIM_OCMODE_PWM1;
    oc_config.OCPolarity = TIM_OCPOLARITY_HIGH;
    oc_config.OCNPolarity = TIM_OCNPOLARITY_HIGH;
    oc_config.OCFastMode = TIM_OCFAST_DISABLE;
    oc_config.OCIdleState = TIM_OCIDLESTATE_RESET;
    oc_config.OCNIdleState = TIM_OCNIDLESTATE_RESET;
    oc_config.Pulse = 400;

    /* Configure channel 1 for output capture */
    CHECK_DEBUG(HAL_TIM_PWM_ConfigChannel(&timer1, &oc_config, TIM_CHANNEL_1),
        HAL_OK, "Couldn't configure output channel 1");

    /* Configure master/slave operation */
    TIM_MasterConfigTypeDef master_config;
    master_config.MasterOutputTrigger = TIM_TRGO_RESET;
    master_config.MasterOutputTrigger2 = TIM_TRGO2_RESET;
    master_config.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    CHECK_DEBUG(HAL_TIMEx_MasterConfigSynchronization(&timer1, &master_config),
        HAL_OK, "Couldn't configure master/slave operation");

    /* Configure dead time behavior */
    TIM_BreakDeadTimeConfigTypeDef deadtime_config;
    deadtime_config.OffStateRunMode = TIM_OSSR_DISABLE;
    deadtime_config.OffStateIDLEMode = TIM_OSSI_DISABLE;
    deadtime_config.LockLevel = TIM_LOCKLEVEL_OFF;
    deadtime_config.DeadTime = 0;
    deadtime_config.BreakState = TIM_BREAK_DISABLE;
    deadtime_config.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
    deadtime_config.BreakFilter = 0;
    deadtime_config.BreakAFMode = TIM_BREAK_AFMODE_INPUT;
    deadtime_config.Break2State = TIM_BREAK2_DISABLE;
    deadtime_config.Break2Polarity = TIM_BREAK2POLARITY_HIGH;
    deadtime_config.Break2Filter = 0;
    deadtime_config.Break2AFMode = TIM_BREAK_AFMODE_INPUT;
    deadtime_config.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
    CHECK_DEBUG(HAL_TIMEx_ConfigBreakDeadTime(&timer1, &deadtime_config),
        HAL_OK, "Couldn't configure dead time behavior");

    /* Start PWM on Channel 1 */
    CHECK_DEBUG(HAL_TIM_PWM_Start(&timer1, TIM_CHANNEL_1),
        HAL_OK, "Couldn't start PWM on Channel 1");

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

/* Configure the device to receive input from the push-button */
void config_gpio() {

    /* Enable the AHB clock for GPIO Port A */
    __HAL_RCC_GPIOA_CLK_ENABLE();

    /* Configure TIM1_CH1 (PA8) for PWM */
    GPIO_InitTypeDef gpio_struct;
    gpio_struct.Pin = GPIO_PIN_8;
    gpio_struct.Mode = GPIO_MODE_AF_PP;
    gpio_struct.Pull = GPIO_PULLUP;
    gpio_struct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    gpio_struct.Alternate = GPIO_AF6_TIM1;
    HAL_GPIO_Init(GPIOA, &gpio_struct);
}
