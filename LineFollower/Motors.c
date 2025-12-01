// Motor.c
// Line Following Robot Starter program 
// Created by TYLOR FRANCA 
// February 2025
// CECS 497 SPRING 2025
// California State University, Long Beach
// Modified by Min He and Mark Joseph Austria
// 4/25/2025


#include "tm4c123gh6pm.h"
#include <stdint.h>

// bits 6,7 are PWM outputs connected to DC Motor: bit 6->left motor, bit 7->right motor
// PB5432 for motor directions: PB54 - left DC Motor, PB32 - right DC Motor
#define MOTOR_PINS  0x00

// This function initialize PB7-PB2 to output PWM signals and 
// direction signals for Two DC Motors:
// PB67 for motor PWM signals: PB6 - Left DC Motor, PB7 - Right DC Motor
// PB5432 for motor directions: PB54 - left DC Motor, PB32 - right DC Motor
void Motor_Init(void){
}

