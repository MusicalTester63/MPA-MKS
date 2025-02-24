/*
 * sct.c
 *
 *  Created on: Oct 9, 2024
 *      Author: xhamra02
 */


#include "sct.h"
static const uint32_t reg_values[3][10] = {
{
	//PCDE--------GFAB @ DIS1
	0b0111000000000111 << 16,
	0b0100000000000001 << 16,
	0b0011000000001011 << 16,
	0b0110000000001011 << 16,
	0b0100000000001101 << 16,
	0b0110000000001110 << 16,
	0b0111000000001110 << 16,
	0b0100000000000011 << 16,
	0b0111000000001111 << 16,
	0b0110000000001111 << 16,
	},
	{
	//----PCDEGFAB---- @ DIS2
	0b0000011101110000 << 0,
	0b0000010000010000 << 0,
	0b0000001110110000 << 0,
	0b0000011010110000 << 0,
	0b0000010011010000 << 0,
	0b0000011011100000 << 0,
	0b0000011111100000 << 0,
	0b0000010000110000 << 0,
	0b0000011111110000 << 0,
	0b0000011011110000 << 0,
	},
	{
	//PCDE--------GFAB @ DIS3
	0b0111000000000111 << 0,
	0b0100000000000001 << 0,
	0b0011000000001011 << 0,
	0b0110000000001011 << 0,
	0b0100000000001101 << 0,
	0b0110000000001110 << 0,
	0b0111000000001110 << 0,
	0b0100000000000011 << 0,
	0b0111000000001111 << 0,
	0b0110000000001111 << 0,
	},
};
void sct_init(){
	sct_led(0);
}


void sct_led(uint32_t value) {
	for (int i = 0; i < 32; i++) {
	        if (value & 1) {
	            HAL_GPIO_WritePin(SCT_SDI_GPIO_Port, SCT_SDI_Pin, GPIO_PIN_SET);
	        } else {
	            HAL_GPIO_WritePin(SCT_SDI_GPIO_Port, SCT_SDI_Pin, GPIO_PIN_RESET);
	        }

	        HAL_GPIO_WritePin(SCT_CLK_GPIO_Port, SCT_CLK_Pin, GPIO_PIN_SET);
	        HAL_GPIO_WritePin(SCT_CLK_GPIO_Port, SCT_CLK_Pin, GPIO_PIN_RESET);

	        value >>= 1;

	    }

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


