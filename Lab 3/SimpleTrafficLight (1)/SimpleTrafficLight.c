// CECS346 Lab 3: FSM
// Team members:
//Eric Santana
//Bao Luong
//Noah Luu
//Thaisinge Kour

// Lab description:Implement a Moore Finite State Machine
//that control the changing of LED traffic lights when traffic or person is detected.

// Hardware Design
// 1)	Port E will be used to control 4 LEDs: white(PE3), red (PE2), yellow (PE1), green (PE0).
// 2)	Port A will be used for the two switches: sw1 (PA2), sw2 (PA3)

#include <stdint.h>   // for data type alias


// Registers for switches
// Complete the following register definitions
#define SENSOR							(*((volatile unsigned long *)0x40004030))	// bit addresses for the two switches/Sensors: bits 2&3
#define GPIO_PORTA_DATA_R 	(*((volatile unsigned long *)0x400043FC))    
#define GPIO_PORTA_DIR_R   	(*((volatile unsigned long *)0x40004400))     
#define GPIO_PORTA_AFSEL_R  (*((volatile unsigned long *)0x40004420))   
#define GPIO_PORTA_DEN_R   	(*((volatile unsigned long *)0x4000451C))     
#define GPIO_PORTA_AMSEL_R  (*((volatile unsigned long *)0x40004528))   
#define GPIO_PORTA_PCTL_R 	(*((volatile unsigned long *)0x4000452C)) 
       

//// Registers for LEDs
#define LIGHT               (*((volatile unsigned long *)0x4002403C))  // bit addresses for the four LEDs
#define GPIO_PORTE_DIR_R  	(*((volatile unsigned long *)0x40024400))      
#define GPIO_PORTE_AFSEL_R 	(*((volatile unsigned long *)0x40024420))     
#define GPIO_PORTE_DEN_R   	(*((volatile unsigned long *)0x4002451C))     
#define GPIO_PORTE_AMSEL_R 	(*((volatile unsigned long *)0x40024528))     
#define GPIO_PORTE_PCTL_R   (*((volatile unsigned long *)0x4002452C))   
#define SYSCTL_RCGC2_R    	(*((volatile unsigned long *)0x400FE108))     

// Register A Setup Input
#define SYSCTL_RCGC2_GPIOA   	0x00000001     		// port A Clock Gating Control
#define PORTA_DIR_CODE 				0x0C 							// A3 & A2 pins set as input
#define PORTA_AFSEL_CODE 			0x00 							// Disable AFSEL
#define PORTA_DEN_CODE 				0x0C 							// Enable pins A3 & A2
#define PORTA_AMSEL_CODE 			0x00							//Disable AMSEL
#define PORTA_PCTL_CODE 			0x00000FFFF 			// Disable PCTL


// Refister E Setup Output
#define SYSCTL_RCGC2_GPIOE   	0x00000010     	// port E Clock Gating Control
#define PORTE_DIR_CODE 				0x0F						// Set Bits E0 - E3 as output
#define PORTE_AFSEL_CODE 			0x00 						//Disable AFSEL 
#define PORTE_DEN_CODE 				0x0F 						// Enable Bits E0 - E3
#define PORTE_AMSEL_CODE 			0x00 						// Disable AMSEL
#define PORTE_PCTL_CODE 			0x0000FFFF 			// Disable PCTL



//Time definistions
#define HALF_SEC_Generator  (532776)   				// TODO: define the constant for generating 0.5 second delay
#define HALF_Sec 1
#define ONE_SEC 2
#define TWO_SEC 4

// LED State Outputs
#define GREEN 0x01
#define YELLOW 0x02
#define RED 0x04
#define WHITE 0x08
#define REDWHITE 0x0C


void Delay(uint8_t n);
void Light_Init(void);
void Sensor_Init(void);

// FSM state data structure
struct State {
  uint8_t Out; 
  uint8_t Time;  
  uint8_t Next[4];
}; 

typedef const struct State STyp;

//State 0: Green LED is on, output 0x01, wait 2 seconds
//State 1: Yellow LED is on, output 0x02, wait 1 second
//State 2: Red & White LED is on, output 0x0C, wait 2 seconds
//State 3: White LED is off, & RED LED is ON, output 0x04, wait 0.5 seconds
//State 4: White LED is ON & RED LED is on, output 0x0C, wait 0.5 seconds

// Constants definitions
enum my_states {State0,State1,State2,State3,State4};

// Output pins are:3(white), 2(red), 1(yellow), 0(green)
// Input pins are: 1:sw2, 0:sw1 

 STyp FSM[5]={
 {GREEN,TWO_SEC,{State1,State1,State0,State0}},// State 0
 {YELLOW,ONE_SEC,{State2,State2,State2,State2}},// State 1
 {REDWHITE,TWO_SEC,{State3,State2,State3,State2}}, // State 2
 {RED, HALF_Sec,{State4,State4,State4,State4}},// State 3
 {REDWHITE, HALF_Sec,{State0,State0,State0,State0}} // State 4
 };  

int main(void){ 
  uint8_t S;  // index to the current state 
  uint8_t Input; 
	
	Light_Init();
	Sensor_Init();
  
	S = State0;                     // FSM start with green  
    
  while(1){
		// Put your FSM engine here
		
		LIGHT = FSM[S].Out;
		//Delay(7);
		Delay(FSM[S].Time);
		//Delay(7);
		Input = (SENSOR >> 2);
		//Delay(7);
		S = FSM[S].Next[Input];
	//	Delay(7);
  }
}

void Delay(uint8_t n){
	volatile uint32_t time;
	
  time = n*HALF_SEC_Generator;  // 0.5sec
  while(time){
		time--;
  }
}

// PORT A SW1 & SW1 Inputs
void Sensor_Init(void){
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOA;      // Activate Port A clocks
	while ((SYSCTL_RCGC2_R & SYSCTL_RCGC2_GPIOA) != SYSCTL_RCGC2_GPIOA){} // wait for clock to be active
		
	GPIO_PORTA_AMSEL_R &=~(PORTA_AMSEL_CODE); // Disable analog function on PA3-2
  GPIO_PORTA_PCTL_R 	&=~(PORTA_PCTL_CODE);  // Enable regular GPIO
  GPIO_PORTA_DIR_R   &=~( PORTA_DIR_CODE);  // Inputs on PA3-2
  GPIO_PORTA_AFSEL_R  &=~(PORTA_AFSEL_CODE);// Regular function on PA3-2	
  GPIO_PORTA_DEN_R   |= PORTA_DEN_CODE;  // Enable digital signals on PA3-2
		
}

//LED Out PUT
void Light_Init(void){
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOE;      // Activate Port E clocks
	while ((SYSCTL_RCGC2_R & SYSCTL_RCGC2_GPIOE) != SYSCTL_RCGC2_GPIOE){} // wait for clock to be active
//		
  GPIO_PORTE_AMSEL_R &=~(PORTE_AMSEL_CODE); // Disable analog function on PE3-0
  GPIO_PORTE_PCTL_R &=~(PORTE_PCTL_CODE);  // Enable regular GPIO
  GPIO_PORTE_DIR_R  |= PORTE_DIR_CODE;  // Outputs on PE3-0
  GPIO_PORTE_AFSEL_R  &=~(PORTE_AFSEL_CODE);  // Regular function on PE3-0
  GPIO_PORTE_DEN_R  |= (PORTE_DEN_CODE);    // Enable digital on PE3-0
}
