// Motors.h
// Line Following Robot Starter program 
// Created by TYLOR FRANCA 
// February 2025
// CECS 497 SPRING 2025
// California State University, Long Beach
// Modified by Min He and Mark Joseph Austria
// 4/25/2025

#include "tm4c123gh6pm.h"
#include <stdint.h>

// Define motor outputs for different motions.
// PB7 for right motor and PB6 for left motor.
// PB5432 are the four direction pins:PB5:L:SLP,PB4:L:DIR, PB3:R:SLP,PB2:R:DIR
// SLP|DIR: 11: forward
// SLP|DIR: 10: backward
// TODO: find the constant values
#define FORWARD 		0   // PWM(bits7,6):11, direction:0x3C
#define TURN_LEFT 	0	  // PWM(bits7,6):10, direction:0x3C
#define TURN_RIGHT 	0   // PWM(bits7,6):01, direction:0x3C
#define STOP   			00


// bit address definitions for port data registers
// PB67 for motor PWM signals: PB6 - Left DC Motor, PB7 - Right DC Motor
// PB5432 are the four direction pins:PB5:L:SLP,PB4:L:DIR, PB3:R:SLP,PB2:R:DIR
// TODO: find the bit addresses
#define MOTORS (*((volatile uint32_t *)0)) 

// This function initialize PB7-PB2 to output PWM signals and 
// direction signals for Two DC Motors:
// PB67 for motor PWM signals: PB6 - Left DC Motor, PB7 - Right DC Motor
// PB5432 for motor directions: PB54 - left DC Motor, PB32 - right DC Motor
void Motor_Init(void);
	
