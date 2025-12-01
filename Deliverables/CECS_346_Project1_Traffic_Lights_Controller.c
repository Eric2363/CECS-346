// CECS346 Project 1: Traffic Light Controller
// Team members:
//Eric Santana
//Bao Luong
//Noah Luu
//Thaisinge Kour

// Project Description:Implement a Moore Finite State Machine
//	that control the changing of LED traffic lights when traffic or person is detected.

// Hardware Design
// Input: PORTE 
//	SOUTH PE2, WEST PE1, Pedestrian PE0

// Output: PORTB
// WEST Traffic Light Outputs
// RED PB5, YELLOW PB4, GREEN PB3
// SOUTH Traffic Light Outputs
// RED PB2, YELLOW PB1, GREEN PB0

// Pedestrian Output
//Output: PORTF
// Walk-F3,Don't Walk-F1

#include <stdint.h>   
#include  "Lab4SysTick.h"

//Port E  Addresses: Port E is an Input
#define SENSORS							(*((volatile unsigned long *)0x4002401C))	//Bit address: E2,E1,E0
#define GPIO_PORTE_DATA_R 	(*((volatile unsigned long *)0x400243FC))    
#define GPIO_PORTE_DIR_R   	(*((volatile unsigned long *)0x40024400))     
#define GPIO_PORTE_AFSEL_R  (*((volatile unsigned long *)0x40024420))   
#define GPIO_PORTE_DEN_R   	(*((volatile unsigned long *)0x4002451C))     
#define GPIO_PORTE_AMSEL_R  (*((volatile unsigned long *)0x40024528))   
#define GPIO_PORTE_PCTL_R 	(*((volatile unsigned long *)0x4002452C))     

// Port B Addresses: Port B is an Output
#define T_LIGHTS            (*((volatile unsigned long *)0x400050FC))
#define GPIO_PORTB_DIR_R  	(*((volatile unsigned long *)0x40005400))      
#define GPIO_PORTB_AFSEL_R 	(*((volatile unsigned long *)0x40005420))     
#define GPIO_PORTB_DEN_R   	(*((volatile unsigned long *)0x4000551C))     
#define GPIO_PORTB_AMSEL_R 	(*((volatile unsigned long *)0x40005528))     
#define GPIO_PORTB_PCTL_R   (*((volatile unsigned long *)0x4000552C))  	

// Port F Addresses: Port F is an Output
#define P_LIGHTS            (*((volatile unsigned long *)0x40025028))
#define GPIO_PORTF_DIR_R  	(*((volatile unsigned long *)0x40025400))      
#define GPIO_PORTF_AFSEL_R 	(*((volatile unsigned long *)0x40025420))     
#define GPIO_PORTF_DEN_R   	(*((volatile unsigned long *)0x4002551C))     
#define GPIO_PORTF_AMSEL_R 	(*((volatile unsigned long *)0x40025528))     
#define GPIO_PORTF_PCTL_R   (*((volatile unsigned long *)0x4002552C))
#define GPIO_PORTF_LOCK_R 	(*((volatile unsigned long *)0x40025520))
#define GPIO_PORTF_CR_R			(*((volatile unsigned long *)0x40025524))
	

#define SYSCTL_RCGC2_R    	(*((volatile unsigned long *)0x400FE108))     

// PORT E Register Setup: Input
#define SYSCTL_RCGC2_GPIOE   	0x00000010     		// port E Clock Gating Control
#define PORTE_DIR_CODE 				0x07 							// Selecting E2-E0: Must clear before setting &=~
#define PORTE_AFSEL_CODE 			0x00 							// Disable AFSEL
#define PORTE_DEN_CODE 				0x07 							// Enable pins E2, E1, E0
#define PORTE_AMSEL_CODE 			0x00							//Disable AMSEL
#define PORTE_PCTL_CODE 			0x00000FFFF 			// Disable PCTL


// PORT B Register Setup: Output
#define SYSCTL_RCGC2_GPIOB   	0x00000002     	// port B Clock Gating Control
#define PORTB_DIR_CODE 				0x3F						// Set Bits B5-B0 as output
#define PORTB_AFSEL_CODE 			0x00 						//Disable AFSEL 
#define PORTB_DEN_CODE 				0x3F 						// Enable Bits B5-B0
#define PORTB_AMSEL_CODE 			0x00 						// Disable AMSEL
#define PORTB_PCTL_CODE 			0x0000FFFF 			// Disable PCTL


// PORT F Register Setup: Output
#define SYSCTL_RCGC2_GPIOF   	0x00000020     	// port F Clock Gating Control
#define PORTF_DIR_CODE 				0x0A						// Set Bits F3,F1 as output
#define PORTF_AFSEL_CODE 			0x00 						//Disable AFSEL 
#define PORTF_DEN_CODE 				0x0A 						// Enable Bits F3,F1
#define PORTF_AMSEL_CODE 			0x00 						// Disable AMSEL
#define PORTF_PCTL_CODE 			0x0000FFFF 			// Disable PCTL
#define PORTF_LOCK_CODE				0x4C4F434B			// Unlock Port F
#define PORTF_CR_CODE					0x1F						// Allow Changes to port F: F4-F0

// Time def
#define ONE_SEC  							4
#define TWO_SEC  							8
#define QUARTER_SEC						1



// Traffic Light 
#define Go4South 0x21
#define wait4South 0x22
#define Go4West 0x0C
#define wait4West 0x14
#define CrossWalkActive 0x24

// pedestrian Light
#define PersonWait 	0x02
#define PersonGo 	 	0x08 //0x02
#define PersonHurry 0x00 //0x03

// Initialize
void T_Light_Init(void);
void P_Light_Init(void);
void Sensors_Init(void);

