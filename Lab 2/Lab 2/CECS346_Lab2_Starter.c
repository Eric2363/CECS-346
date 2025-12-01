// Documentation
// CECS346 Lab 2 - Switch and LED Interface
// Description: ???
// Student Name: Eric Santana
// ID: 015107467

// Input/Output:
//   PE0 - Connects to switch 1 // Read State
//   PE1 - Connects to switch 2 // Read State
//   PB0 - Connect to Green LED
//   PB1 - Connects to Yellow LED
//   PB2 - Connects to Red LED

// Preprocessor Directives
#include <stdint.h>


// 1. Pre-processor Directives Section
// Constant declarations to access port registers using 
// symbolic names instead of addresses
// Replace the ? on the following lines with address for the corresponding registers

// PORT E register address definitions
#define GPIO_PORTE_DATA_R       (*((volatile uint32_t *)0x400243FC))
#define GPIO_PORTE_DIR_R        (*((volatile uint32_t *)0x40024400))
#define SW 				              (*((volatile uint32_t *)0x4002400C))    // define PORTE, bits 0,1 bit addresses
#define GPIO_PORTE_AFSEL_R      (*((volatile unsigned long *)0x40024420))
#define GPIO_PORTE_DEN_R        (*((volatile unsigned long *)0x4002451C))
#define GPIO_PORTE_AMSEL_R      (*((volatile unsigned long *)0x40024528))
#define GPIO_PORTE_PCTL_R       (*((volatile unsigned long *) 0x4002452C))

// PORT B register address definitions
#define LIGHT 		              (*((volatile uint32_t *)0x4000501C))    // define PORTB, bits 0,1,2 bit addresses
#define GPIO_PORTB_DATA_R       (*((volatile unsigned long *)0x400053FC))
#define GPIO_PORTB_DIR_R        (*((volatile unsigned long *)0x40005400))
#define GPIO_PORTB_AFSEL_R      (*((volatile unsigned long *)0x40005420))
#define GPIO_PORTB_DEN_R        (*((volatile unsigned long *)0x4000551C))
#define GPIO_PORTB_AMSEL_R      (*((volatile unsigned long *)0x40005528))
#define GPIO_PORTB_PCTL_R       (*((volatile unsigned long *)0x4000552C))

// System control register RCGC2 definition
#define SYSCTL_RCGCGPIO          (*((volatile unsigned long *)0x400FE608))

// Constant definitions
#define SW1_MASK 	0x01           // PE0 for switch 1 // orange wire
#define SW2_MASK 	0x02            // PE1 for switch 2 // purple wire
#define RED 			0x04					 // PB2 for red LED
#define YELLOW 		0x02					   // PB1 for yellow LED
#define GREEN 		0x01					   // PB0 for green LED

// Function Prototypes - Each subroutine defined
void Delay(uint8_t n_500ms);
void PortB_Init(void);
void PortE_Init(void);

int main(void) {
	// Initialize GPIO Ports B, E
  PortB_Init();
	PortE_Init();
	// Initial state: Green LED on, the other two LEDs off
  LIGHT = GREEN;

	while(1) {
		// If sw1 is pressed, the currently on LED will be turned off, the next LED will be turned on. 
		
		if(SW == SW1_MASK && LIGHT == GREEN){	// Switch 1 is pressed and current LED is green
			
			LIGHT = LIGHT<< 0x01; // Shifts Light by 1 to the left. Results in Color change.
			while(SW == SW1_MASK);// Waits for SW1 to be released before moving on
		}
		
		else if(SW == SW1_MASK && LIGHT == YELLOW){ // button is pressed and current light is yellow
			
			LIGHT = LIGHT << 0x01; // Shifts Light by 1 to the left. Results in Color change.
			while(SW == SW1_MASK);// Waits for SW1 to be released before moving on
		}
		else if( SW == SW1_MASK && LIGHT == RED){// button is pressed and current light is red.
			
			LIGHT = LIGHT >> 2; // shifts light 2 to the right, which resets it back to green.
			while(SW == SW1_MASK);// Waits for SW1 to be released before moving on
		}
		
    // If sw2 is pressed, currently on LED will flash at a speed of 0.5s.
		//toggle light on and off then have delay
		
		while (SW == SW2_MASK){
			
				if (LIGHT == GREEN){//i
					
					while(SW ==SW2_MASK){
						
						LIGHT ^= GREEN;
						Delay(255);
					}
					LIGHT |= GREEN; // RESET TO GREEN
				/////////////////////////////////////////////////	
				} else if (LIGHT == YELLOW){
						while(SW == SW2_MASK){
							LIGHT ^= YELLOW;
							Delay(255);
						}
						LIGHT |= YELLOW; // RESET TO YELLOW
				///////////////////////////////////////////////////		
					}else if (LIGHT == RED){
							while(SW == SW2_MASK){
								LIGHT ^= RED;
								Delay(255);
							}
							LIGHT |= RED; // RESET TO RED
						}
					}
				
	}
		
    // If both sw1 and sw2 are pressed, current led will flash once at a speed of 0.5s 
		//    and then move to the next one.	
			
	}

// Subroutine to initialize port B pins for output
// PB2,PB1,PB0 are outputs for red, green, and yellow LEDs
// Inputs: None
// Outputs: None
void PortB_Init(void){   
	
	SYSCTL_RCGCGPIO  |= 0x00000002; // Activate B Clock
	while((SYSCTL_RCGCGPIO & 0x00000002) != 0x00000002){} // Wait for B clock to activate
	
	GPIO_PORTB_DIR_R |= 0x07; // Sets PB0,PB1,PB2 as outputs
	GPIO_PORTB_DEN_R |= 0x07;  //Enable digital pins PB0,PB1, PB2
	GPIO_PORTB_AFSEL_R &= ~ 0xFF; //Disable Alternate Function for all Port B Pins
	GPIO_PORTB_AMSEL_R  &= ~ 0x07; // Disable Analog mode for B0, B1, B2
	GPIO_PORTB_PCTL_R &=~ 0x00000FFF; // Clear Bits to Zero for normal digital operation for B0, B1, B2
		
		
		
}

// Subroutine to initialize port E pins PE0 & PE1 for input
// Inputs: None
// Outputs: None
void PortE_Init(void){
	
		SYSCTL_RCGCGPIO |= 0x00000010;
		while((SYSCTL_RCGCGPIO & 0x00000010) != 0x00000010) {}
			
		GPIO_PORTE_DIR_R &=~ 0x03; // Sets PE0 and PE1 to zero to set as input
		GPIO_PORTE_DEN_R |= 0x03; // Enable Digital pins  PE0 and PE1
		GPIO_PORTE_AFSEL_R &=~ 0xFF; // Disable Alternate Function for all port E pins
		GPIO_PORTE_AMSEL_R &=~ 0x03; // Disable Analog mode for PE0, PE1
		GPIO_PORTE_PCTL_R &=~ 0x000000FF; // Clear Bits to Zero for normal digial operation for E0, E1	
}

// Subroutine to wait about 0.5 sec
// Inputs: None
// Outputs: None
// Notes: the Keil simulation runs slower than the real board
void Delay(uint8_t n_500ms) {
	
	volatile uint32_t i,j;
	for(i=0; i < n_500ms; i++){
		for(j = 0; j < 2000; j++){
		}
	}
	
}
