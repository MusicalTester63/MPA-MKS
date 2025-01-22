#include "sct.h"
	static const uint32_t reg_values[5][10] = {
			{
			        //PCDE--------GFAB @ DISPLAY1
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
			        //----PCDEGFAB---- @ DISPLAY2
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
					//PCDE--------GFAB @ DISPLAY3
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
				{
					//PCDE--------GFAB @ DIS1
					//----PCDEGFAB---- @ DIS2
					//PCDE--------GFAB @ DIS3
					0b0000000000000000 << 16,
					0b1000000000000000 << 16,
					0b0000100000000000 << 0,
					0b1000000000000000 << 0,
				},
				{
					//----43215678---- @ LED
					0b0000000000000000 << 16,
					0b0000000100000000 << 16,
					0b0000001100000000 << 16,
					0b0000011100000000 << 16,
					0b0000111100000000 << 16,
					0b0000111110000000 << 16,
					0b0000111111000000 << 16,
					0b0000111111100000 << 16,
					0b0000111111110000 << 16,
				},
	};


// Function for initializing display
void sct_init(){
	sct_led(0);     // Clear display
}

// Function displaying value on the display
void sct_led(uint32_t value){

    for(uint8_t i = 0; i < 32; ++i){    // Loop for 32 bits
        HAL_GPIO_WritePin(SCT_SDI_GPIO_Port, SCT_SDI_Pin, value & 1);  // Write value of the least significant bit to the shift register
        value >>= 1;   // Shift value to the right
        HAL_GPIO_WritePin(SCT_CLK_GPIO_Port, SCT_CLK_Pin, 1);  // Set clock pin to high
        HAL_GPIO_WritePin(SCT_CLK_GPIO_Port, SCT_CLK_Pin, 0);  // Set clock pin to low
    }

    // Latch data - send data to shift register output
    HAL_GPIO_WritePin(SCT_NLA_GPIO_Port, SCT_NLA_Pin, 1);
	HAL_GPIO_WritePin(SCT_NLA_GPIO_Port, SCT_NLA_Pin, 0);
}

// Function for converting value to 3 digit number
void sct_value(uint16_t value, uint16_t dp, uint16_t led){
    uint32_t reg = 0;
	reg |= reg_values[0][value / 100 % 10]; // Set value of hundreds (first display)
	reg |= reg_values[1][value / 10 % 10];  // Set value of tens (second display)
	reg |= reg_values[2][value / 1 % 10];   // Set value of ones (third display)

	//Decimal point
	reg |= reg_values[3][dp];   // Set value of LED

	//LED bar
	reg |= reg_values[4][led];   // Set value of LED


	sct_led(reg);   // Send value to sct_led function
}
