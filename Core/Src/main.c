/*******************************************************************************
 * @file           : main.c
 * @brief          : Main program body for the Project 1 Digital Lockbox
 * 					 using the Keypad and LCD display.
 *******************************************************************************/


#include "main.h"
#include "keypad.h"
#include "lcd.h"
#include "stm32l476xx.h"

#define PIN_LENGTH 4

// Uses ASCII values of chars
static const char default_pin[PIN_LENGTH] = {'0', '0', '0', '0'};

static char current_pin[PIN_LENGTH];

typedef enum {
    LOCKED,
    UNLOCKED,
    CHANGE_PIN
} state_t;

static int CheckPIN(const char input[]) {
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
	GPIOA->MODER &= ~(0x3 << (5*2));
	GPIOA->MODER |= (0x1 << (5*2));

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
	char input_pin[PIN_LENGTH];
	char new_pin[PIN_LENGTH];

	while(1) {
		switch (state) {
			case LOCKED: {
			//common code block for clearing display and displaying intended message for specified state
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

				if ((input >= '0') && (input <= '9')) { // if a key 0-9 is pressed, not * or #
				    LCD_WriteChar(input);
					input_pin[pin_index] = input;
					pin_index++;
				} else if (input == '*') {
					pin_index = 0;
					lcd_write_flag = 1;
				}

				if (input != -1) { // keypress detected
				    while (KEYPAD_getKey() != -1) {} // wait until keypad is released before continuing
					HAL_Delay(50);
				}

				if (pin_index == PIN_LENGTH) { // compare input_pin to current_pin
					if (CheckPIN(input_pin)) {
						pin_index = 0;
				        state = UNLOCKED;
				        lcd_write_flag = 1;
						HAL_Delay(500);
				    } else { //invalid key
						pin_index = 0;
						lcd_write_flag = 1;
						LCD_Command(CURSOR_OFF);
						HAL_Delay(500);
						LCD_Command(CLEAR_DISPLAY);
						LCD_WriteString("INVALID KEY:");
						LCD_Command(LINE_TWO);
						LCD_WriteString("TRY AGAIN");
						HAL_Delay(1000); // display error message for 1.5 seconds
						LCD_Command(DISPLAY_ON); // turn cursor back on
				    }
				}
				break;
			}

			case UNLOCKED: {
    			if (lcd_write_flag == 1) {
					GPIOA->ODR &= ~(1 << 5); // LED OFF
    				LCD_Command(CLEAR_DISPLAY);
                    LCD_Command(CURSOR_OFF);
    				LCD_WriteString("UNLOCKED");
    				LCD_Command(LINE_TWO);
    				LCD_WriteString("ANY KEY TO LOCK");
    				lcd_write_flag = 0;
    			}
                int input = KEYPAD_getKey();

				if (((input >= '0') && (input <= '9')) || (input == '*')) { // if a key 0-9 or * is pressed
				    lcd_write_flag = 1;
				    state = LOCKED;
				} else if (input == '#') {
					lcd_write_flag = 1;
					state = CHANGE_PIN;
				}

				if (input != -1) { // keypress detected
				    LCD_Command(CLEAR_DISPLAY); // clear display
				    while (KEYPAD_getKey() != -1) {} // wait until key is released before next state executes
					LCD_Command(DISPLAY_ON); // turn cursor back on
				}

				// unlocked behavior
				// we want -> CHANGE_PIN when # is pressed
				// if
				// and -> LOCKED when any other key is pressed.
				break;
			}

			case CHANGE_PIN: {
			    if(lcd_write_flag == 1){
					LCD_Command(CLEAR_DISPLAY);
					LCD_WriteString("CHANGE PIN");
					LCD_Command(LINE_TWO);
					LCD_WriteString("NEW PIN: ");
					lcd_write_flag = 0;
				}
				// check which numbers are being pressed and update current_pin
				int pin_value = KEYPAD_getKey();

				if((pin_value >= '0') && (pin_value <= '9')){ // 0-9 pressed
				    LCD_WriteChar(pin_value);
					HAL_Delay(5);
                    new_pin[pin_index] = pin_value;
                    pin_index++;
				} else if (pin_value == '*') { // * is pressed
					pin_index = 0;
					lcd_write_flag = 1;
				}

				if (pin_value != -1) { // keypress detected
				    while (KEYPAD_getKey() != -1) {} // wait until keypad is released before continuing
					HAL_Delay(50);
				}

                if(pin_index == PIN_LENGTH){
                    pin_index = 0;
                    for (int i = 0; i < PIN_LENGTH; i++) {
                        current_pin[i] = new_pin[i];
                    }
                        //transitioning out of the change_pin state to the locked state
                    LCD_Command(CLEAR_DISPLAY);
                    LCD_Command(CURSOR_OFF);
                    LCD_WriteString("NEW PIN SET TO:");
                    LCD_Command(LINE_TWO);
                    for (int i = 0; i < PIN_LENGTH; i++) {
                        LCD_WriteChar(current_pin[i]);
                    }
                    HAL_Delay(2000); //display confirmation for 2 seconds
                    LCD_Command(DISPLAY_ON); // turn cursor back on
                    lcd_write_flag = 1;
                    state = LOCKED;
                }
				break;
			}
		}
	}

 }
