/*******************************************************************************
 * @file           : main.c
 * @brief          : Main program body for assignment 4
 *******************************************************************************/


#include "main.h"
#include "stm32l476xx.h"

#define TIMER_PIN 5
#define TIM_CC1IE_BIT 1


void timer_init(void){

	RCC->APB1ENR|=RCC_APB1ENR_TIM2EN; //enable timer
	TIM2->PSC=0; //we want 5kHz wave with 4MHz input --- means divide by 1 so the timer runs at the full 4mHz

	// ARR is counts needed per period = 4000000/5000 = 800
	TIM2->ARR = 800-1; // 799 since counter goes from 0 to 799 which is 800 counts

	// counter restarts at 0 on each update event --> reaches 200 after 200 ticks which is 25% through period
	TIM2->CCR1 = 200; // 25% of 800  = 200	

	TIM2->DIER|=TIM_DIER_UIE; // update event can interrupt -- counter reached 0/rollover
	TIM2->DIER |= TIM_DIER_CC1IE; // CCR1 match can interrupt -- counter reached 200
	NVIC_EnableIRQ(TIM2_IRQn); //CPU accepts TIM2 interrupts

	TIM2->CR1|= TIM_CR1_CEN; // start counting
}

void TIM2_IRQHandler(void){
	

	//if CC1IF is 1 then that means interrupt happened because of counter reaching 200 -- this is when we turn PA5 to LOW
	if(TIM2->SR & TIM_SR_CC1IF){
		// turning PA5 to LOW
		GPIOA->ODR &= ~(1 << TIMER_PIN);
		TIM2->SR &= ~(TIM_SR_CC1IF);
	}
	
	if(TIM2->SR & TIM_SR_UIF){
		GPIOA->ODR |= (1 << TIMER_PIN);
		TIM2->SR &= ~(TIM_SR_UIF);
	}

}



int main(void)
{
	HAL_Init();
	RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOAEN); // Enable GPIOA clock

	GPIOA->MODER &= ~(0x3U << (TIMER_PIN*2)); // clearing PA5
	GPIOA->MODER|=GPIO_MODER_MODE5_0;//set PA5 as output -- can monitor clock cycles with this pin

	// set P5 HIGH 
	GPIOA->ODR |= (1 << TIMER_PIN);
	

	timer_init();


	while(1) {

	}

 }
