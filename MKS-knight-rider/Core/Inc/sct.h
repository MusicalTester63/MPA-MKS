/*
 * sct.h
 *
 *  Created on: Oct 9, 2024
 *      Author: Jiri Jilek
 *
 *      This is header file of SCT library.
 */

#ifndef INC_SCT_H_
#define INC_SCT_H_

#include "main.h"

void sct_init();
void sct_led(uint32_t value);
void sct_value(uint16_t value, uint16_t led);

#endif /* INC_SCT_H_ */
