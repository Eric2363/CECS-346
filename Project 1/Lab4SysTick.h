// Lab4SysTick.h
// Runs on TM4C123
// Provide functions that initialize the SysTick module and 
// function to generate a multiple of 10 milliseconds using busy wait.  
// Original author: Daniel Valvano,Jonathan W. Valvano September 11, 2013
// Modified by Min He on 9/24/2020, 2/22/2025.

#ifndef __LAB4SYSTICK_H__
#define __LAB4SYSTICK_H__

#include <stdint.h>
#include "Lab4SysTick.h"

// Initialize SysTick with busy wait running at bus clock. 
// Do not enable SysTick timer in this function
void SysTick_Init(void);

// Time delay using busy wait assumes 16 MHz system clock.
// This function generate a time delay of 0.5s.
// Implementaion requirement: enable the timer before generating delay and 
// disable it after 0.5s is generated.
// NOTE: Use SysTick_Wait10ms as an example to learn how to generate 0.5s delay
void SysTick_Wait_HalfSec(void);

// This function will generate multiple 0.5s delay
// Parameter:
// n_half_s: specify how many 0.5s will be generated
// maximum delay can be generated: 2^32*0.5s
void Wait_N_Half_Sec( uint32_t n_half_s) ;
#endif // __LAB4SYSTICK_H__