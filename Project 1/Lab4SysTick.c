// Lab4SysTick.c
// Runs on TM4C123
// Provide functions that initialize the SysTick module and 
// function to generate a multiple of 10 milliseconds using busy wait.  
// Original author: Daniel Valvano,Jonathan W. Valvano September 11, 2013
// Modified by Min He on 9/24/2020, 2/22/2025.

#include <stdint.h>
#include "Lab4SysTick.h"

#define NVIC_ST_CTRL_R          (*((volatile uint32_t *)0xE000E010))
#define NVIC_ST_RELOAD_R        (*((volatile uint32_t *)0xE000E014))
#define NVIC_ST_CURRENT_R       (*((volatile uint32_t *)0xE000E018))
#define NVIC_ST_CTRL_COUNT      (0x00010000)  // Count flag
#define NVIC_ST_CTRL_CLK_SRC    (0x00000004)  // Clock Source
#define NVIC_ST_CTRL_ENABLE     (0x00000001)  // Counter mode
#define QUARTER_SEC 					  (4000000)  // reload value for 250ms. Reload_Value = Time_Wanted_ms *  16Mhz



// Initialize SysTick with busy wait running at bus clock. 
// Do not enable SysTick timer in this function
void SysTick_Init(void){
	
	NVIC_ST_CTRL_R &=~NVIC_ST_CTRL_ENABLE;
	NVIC_ST_CTRL_R |= NVIC_ST_CTRL_CLK_SRC;
}

// Time delay using busy wait assumes 16 MHz system clock.
// This function generate a time delay of 0.5s.
// Implementaion requirement: enable the timer before generating delay and 
// disable it after 0.5s is generated.
// NOTE: Use SysTick_Wait10ms as an example to learn how to generate 0.5s delay
void SysTick_Wait_HalfSec(void){
	
	NVIC_ST_RELOAD_R = QUARTER_SEC - 1;  // Reload value = 4,000,000 for 250ms delay.
	NVIC_ST_CURRENT_R = 0;					  // any value clears current  
	NVIC_ST_CTRL_R |= NVIC_ST_CTRL_ENABLE;		// Enable SysTick timer
	while((NVIC_ST_CTRL_R & NVIC_ST_CTRL_COUNT) == 0) {} // wait all 4,000,000 counts until count flag is done
		NVIC_ST_CTRL_R &=~ NVIC_ST_CTRL_ENABLE;						// disable SysTick timer
	
}

// This function will generate multiple 0.25s delay
// Parameter:
// n_half_s: specify how many 0.25s will be generated
// maximum delay can be generated: 2^32*0.25s
void Wait_N_Half_Sec( uint32_t n_half_s) {
		
	uint32_t i;
      for (i=0;i<n_half_s;i++) {
         SysTick_Wait_HalfSec();
      }
}

						
