/*
 * sct.c
 *
 *  Created on: Oct 9, 2024
 *      Author: xhamra02
 */


#include "sct.h"

void sct_init(){
	sct_led(0);
}


void sct_led(uint32_t value) {
    for (int i = 0; i < 32; i++) {
        // Shift the LSB of 'value' to the shift register
        HAL_GPIO_WritePin(SCT_SDI_GPIO_Port, SCT_SDI_Pin, (value & 1) ? GPIO_PIN_SET : GPIO_PIN_RESET);
        HAL_GPIO_WritePin(SCT_CLK_GPIO_Port, SCT_CLK_Pin, GPIO_PIN_SET);  // Pulse the clock
        HAL_GPIO_WritePin(SCT_CLK_GPIO_Port, SCT_CLK_Pin, GPIO_PIN_RESET);
        value >>= 1;  // Shift to the next bit
    }
    // Transfer data to the output latch
    HAL_GPIO_WritePin(SCT_NLA_GPIO_Port, SCT_NLA_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(SCT_NLA_GPIO_Port, SCT_NLA_Pin, GPIO_PIN_RESET);
}




void sct_value(uint16_t value) {
    uint32_t reg = 0;
    reg |= reg_values[0][value / 100 % 10];  // Hundreds
    reg |= reg_values[1][value / 10 % 10];   // Tens
    reg |= reg_values[2][value % 10];        // Units
    sct_led(reg);
}


