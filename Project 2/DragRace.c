// DragRace.c
// Starter file for CECS346 Project 2
// By Eric Santana
//Bao Luong
//Noah Luu
//Thaisinge Kour
//Group: 2
// 4/9/2025

#include "tm4c123gh6pm.h"
#include <stdint.h> // C99 data types
#include <stdbool.h> // provides boolean data type
#include "PLL.h"
#include "SysTick.h"
#include "Sensors_Lights.h"

// Function Prototypes
// External functions from startup.s
extern void DisableInterrupts(void); // Disable interrupts
extern void EnableInterrupts(void);  // Enable interrupts
extern void WaitForInterrupt(void);  // Go to low power mode while waiting for the next interrupt
extern void Debounce(void);

// Functions defined in this file.
void System_Init(void);

// TODO: define bit addresses for two sensors, 8 color lights, and one reset button 
#define SENSORS 								(*((volatile unsigned long *)    0x40004030    )) // bit addresses for 2 sensors 
#define LIGHTS              		(*((volatile unsigned long *)    0x400053FC     )) // bit addresses for 8 Race Lights  PORT B
#define RESET                   (*((volatile unsigned long *)    0x40024004     )) // bit address for one reset button

// TODO: define number of states for FSM
#define NUM_STATE            (11)

// TODO: FSM definition
struct State { 
	uint8_t Out;
	uint8_t Time;     // multiple of 0.5 second
	uint8_t Next[4];  
};

typedef const struct State STyp;

// TODO: define reload value for half second
#define HALF_SEC_RELOAD_VALUE   (8000000U)
#define ONE_SEC 2
#define Half_SEC 1

// TODO: assign a value to all states in Drag Race FSM
// use all upper case for constants defined here
enum DragRace_states {INIT, Wait_For_Staging, Countdown_Y1, Countdown_Y2, Go, False_Start_Left,
	
False_Start_Right, False_Start_Both, Win_Left, Win_Right, Win_Both};

// TODO: define Outputs for the FSM 
#define ALL_ON 	    (0xFF) // Turns on all the LEDs 
#define ALL_OFF 	  (0x00) // Turns off all the LEDs 
#define YELLOW1_ON	(0x11) // Turns on 1st set of yellow LEDs 
#define YELLOW2_ON  (0x22) // Turns on 2nd set of yellow LEDs
#define GREEN_BOTH  (0x44) // Turns on the green LEDs 
#define GREEN_LEFT  (0x04) // Turns on left green LED
#define GREEN_RIGHT (0x40) // Turns on right green LED
#define RED_BOTH    (0x88) // Turns on both red LEDS
#define RED_LEFT    (0x80) // Turns on left red LED
#define RED_RIGHT   (0x08) // Turns on right red LED 

//TODO: Define Drag Race FSM
STyp DragRace_FSM[NUM_STATE] = {
	//Initialize
{ALL_ON,ONE_SEC,{Wait_For_Staging,Wait_For_Staging,Wait_For_Staging,Wait_For_Staging}},
//Wait For Staging
{ALL_OFF,Half_SEC,{Wait_For_Staging,Wait_For_Staging,Wait_For_Staging,Countdown_Y1}},
//Countdown Y1
{YELLOW1_ON,Half_SEC,{False_Start_Both,False_Start_Left,False_Start_Right,Countdown_Y2}},
//Countdown Y2
{YELLOW2_ON,Half_SEC,{False_Start_Both,False_Start_Left,False_Start_Right,Go}},
//Go
{GREEN_BOTH,Half_SEC,{Go,Win_Right,Win_Left,Win_Both}},
//False Start Left
{RED_LEFT,ONE_SEC,{Wait_For_Staging,Wait_For_Staging,Wait_For_Staging,Wait_For_Staging}},
//False Start Right
{RED_RIGHT,ONE_SEC,{Wait_For_Staging,Wait_For_Staging,Wait_For_Staging,Wait_For_Staging}},
//False Start Both
{RED_BOTH,ONE_SEC,{Wait_For_Staging,Wait_For_Staging,Wait_For_Staging,Wait_For_Staging}},
//Win Left
{GREEN_LEFT,ONE_SEC,{Wait_For_Staging,Wait_For_Staging,Wait_For_Staging,Wait_For_Staging}},
//Win Right
{GREEN_RIGHT,ONE_SEC,{Wait_For_Staging,Wait_For_Staging,Wait_For_Staging,Wait_For_Staging}},
//Win Both
{GREEN_BOTH,ONE_SEC,{Wait_For_Staging,Wait_For_Staging,Wait_For_Staging,Win_Both}}

};
	
// TODO: define bit positions for left, right and reset buttons
#define RESET_MASK  			0x01 // bit position for reset button
#define LEFT_SENSOR_MASK  (0x08) // bit position for left sensor
#define RIGHT_SENSOR_MASK (0x04) // bit position for right sensor
#define BOTH_SENSORS_MASK 0x0C //bit for both
	
uint8_t Input;
bool timesup;
bool reset;  // flag to reset the system, set by the reset button located at breadboard, not the launchpad reset button.
uint8_t S;
int main(void){
  uint8_t S;  // current state index
	
	System_Init();
	

  
  while(1){
    // TODO: reset FSM to its Initial state, reset globals to default values
     S = INIT;
		reset = false;
		Input = 0;
		
		while (!reset) {
			// TO Do: take care of FSM outputs and time in state.
			LIGHTS = DragRace_FSM[S].Out;
      SysTick_Start(DragRace_FSM[S].Time*HALF_SEC_RELOAD_VALUE);
			
			while((!timesup)&&(!reset)){
			  WaitForInterrupt();
			}
			timesup=false;			
			S = DragRace_FSM[S].Next[Input];
		}
		
  }
}

// Initialize the following hardware modules: PLL, Sensors, Reset button, Lights, SysTick timer
// Initialize the following golbal variables: timesup for FSM state time, reset for reset the whole system
void System_Init(void) {
	DisableInterrupts();
	PLL_Init();
  Sensors_Init(); 
	Reset_Init(); 
	Lights_Init();
	SysTick_Init(); 
  // TODO: reset global variables: timesup, reset, Input 
	
	timesup = false;
	reset = false;
	Input = 0;

	EnableInterrupts();
}

// Interrupt handler for the two sensors: update Input here 
void GPIOPortA_Handler(void){
	
	//Debounce();
 // Read which pin triggered interupt
	// Use GPIO_PORTA_RIS_R to see what pins triggered interupt
	if(GPIO_PORTA_RIS_R & LEFT_SENSOR_MASK){
		Input = SENSORS >> 2;
		GPIO_PORTA_ICR_R = LEFT_SENSOR_MASK;
	}
	if(GPIO_PORTA_RIS_R & RIGHT_SENSOR_MASK){
		Input = SENSORS >> 2;
		GPIO_PORTA_ICR_R = RIGHT_SENSOR_MASK;
	}
	

	
}

// Interrupt handler for reset button:  
// update global variable: reset
void GPIOPortE_Handler(void) {
	//Debounce();
	if(GPIO_PORTE_RIS_R & RESET_MASK){
		reset = true;
		GPIO_PORTE_ICR_R = RESET_MASK;
	}
}

// Systick interrupt handler:
// Stop systick timer and update global variable: timesup 
void SysTick_Handler(void) {
	
	SysTick_Stop();
	timesup = true;
}

void Debounce(void){

	for(uint32_t time = 0; time < 160000; time++){}
}
