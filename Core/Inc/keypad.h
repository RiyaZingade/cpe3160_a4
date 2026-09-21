/*******************************************************************************
 * @file           : lcd.h
 * @brief          : header for lcd.c file with function definitions and constants
 * 					 for LCD commands, ports, and pins
 *
 *******************************************************************************/

#ifndef KEYPAD_H
#define KEYPAD_H

#include <stdint.h>
#include "stm32l4xx_hal.h"

#define ROW_PORT GPIOC
#define COL_PORT GPIOC

#define ROW_PINS 0xF	// PC0-PC3
#define COL_PINS 0x70	// PC4-PC6

#define NUM_ROWS 4
#define NUM_COLS 3

#define ROW1 0		// PC0
#define ROW2 1		// PC1
#define ROW3 2  	// PC2
#define ROW4 3		// PC3
#define COL1 4		// PC4
#define COL2 5		// PC5
#define COL3 6		// PC6

void KEYPAD_Init(void);
int8_t KEYPAD_getKey(void);

#endif
