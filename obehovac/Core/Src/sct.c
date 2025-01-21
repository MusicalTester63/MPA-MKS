/*
 * sct.c
 *
 *  Created on: Oct 9, 2024
 *      Author: xglosm01
 */


#include "sct.h"


#define MAX 32




static const uint32_t reg_values[1][10] = {
			{
					//PCDE--------GFAB @ DIS1
					0b0010000000000000 << 16,
					0b0001000000000000 << 16,
					0b0000000000000100 << 16,
					0b0000000000000010 << 16,
					//----PCDEGFAB---- @ DIS2
					0b0000000000100000 << 0,
					//PCDE--------GFAB @ DIS3
					0b0000000000000010 << 0,
					0b0000000000000001 << 0,
					0b0100000000000000 << 0,
					0b0010000000000000 << 0,
					//----PCDEGFAB---- @ DIS2
					0b0000001000000000 << 0,
			},
			/*{
					//PCDE--------GFAB @ DIS1
					0b0010000000000000 << 16,
					0b0001000000000000 << 16,
					0b0000000000000100 << 16,
					0b0000000000000010 << 16,
					//----PCDEGFAB---- @ DIS2
					0b0000000000100000 << 0,
					//PCDE--------GFAB @ DIS3
					0b0000000000000010 << 0,
					0b0000000000000001 << 0,
					0b0100000000000000 << 0,
					0b0010000000000000 << 0,
					//----PCDEGFAB---- @ DIS2
					0b0000001000000000 << 0,
			},*/

	};



void sct_init() {
	sct_led(0);
}

void sct_led(uint32_t value) {
	for (uint32_t i = 0; i < MAX; ++i) {
		HAL_GPIO_WritePin(SCT_SDI_GPIO_Port, SCT_SDI_Pin,(value >> i) & 1 );
		HAL_GPIO_WritePin(SCT_CLK_GPIO_Port, SCT_CLK_Pin, 1);
		HAL_GPIO_WritePin(SCT_CLK_GPIO_Port, SCT_CLK_Pin, 0);
	}
	HAL_GPIO_WritePin(SCT_NLA_GPIO_Port, SCT_NLA_Pin, 1);
	HAL_GPIO_WritePin(SCT_NLA_GPIO_Port, SCT_NLA_Pin, 0);

	HAL_GPIO_WritePin(SCT_NOE_GPIO_Port, SCT_NOE_Pin, 0);
}




void sct_value(uint16_t value){

	uint32_t reg = 0;
	reg |= reg_values[0][value];

	sct_led(reg);
}
