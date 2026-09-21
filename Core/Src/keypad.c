/*******************************************************************************
 * @file           : lcd.c
 * @brief          : lcd program body providing functions to initialize the LCD
 * 					 and write commands/characters to the LCD display.
 *
 *******************************************************************************/

#include "keypad.h"

#define ASTERISK 0x2A
#define POUND 0xA3

 void KEYPAD_Init() {
    // turns on clock to GPIO bank C
	RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOCEN);

  	// set PC0-3 to input mode (rows)
	ROW_PORT->MODER &= ~(0xFF);

	// clear PUPDR, then set to pull down
	ROW_PORT->PUPDR &= ~(0xFF);
	ROW_PORT->PUPDR |= (0xAA);

	// set PC4-6 to output mode (columns)
	COL_PORT->MODER &= ~(0x3F << (COL1*2));
	COL_PORT->MODER |= (0x15 << (COL1*2));
}

// Returns the ASCII value of keypad button press, or -1 if no button pressed
int8_t KEYPAD_getKey(void) {
	uint8_t row = 0;
	for (uint8_t col = 0; col < NUM_COLS; col++) {
		// set all columns to low
		COL_PORT->ODR &= ~COL_PINS;

		// set the curr column to high
		COL_PORT->ODR |= (1 << (COL1 + col));

		// check rows for a high signal
		row = ROW_PORT->IDR & ROW_PINS;
		if (row == 0) {
			continue;
		}
		else { // check each row to see which one is high
			if (row & (1 << ROW1)) {
				return '0' + col + 1;
			}
			else if (row & (1 << ROW2)) {
				return '0' + col + 4;
			}
			else if (row & (1 << ROW3)) {
				return '0' + col + 7;
			}
			else if (row & (1 << ROW4)) {
				if (col == 0) { // col 0 = *
					return (int8_t)'*';
				}
				else if (col == 1) { // col 1 = 0
                    return (int8_t)'0';
				}
                else if (col == 2) { // col 2 = #
                    return (int8_t)'#';
                }
			}
		}
	}
	return -1;	// no key pressed
}
