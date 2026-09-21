/*******************************************************************************
 * @file           : main.c
 * @brief          : Main program body to write "Hello World"
 * 					 and "Assignment 3" on the LCD display.
 *******************************************************************************/


#include "main.h"
#include "keypad.h"
#include "lcd.h"
#include "stm32l476xx.h"

#define PIN_LENGTH 4

static const int default_pin[4] = {'0', '0', '0', '0'};

static int current_pin[4];

typedef enum {
    LOCKED,
    UNLOCKED,
    CHANGE_PIN
} state_t;

int CheckPIN(const int input[]) {
    for (int i = 0; i < PIN_LENGTH; i++) {
        if (input[i] != current_pin[i]) {
			return 0;
		}
    }
	return 1;
}

 int main(void)
 {
	HAL_Init();
	LCD_Init();
	KEYPAD_Init();

	// Configure LED: PA5 as output
	RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOAEN); // Enable GPIOA clock
	GPIOA->MODER &= ~(0x3 << 5*2);
	GPIOA->MODER |= (0x1 << 5*2); 

	GPIOA->ODR |= (1 << 5); // LED On

	LCD_WriteString("LOCKED");
	LCD_Command(LINE_TWO);
	LCD_WriteString("Enter PIN: ");
	int lcd_write_flag = 0;

	state_t state = LOCKED;

	for (int i = 0; i < PIN_LENGTH; i++) {
	    current_pin[i] = default_pin[i];
	}

	int pin_index = 0;
	int input_pin[4];
	while(1) {
		switch (state) {
			case LOCKED:
				if (lcd_write_flag == 1) {
					GPIOA->ODR |= (1 << 5); // LED On
					LCD_Command(CLEAR_DISPLAY);
					LCD_WriteString("LOCKED");
					LCD_Command(LINE_TWO);
					LCD_WriteString("ENTER KEY: ");
					lcd_write_flag = 0;
				}
				// if () -- if user puts in right pins, state = UNLOCKED --- check each pin
				int input = KEYPAD_getKey();
				if (input != -1 && input != '*') { // if a key is pressed
				    LCD_WriteChar(input);
					input_pin[pin_index] = input;
					pin_index++;
				} else if (input == '*') {
					pin_index = 0;
					lcd_write_flag = 1;
					break;
				}

				if (pin_index == 4) { // compare input_pin to current_pin
					if (CheckPIN(input_pin) == 1) {
						pin_index = 0;
				        state = UNLOCKED;
				        lcd_write_flag = 1;
				    } else {
						pin_index = 0;
				        state = LOCKED;
						lcd_write_flag = 1;

						LCD_Command(CLEAR_DISPLAY);
						LCD_WriteString("INVALID KEY");
						LCD_Command(LINE_TWO);
						LCD_WriteString("TRY AGAIN");
						HAL_Delay(1500); //display error message for 1.5 seconds
				    }
				}
				break;


			case UNLOCKED:
    			if (lcd_write_flag == 1) {
    				LCD_Command(CLEAR_DISPLAY);
    				LCD_WriteString("UNLOCKED");
    				LCD_Command(LINE_TWO);
    				LCD_WriteString("PRESS KEY TO LOCK");
    				lcd_write_flag = 0;
    			}
				// unlocked behavior
				// -- we want -> UNLOCKED when # is pressed
				break;


			case CHANGE_PIN:
			    if(lcd_write_flag == 1){
					LCD_Command(CLEAR_DISPLAY);
					LCD_WriteString("CHANGE PIN");
					LCD_Command(LINE_TWO);
					LCD_WriteString("NEW PIN: ");
					lcd_write_flag = 0;
				}
				// check which numbers are being pressed and update current_pin
				int pin_value = KEYPAD_getKey();

				if(pin_value != -1 && pin_value 1=){
				    LCD_WriteChar(pin_value);

                    current_pin[pin_index] = pin_value;
                    pin_index++;

                    if(pin_index == 4){
                        pin_index = 0;

                        //transitioning out of the change_pin state to the locked state

                        LCD_Command(CLEAR_DISPLAY);
                        LCD_WriteString("PIN SET: %s", current_pin);
						LCD_Command(CURSOR_OFF);
                        HAL_Delay(1500); //display confirmation for 1.5 seconds
						LCD_Command(DISPLAY_ON);
                        lcd_write_flag = 1;
                        state = LOCKED;
                    }

				}
				break;
		}
	}


 }
