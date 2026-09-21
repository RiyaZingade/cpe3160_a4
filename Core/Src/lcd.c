/*******************************************************************************
 * @file           : lcd.c
 * @brief          : lcd program body providing functions to initialize the LCD
 * 					 and write commands/characters to the LCD display.
 *
 *******************************************************************************/

#include "lcd.h"


void LCD_Init(void) {
    RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOBEN);
    LCD_CTRL->MODER &= ~(0x3F0000); // clear PB8-PB10
    LCD_CTRL->MODER |= (0x150000); // set PB8-PB10 to output mode
    LCD_DATA->MODER &= ~(0xFFFF); // clear PB0-PB7
    LCD_DATA->MODER |= (0x5555); // set PB0-PB7 to output mode

    LCD_CTRL->ODR &= ~(LCD_E); // E low
    HAL_Delay(100);
    LCD_Command(WAKEUP);
    HAL_Delay(30);
    LCD_Command(WAKEUP);
    HAL_Delay(10);
    LCD_Command(WAKEUP);
    HAL_Delay(10);

    LCD_Command(FUNCTION_SET);
    LCD_Command(DISPLAY_ON);
    LCD_Command(ENTRY_MODE_SET);
    LCD_Command(CLEAR_DISPLAY);
}

void LCD_Command(uint8_t command) {
    LCD_DATA->ODR &= ~(LCD_DB); //clear DB 7:0
    LCD_DATA->ODR |= (LCD_DB & command); //set DB 7:0
    LCD_CTRL->ODR &= ~(LCD_RS); //clear RS
    LCD_CTRL->ODR &= ~(LCD_RW); //clear R/W - write mode
    LCD_CTRL->ODR |= (LCD_E); //E high
    HAL_Delay(1);
    LCD_CTRL->ODR &= ~(LCD_E); // E low
    }

void LCD_WriteChar(char character) {
    LCD_DATA->ODR &= ~(LCD_DB); //clear DB 7:0
    LCD_DATA->ODR |= (LCD_DB & character); //set DB 7:0
    LCD_CTRL->ODR |= (LCD_RS); //set RS = 1
    LCD_CTRL->ODR &= ~(LCD_RW); //clear R/W - write mode
    LCD_CTRL->ODR |= (LCD_E); //E high
    HAL_Delay(1);
    LCD_CTRL->ODR &= ~(LCD_E); // E low
}

void LCD_WriteString(char* str) {
    while (*str) {
        LCD_WriteChar(*str++);
    }
}