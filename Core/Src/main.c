/*******************************************************************************
 * @file           : main.c
 * @brief          : Main program body for assignment 4
 *******************************************************************************/


#include "main.h"
#include "stm32l476xx.h"

#define TIMER_PIN 5
#define TIM_CC1IE_BIT 1

void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  //RCC_OscInitStruct.MSIState = RCC_MSI_ON;  //datasheet says NOT to turn on the MSI then change the frequency.
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_10;
	/* from stm32l4xx_hal_rcc.h
	#define RCC_MSIRANGE_0                 MSI = 100 KHz
	#define RCC_MSIRANGE_1                 MSI = 200 KHz
	#define RCC_MSIRANGE_2                 MSI = 400 KHz
	#define RCC_MSIRANGE_3                 MSI = 800 KHz
	#define RCC_MSIRANGE_4                 MSI = 1 MHz
	#define RCC_MSIRANGE_5                 MSI = 2 MHz
	#define RCC_MSIRANGE_6                 MSI = 4 MHz
	#define RCC_MSIRANGE_7                 MSI = 8 MHz
	#define RCC_MSIRANGE_8                 MSI = 16 MHz
	#define RCC_MSIRANGE_9                 MSI = 24 MHz
	#define RCC_MSIRANGE_10                MSI = 32 MHz
	#define RCC_MSIRANGE_11                MSI = 48 MHz   dont use this one*/
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;  //datasheet says NOT to turn on the MSI then change the frequency.
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

void timer_init(void){

	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN; // enable timer
	TIM2->PSC=0; //we want 5kHz wave with 4MHz input --- means divide by 1 so the timer runs at the full 4mHz

	// ARR is counts needed per period = 4000000/5000 = 800
	TIM2->ARR = 6400-1; // 799 since counter goes from 0 to 799 which is 800 counts

	// counter restarts at 0 on each update event --> reaches 200 after 200 ticks which is 25% through period
	TIM2->CCR1 = 1600; // 25% of 800  = 200	

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
	SystemClock_Config();
	RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOAEN); // Enable GPIOA clock

	GPIOA->MODER &= ~(0x3U << (TIMER_PIN*2)); // clearing PA5
	GPIOA->MODER|=GPIO_MODER_MODE5_0;//set PA5 as output -- can monitor clock cycles with this pin

	// set P5 HIGH 
	GPIOA->ODR |= (1 << TIMER_PIN);
	

	timer_init();


	while(1) {

	}

 }
