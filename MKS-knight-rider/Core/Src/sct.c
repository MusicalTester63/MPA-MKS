/*
 * sct.c
 *
 *  Created on: Oct 9, 2024
 *      Author: Jiri Jilek
 * 
 * 
 *      This is source file of library for SCT peripheral.
 * 
 */

#include "sct.h"

static const uint32_t reg_values[6][10] = {   // Shift register preloaded values
		{
				//PCDE--------GFAB @ DISPLAY1
				0b0111000000000111 << 16, 0b0100000000000001 << 16,
				0b0011000000001011 << 16, 0b0110000000001011 << 16,
				0b0100000000001101 << 16, 0b0110000000001110 << 16,
				0b0111000000001110 << 16, 0b0100000000000011 << 16,
				0b0111000000001111 << 16, 0b0110000000001111 << 16, }, {
				//----PCDEGFAB---- @ DISPLAY2
				0b0000011101110000 << 0, 0b0000010000010000 << 0,
				0b0000001110110000 << 0, 0b0000011010110000 << 0,
				0b0000010011010000 << 0, 0b0000011011100000 << 0,
				0b0000011111100000 << 0, 0b0000010000110000 << 0,
				0b0000011111110000 << 0, 0b0000011011110000 << 0, }, {
				//PCDE--------GFAB @ DISPLAY3
				0b0111000000000111 << 0, 0b0100000000000001 << 0,
				0b0011000000001011 << 0, 0b0110000000001011 << 0,
				0b0100000000001101 << 0, 0b0110000000001110 << 0,
				0b0111000000001110 << 0, 0b0100000000000011 << 0,
				0b0111000000001111 << 0, 0b0110000000001111 << 0, },


				{
				//----43215678---- @ LED
				0b0000000000000000 << 16, 0b0000000100000000 << 16,
				0b0000001100000000 << 16, 0b0000011100000000 << 16,
				0b0000111100000000 << 16, 0b0000111110000000 << 16,
				0b0000111111000000 << 16, 0b0000111111100000 << 16,
				0b0000111111110000 << 16, }, {
				// LED patterns
				0b0000000000000000 << 16, // NO LED
				0b0000000100000000 << 16, // LED 1 on
				0b0000001000000000 << 16, // LED 2 on
				0b0000010000000000 << 16, // LED 3 on
				0b0000100000000000 << 16, // LED 4 on
				0b0000000010000000 << 16, // LED 5 on
				0b0000000001000000 << 16, // LED 6 on
				0b0000000000100000 << 16, // LED 7 on
				0b0000000000010000 << 16, // LED 8 on
				0b0000000000000000 << 16, // NO LED
				}, {
				///KNIGHT RIDER
				//0b0000000000000000 << 16, // NO LED
				0b0000000100000000 << 16, // LED 1 on
				0b0000001100000000 << 16, // LED 2 and 2
				0b0000011000000000 << 16, // LED 3 and 2
				0b0000110000000000 << 16, // LED 4 and 3
				0b0000100010000000 << 16, // LED 5 and 4
				0b0000000011000000 << 16, // LED 6 and 5
				0b0000000001100000 << 16, // LED 7 and 6
				0b0000000000110000 << 16, // LED 8 and 5
				0b0000000000010000 << 16, // LED 9 and 8
				//0b0000000000000000 << 16, // NO LED
				}

		};

// Function for initializing display
void sct_init() {
	sct_led(0);     // Clear display
}

// Function displaying value on the display
void sct_led(uint32_t value) {

	for (uint8_t i = 0; i < 32; ++i) {    // Loop for 32 bits
		HAL_GPIO_WritePin(SCT_SDI_GPIO_Port, SCT_SDI_Pin, value & 1); // Write value of the least significant bit to the shift register
		value >>= 1;   // Shift value to the right
		HAL_GPIO_WritePin(SCT_CLK_GPIO_Port, SCT_CLK_Pin, 1); // Set clock pin to high
		HAL_GPIO_WritePin(SCT_CLK_GPIO_Port, SCT_CLK_Pin, 0); // Set clock pin to low
	}

	// Latch data - send data to shift register output
	HAL_GPIO_WritePin(SCT_NLA_GPIO_Port, SCT_NLA_Pin, 1);
	HAL_GPIO_WritePin(SCT_NLA_GPIO_Port, SCT_NLA_Pin, 0);
}

// Function for converting value to 3 digit number
void sct_value(uint16_t value, uint16_t led) {
	uint32_t reg = 1;

	// 8-segment display
	reg |= reg_values[0][value / 100 % 10]; // Set value of hundreds (first display)
	reg |= reg_values[1][value / 10 % 10]; // Set value of tens (second display)
	reg |= reg_values[2][value / 1 % 10];   // Set value of ones (third display)

	//LED bar
	reg |= reg_values[5][led];   // Set value of LED

	// LED bar: Light up LED x and LED x-1

	/*
	 if (led > 1 && led != 8) {
	 // Shift the bitmask to light up LED x and LED x-1
	 reg |= (reg_values[3][led] | reg_values[3][led - 1]);
	 } else {
	 // If LED 0 is requested, just light up LED 0 (no LED -1)
	 reg |= reg_values[3][led];
	 }*/

	sct_led(reg);   // Send value to sct_led function
}

