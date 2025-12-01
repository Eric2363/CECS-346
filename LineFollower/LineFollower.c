// LineFollower.c
// Line Following Robot Starter program 
// Created by TYLOR FRANCA 
// February 2025
// CECS 497 SPRING 2025
// California State University, Long Beach
// Modified by Min He and Mark Joseph Austria
// 4/25/2025


#include "tm4c123gh6pm.h"
#include "Systick.h"
#include "Motors.h"
#include "Sensor.h"
#include <stdint.h>

// Function prototypes
void System_Init(void);

struct State {
  uint8_t motors;              // controls motor power supply
  uint16_t delay;              // Delay in ms
  uint8_t next[4];   					 // next state
};
typedef const struct State State_t;

enum states {Center,Left,Right,Stop};

State_t linefollower_fsm[]={
	//TODO: Fill out FSM
  // Straight
	// Left
	// Right
  // Stop
};

enum states curr_s;   // Initial state
uint8_t Input;

int main(void){
	uint8_t Input;
	
	System_Init();
	
	//TODO: Fill out starting state
	//curr_s =;
	
	while (1) {
		//TODO: Fill out FSM Engine	
		//MOTORS = ;
		//Wait_N_MS();
		//Input = ;
		//curr_s = ;
	}
}

void System_Init(void){
	Sensor_Init();
	SysTick_Init();
	Motor_Init();
}