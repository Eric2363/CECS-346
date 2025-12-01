// Systick.c
// Line Following Robot Starter program 
// Created by TYLOR FRANCA 
// February 2025
// CECS 497 SPRING 2025
// California State University, Long Beach
// Modified by Min He and Mark Joseph Austria
// 4/25/2025


#include "tm4c123gh6pm.h"
#include "Systick.h"
#include <stdint.h>

#define ONE_MILLI_S	// SysTick timer reload value for one millisecond, assume 16MHz system clock.
#define ONE_MICRO_S // SysTick timer reload value for one microsecond, assume 16MHz system clock.

void SysTick_Init(void){
  // disable SysTick during setup
  
	// set SysTick timer to use core clock: 16MHz
	
}

void DelayMs(void){	
	//set Reload to ONE_MILLI_S
	
  // Set Current to 0                                       
  
	// enable SysTick timer
	
	// wait for COUNT bit in control register to be raised.
	
  // disable SysTick timer
	
}

// Time delay using busy wait.
// This function will generate multiple of millisecond.
// Input: 32-bit interger for multiple of ms
void Wait_N_MS(uint32_t delay){	
}

void DelayUs(void){
	//set Reload to ONE_MICRO_S
	
  // Set Current to 0                                       
  
	// enable SysTick timer
	
	// wait for COUNT bit in control register to be raised.
	
  // disable SysTick timer

}

// Time delay using busy wait.
// This function will generate multiple of microsecond.
// Input: 32-bit interger for multiple of us
void Wait_N_US(uint32_t delay){	
}
