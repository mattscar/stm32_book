#include "main.h"

void config_clocks();
void config_gpio();

void itoa(int n, char s[]);

int main() {

    /* Set priority */
    HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
    HAL_InitTick(TICK_INT_PRIORITY);

    /* Configure device clocks */
    config_clocks();

    /* Configure UART printing */
    config_debug();

    /* Configure push button and LED */
    config_gpio();

    /* Set source of ADC clock */
    RCC_PeriphCLKInitTypeDef clk_init;
    clk_init.PeriphClockSelection = RCC_PERIPHCLK_ADC12;
    clk_init.Adc12ClockSelection = RCC_ADC12CLKSOURCE_SYSCLK;
    HAL_RCCEx_PeriphCLKConfig(&clk_init);

    /* Enable ADC clock */
    __HAL_RCC_ADC12_CLK_ENABLE();

    /* Configure ADC peripheral */
    adc_conf.Instance = ADC1;
    adc_conf.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
    adc_conf.Init.Resolution = ADC_RESOLUTION_12B;
    adc_conf.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    adc_conf.Init.GainCompensation = 0;
    adc_conf.Init.ScanConvMode = ADC_SCAN_DISABLE;
    adc_conf.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
    adc_conf.Init.LowPowerAutoWait = DISABLE;
    adc_conf.Init.ContinuousConvMode = DISABLE;
    adc_conf.Init.NbrOfConversion = 1;
    adc_conf.Init.DiscontinuousConvMode = DISABLE;
    adc_conf.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    adc_conf.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    adc_conf.Init.DMAContinuousRequests = DISABLE;
    adc_conf.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
    adc_conf.Init.OversamplingMode = DISABLE;
    HAL_ADC_Init(&adc_conf);

    /* Configure ADC channel */
    ADC_ChannelConfTypeDef channel_conf;
    channel_conf.Channel = ADC_CHANNEL_6;
    channel_conf.Rank = ADC_REGULAR_RANK_1;
    channel_conf.SamplingTime = ADC_SAMPLETIME_640CYCLES_5;
    channel_conf.SingleDiff = ADC_SINGLE_ENDED;
    channel_conf.OffsetNumber = ADC_OFFSET_NONE;
    channel_conf.Offset = 0;
    HAL_ADC_ConfigChannel(&adc_conf, &channel_conf);

    /* Calibrate the ADC peripheral */
    HAL_ADCEx_Calibration_Start(&adc_conf, ADC_SINGLE_ENDED);

    /* Infinite loop */
    while(1) {}
}

/* Callback invoked when button pressed */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {

    /* Toggle LED */
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);

    /* Start ADC conversion */
    HAL_ADC_Start(&adc_conf);

    /* Wait for conversion to complete */
    HAL_ADC_PollForConversion(&adc_conf, 1000);

    /* Get the conversion result */
    uint32_t res = HAL_ADC_GetValue(&adc_conf);

    /* Print result */
    char result_str[8];
    itoa((int)res, result_str);
    print_debug(result_str);
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

/* Configure GPIO pins */
void config_gpio() {

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

    /* Configure the ADC input - PC0 */
    GPIO_InitTypeDef gpio_adc;
    gpio_adc.Pin = GPIO_PIN_0;
    gpio_adc.Mode = GPIO_MODE_ANALOG;
    gpio_adc.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOC, &gpio_adc);

    /* Enable interrupt for button pin */
    HAL_NVIC_SetPriority(EXTI15_10_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}

void itoa(int n, char s[]) {

    int i, j, sign;
    char c;

    /* Record sign */
    if ((sign = n) < 0)
        n = -n;          /* Make n positive */
    i = 0;
    do {       /* Generate digits in reverse order */
        s[i++] = n % 10 + '0';   /* Get next digit */
    } while ((n /= 10) > 0);     /* Delete it */
    if (sign < 0)
        s[i++] = '-';
    s[i] = '\0';

    /* Reverse the string */
    for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}
