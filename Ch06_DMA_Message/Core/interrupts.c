#include <stm32g4xx_hal.h>
#include <interrupts.h>

/* Non-maskable interrupt service routine */
void NMI_Handler() {
    while (1) {}
}

/* Hard fault service routine */
void HardFault_Handler() {
    while (1) {}
}

/* Memory management fault service routine */
void MemManage_Handler() {
    while (1) {}
}

/* Bus fault service routine */
void BusFault_Handler() {
    while (1) {}
}

/* Usage fault service routine */
void UsageFault_Handler() {
    while (1) {}
}

/* SVCall service routine */
void SVC_Handler() {}

/* Debug monitor service routine */
void DebugMon_Handler() { }

/* PendSV service routine */
void PendSV_Handler() {}

/* SysTick service routine */
void SysTick_Handler() {
    HAL_IncTick();
}

/* DMA service routine */
extern DMA_HandleTypeDef dma_struct;
void DMA1_Channel1_IRQHandler() {
    HAL_DMA_IRQHandler(&dma_struct);
}
