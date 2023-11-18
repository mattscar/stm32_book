#include "main.h"

void config_clocks();
void transfer_complete_cbck(DMA_HandleTypeDef *dma);

#define DBG
#define SIZE_IN_BYTES 16
char src_msg[SIZE_IN_BYTES] = "Ch06_DMA_Message";
char dst_msg[SIZE_IN_BYTES];

DMA_HandleTypeDef dma_struct;

int main() {

    /* Set bits in group/subpriority */
    HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
    HAL_InitTick(TICK_INT_PRIORITY);

    /* Configure device clocks */
    config_clocks();

    /* Configure printing output through UART */
    config_debug();

    /* Configure DMA */
    __HAL_RCC_DMA1_CLK_ENABLE();
    dma_struct.Instance = DMA1_Channel1;
    dma_struct.Init.Request = DMA_REQUEST_MEM2MEM;
    dma_struct.Init.Direction = DMA_MEMORY_TO_MEMORY;
    dma_struct.Init.PeriphInc = DMA_PINC_ENABLE;
    dma_struct.Init.MemInc = DMA_MINC_ENABLE;
    dma_struct.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    dma_struct.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    dma_struct.Init.Mode = DMA_NORMAL;
    dma_struct.Init.Priority = DMA_PRIORITY_LOW;
    HAL_DMA_Init(&dma_struct);

    /* Configure DMA interrupt handling */
    HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
    HAL_DMA_RegisterCallback(&dma_struct, HAL_DMA_XFER_CPLT_CB_ID, &transfer_complete_cbck);

    /* Start DMA operation */
    HAL_DMA_Start_IT(&dma_struct, (uint32_t)src_msg, (uint32_t)dst_msg, SIZE_IN_BYTES/4);

    /* Infinite loop */
    while(1) {}
}

/* Display message when DMA operation is complete */
void transfer_complete_cbck(DMA_HandleTypeDef *dma) {
    print_debug(dst_msg);
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
