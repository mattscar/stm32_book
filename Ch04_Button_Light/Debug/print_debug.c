#include "print_debug.h"

UART_HandleTypeDef lpuart;

void config_debug() {

	/* Configure the LPUART_TX pin - PG7 */
	__HAL_RCC_GPIOG_CLK_ENABLE();
	GPIO_InitTypeDef gpio_lpuart;
	gpio_lpuart.Pin = GPIO_PIN_7 | GPIO_PIN_8;
	gpio_lpuart.Mode = GPIO_MODE_AF_PP;
	gpio_lpuart.Pull = GPIO_NOPULL;
	gpio_lpuart.Speed = GPIO_SPEED_FREQ_LOW;
	gpio_lpuart.Alternate = GPIO_AF8_LPUART1;
	HAL_GPIO_Init(GPIOG, &gpio_lpuart);

	/* Configure power */
	__HAL_RCC_PWR_CLK_ENABLE();
	HAL_PWREx_EnableVddIO2();

	/* Enable clock for LPUART1 */
	__HAL_RCC_LPUART1_CLK_ENABLE();
	RCC_PeriphCLKInitTypeDef per_struct;
	per_struct.PeriphClockSelection = RCC_PERIPHCLK_LPUART1;
	per_struct.Lpuart1ClockSelection = RCC_LPUART1CLKSOURCE_PCLK1;
	HAL_RCCEx_PeriphCLKConfig(&per_struct);

	/* Configure LPUART1 */
	lpuart.Instance = LPUART1;
	lpuart.Init.BaudRate = 9600;
	lpuart.Init.WordLength = UART_WORDLENGTH_8B;
	lpuart.Init.StopBits = UART_STOPBITS_1;
	lpuart.Init.Parity = UART_PARITY_NONE;
	lpuart.Init.Mode = UART_MODE_TX;
	lpuart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	lpuart.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	lpuart.Init.ClockPrescaler = UART_PRESCALER_DIV1;
	lpuart.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	lpuart.FifoMode = UART_FIFOMODE_DISABLE;
	HAL_UART_Init(&lpuart);
}

void print_debug(uint8_t* msg) {
	uint16_t len = strlen(msg);
	uint8_t new_line[] = "\r\n";
	HAL_UART_Transmit(&lpuart, msg, len, 500);
	HAL_UART_Transmit(&lpuart, new_line, 2, 500);
}