// FSM state data structure
struct State {
  uint8_t Out; 
	uint8_t P_Out;
  uint8_t Time; 
  uint8_t Next[8];
}; 

typedef const struct State STyp;

// Constants definitions
enum my_states {GoS,WaitS,GoW,WaitW,GoP,WaitPOn1,WaitPOff1,WaitPOn2,WaitPOff2};

// Output pins are:3(white), 2(red), 1(yellow), 0(green)
// Input pins are: 1:sw2, 0:sw1 

 STyp FSM[9]={
	 {Go4South,PersonWait,TWO_SEC,{GoS,WaitS,WaitS,WaitS,GoS,WaitS,WaitS,WaitS}},// State: GoS
	 {wait4South,PersonWait,ONE_SEC,{GoW,GoP,GoW,GoW,GoP,GoP,GoW,GoP}}, // State: WaitS
	 {Go4West,PersonWait,TWO_SEC,{GoW,WaitW,GoW,WaitW,WaitW,WaitW,WaitW,WaitW}}, // State: GoW
	 {wait4West,PersonWait,ONE_SEC,{GoS,GoP,GoS,GoP,GoS,GoP,GoS,GoS}}, // State: WaitW
	 {CrossWalkActive,PersonGo,TWO_SEC,{GoP,GoP,WaitPOn1,WaitPOn1,WaitPOn1,WaitPOn1,WaitPOn1,WaitPOn1}},// State: GoP
	 {CrossWalkActive,PersonHurry,QUARTER_SEC,{WaitPOff1,WaitPOff1,WaitPOff1,WaitPOff1,WaitPOff1,WaitPOff1,WaitPOff1,WaitPOff1}},// State: WaitPOn1
	 {CrossWalkActive,PersonWait,QUARTER_SEC,{WaitPOn2,WaitPOn2,WaitPOn2,WaitPOn2,WaitPOn2,WaitPOn2,WaitPOn2,WaitPOn2}}, // State: WaitPOff1
	 {CrossWalkActive,PersonHurry,QUARTER_SEC,{WaitPOff2,WaitPOff2,WaitPOff2,WaitPOff2,WaitPOff2,WaitPOff2,WaitPOff2,WaitPOff2}},// State WaitPOn2
	 {CrossWalkActive,PersonWait,QUARTER_SEC,{GoS,GoP,GoW,GoW,GoS,GoS,GoS,GoW}}// State: WaitPOff2
	 
 };  

int main(void){ 
  uint8_t S;  // index to the current state 
  uint8_t Input; 
	
	T_Light_Init();
	P_Light_Init();
	Sensors_Init();
  SysTick_Init();
	
	S = GoS;                     // FSM start with GoS  
    
  while(1){
		// Put your FSM engine here
		
		T_LIGHTS = FSM[S].Out;
		P_LIGHTS = FSM[S].P_Out;
		
		Wait_N_Half_Sec(FSM[S].Time);
		
		Input = SENSORS ;
		
		S = FSM[S].Next[Input];
	
  }
}



// PORT A SW1 & SW1 Inputs
void Sensors_Init(void){
	
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOE;      // Activate Port E clocks
	while ((SYSCTL_RCGC2_R & SYSCTL_RCGC2_GPIOE) != SYSCTL_RCGC2_GPIOE){} // wait for clock to be active
		
	GPIO_PORTE_AMSEL_R &=~(PORTE_AMSEL_CODE); // Disable analog function on PE2-E0
  GPIO_PORTE_PCTL_R  &=~(PORTE_PCTL_CODE);  // Enable regular GPIO
  GPIO_PORTE_DIR_R   &=~( PORTE_DIR_CODE);  // Inputs on E2-E0
  GPIO_PORTE_AFSEL_R &=~(PORTE_AFSEL_CODE);// Disable AFSEL
  GPIO_PORTE_DEN_R   |= PORTE_DEN_CODE;  // Enable digital signals on E2-E0
		
}

//LED Out PUT
void T_Light_Init(void){
	
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOB;      // Activate Port E clocks
	while ((SYSCTL_RCGC2_R & SYSCTL_RCGC2_GPIOB) != SYSCTL_RCGC2_GPIOB){} // wait for clock to be active
		
  GPIO_PORTB_AMSEL_R &=~(PORTB_AMSEL_CODE); // Disable analog function 
  GPIO_PORTB_PCTL_R &=~(PORTB_PCTL_CODE);  // Enable regular GPIO
  GPIO_PORTB_DIR_R  |= PORTB_DIR_CODE;  // Outputs on B5-B0
  GPIO_PORTB_AFSEL_R  &=~(PORTB_AFSEL_CODE);  // Regular function on B5-B0
  GPIO_PORTB_DEN_R  |= (PORTB_DEN_CODE);    // Enable digital on B5-B0
}

void P_Light_Init(void){
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOF;
	while ((SYSCTL_RCGC2_R & SYSCTL_RCGC2_GPIOF) != SYSCTL_RCGC2_GPIOF){} // wait for clock to be active
		
	GPIO_PORTF_LOCK_R |= PORTF_LOCK_CODE; // unlock port F
	GPIO_PORTF_CR_R		|= PORTF_CR_CODE;		// allow changes to F pins
  GPIO_PORTF_AMSEL_R &=~(PORTF_AMSEL_CODE); // Disable analog function 
  GPIO_PORTF_PCTL_R &=~(PORTF_PCTL_CODE);  // Enable regular GPIO
  GPIO_PORTF_DIR_R  |= PORTF_DIR_CODE;  // Outputs on B5-B0
  GPIO_PORTF_AFSEL_R  &=~(PORTF_AFSEL_CODE);  // Regular function 
  GPIO_PORTF_DEN_R  |= (PORTF_DEN_CODE);    // Enable digital on F3,F1
	
	
}
