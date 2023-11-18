#include "main.h"

void config_clocks();
void config_gpio();
int32_t convert_from_q(int32_t);

#define NUM_VALS 32
#define DBG 1

/* Array of values from -1 to almost 1 */
static int32_t input_vals[NUM_VALS] = {
    0x80000000, 0x88000000, 0x90000000, 0x98000000, 0xA0000000, 0xA8000000, 0xB0000000, 0xB8000000,
    0xC0000000, 0xC8000000, 0xD0000000, 0xD8000000, 0xE0000000, 0xE8000000, 0xF0000000, 0xF8000000,
    0x00000000, 0x08000000, 0x10000000, 0x18000000, 0x20000000, 0x28000000, 0x30000000, 0x38000000,
    0x40000000, 0x48000000, 0x50000000, 0x58000000, 0x60000000, 0x68000000, 0x70000000, 0x78000000};

static int32_t output_vals[NUM_VALS];
static int32_t dac_vals[NUM_VALS];

int main() {

    /* Set priority */
    HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
    HAL_InitTick(TICK_INT_PRIORITY);

    /* Configure device clocks */
    config_clocks();

    /* Configure the button for general purpose input/output */
    config_gpio();

    /* Configure printing */
#ifdef DBG
    config_debug();
#endif

    /* Initialize clocks for the DAC and CORDIC peripherals */
    __HAL_RCC_DAC1_CLK_ENABLE();
    __HAL_RCC_CORDIC_CLK_ENABLE();

    /* Initialize the DAC peripheral */
    DAC_HandleTypeDef dac;
    dac.Instance = DAC1;
    CHECK_DEBUG(HAL_DAC_Init(&dac),
        HAL_OK, "Couldn't initialize DAC");

    /* Configure the DAC channel */
    DAC_ChannelConfTypeDef dac_chan;
    dac_chan.DAC_HighFrequency = DAC_HIGH_FREQUENCY_INTERFACE_MODE_AUTOMATIC;
    dac_chan.DAC_DMADoubleDataMode = DISABLE;
    dac_chan.DAC_SignedFormat = DISABLE;
    dac_chan.DAC_SampleAndHold = DAC_SAMPLEANDHOLD_DISABLE;
    dac_chan.DAC_Trigger = DAC_TRIGGER_NONE;
    dac_chan.DAC_Trigger2 = DAC_TRIGGER_NONE;
    dac_chan.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
    dac_chan.DAC_ConnectOnChipPeripheral = DAC_CHIPCONNECT_EXTERNAL;
    dac_chan.DAC_UserTrimming = DAC_TRIMMING_FACTORY;
    CHECK_DEBUG(HAL_DAC_ConfigChannel(&dac, &dac_chan, DAC_CHANNEL_1),
        HAL_OK, "Couldn't configure DAC channel");

    /* Initialize the CORDIC peripheral */
    CORDIC_HandleTypeDef cordic;
    cordic.Instance = CORDIC;
    CHECK_DEBUG(HAL_CORDIC_Init(&cordic),
        HAL_OK, "Couldn't initialize CORDIC");

    /* Configure the CORDIC peripheral */
    CORDIC_ConfigTypeDef cordic_cfg;
    cordic_cfg.Function = CORDIC_FUNCTION_SINE;
    cordic_cfg.Precision = CORDIC_PRECISION_6CYCLES;
    cordic_cfg.NbWrite = CORDIC_NBWRITE_1;
    cordic_cfg.NbRead = CORDIC_NBREAD_1;
    cordic_cfg.InSize = CORDIC_INSIZE_32BITS;
    cordic_cfg.OutSize = CORDIC_OUTSIZE_32BITS;
    cordic_cfg.Scale = CORDIC_SCALE_0;
    CHECK_DEBUG(HAL_CORDIC_Configure(&cordic, &cordic_cfg),
        HAL_OK, "Couldn't configure CORDIC operation");

    /* Compute sine values */
    CHECK_DEBUG(HAL_CORDIC_Calculate(&cordic, input_vals, output_vals, NUM_VALS, HAL_MAX_DELAY),
        HAL_OK, "Couldn't perform CORDIC calculation");

    /* Convert sine values to DAC inputs */
    for(int i=0; i<NUM_VALS; i++) {
        dac_vals[i] = convert_from_q(output_vals[i]);
    }

    /* Launch the DAC's operation */
    CHECK_DEBUG(HAL_DAC_Start(&dac, DAC_CHANNEL_1),
        HAL_OK, "Couldn't start DAC");

    /* Infinite loop */
    unsigned int i = 0;
    while(1) {

        /* Convert each sine value to an analog voltage */
        CHECK_DEBUG(HAL_DAC_SetValue(&dac, DAC_CHANNEL_1, DAC_ALIGN_12B_L, dac_vals[i++]),
            HAL_OK, "Couldn't set DAC value");
        i &= (NUM_VALS - 1);
        HAL_Delay(1);
    }
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

    /* Configure the DAC output pin - PA4 */
    GPIO_InitTypeDef gpio_button;
    gpio_button.Pin = GPIO_PIN_4;
    gpio_button.Mode = GPIO_MODE_ANALOG;
    gpio_button.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &gpio_button);
}

/* Convert value from the Q1.31 format */
int32_t convert_from_q(int32_t val) {

    int32_t result = 0;

    /* Set first bit */
    if (val >= 0) {
        result = 1 << 31;
    }

    /* Set following bits */
    result += (val & 0x7FFFFFFF);

    /* Shift value */
    result >>= 16;

    /* Clear unused bits */
    result &= 0xFFF0;

    return result;
}
