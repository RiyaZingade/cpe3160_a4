/*******************************************************************************
 * @file           : lcd.h
 * @brief          : header for lcd.c file with function definitions and constants
 * 					 for LCD commands, ports, and pins
 *
 *******************************************************************************/

#ifndef LCD_H
#define LCD_H

#include <stdint.h>
#include "stm32l4xx_hal.h"

#define LCD_DATA GPIOB
#define LCD_CTRL GPIOB

#define LCD_RS (1 << 8)     // PB8
#define LCD_RW (1 << 9)     // PB9
#define LCD_E  (1 << 10)    // PB10

#define LCD_DB 0xFF // PB0-PB7

#define CLEAR_DISPLAY     0x01
#define RETURN_HOME       0x02
#define WAKEUP            0x30
#define FUNCTION_SET      0x38
#define DISPLAY_ON        0x0F  // display on, cursor on, blinking on
#define ENTRY_MODE_SET    0x06  // increment cursor, no display shift
#define LINE_TWO          0xC0  // move to second line
#define LINE_ONE          0x80  // move to first line
#define CURSOR_OFF        0x0C  // turns off cursor and blinking


void LCD_Init(void);
void LCD_Command(uint8_t command);
void LCD_WriteChar(char character);
void LCD_WriteString(char* str);

#endif