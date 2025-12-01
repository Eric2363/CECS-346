#include "tm4c123gh6pm.h"
#include <stdint.h>
#include <stdbool.h>

// ===== GPIO Addresses & Bit Masks =====
#define LED              (*((volatile uint32_t *)0x40025308)) // PF1 & PF3
#define SENSOR           (*((volatile uint32_t *)0x40024004)) // PE0
#define SERVO            (*((volatile uint32_t *)0x40005004)) // PB0

// ===== Bit Definitions =====
#define PF1_MASK         0x02
#define PF3_MASK         0x08
#define PE0_MASK         0x01
#define PB0_MASK         0x01

#define LED_RED          PF1_MASK
#define LED_GREEN        PF3_MASK
#define LED_ALL          (PF1_MASK | PF3_MASK)

#define SERVO_BIT_MASK   PB0_MASK
#define SERVO_PCTL_MASK  0x0000000F

// ===== System Clock =====
#define SYSTEM_CLOCK_HZ       16000000  // 16 MHz

// ===== Servo Pulse Timing (at 16MHz) =====
#define SERVO_PERIOD_CYCLES   320000    // 20 ms
#define SERVO_CW_PULSE        40000     // ~2.5 ms
#define SERVO_CCW_PULSE       16000     // ~1.0 ms
#define SERVO_CENTER_PULSE    24000     // ~1.5 ms

// ===== Interrupts & Priorities =====
#define GPIO_PORTE_IRQ_NUM     4
#define GPIO_PORTE_PRI_MASK    0xFFFFFF1F
#define GPIO_PORTE_PRI_LEVEL   0x00000060 // Priority 3 (bits 5:7 in PRI1)

// ===== SysTick Configuration =====
#define SYSTICK_DISABLE_MASK   0x00000000
#define SYSTICK_ENABLE_MASK    0x00000005
#define SYSTICK_INT_ENABLE     0x00000002
#define SYSTICK_CLK_SRC        0x00000004

// Servo pulse timing
static uint32_t H;
static uint32_t L;

extern void DisableInterrupts(void);
extern void EnableInterrupts(void);
extern void WaitForInterrupt(void);

// Prototypes
void Sensor_Init(void);
void Servo_Init(void);
void LED_Init(void);
void SysTick_Init(void);
void Drive_Servo(uint32_t angle);
void Drive_Servo_Direction(enum Servo_Dir dir);

enum Servo_Dir { CLOCKWISE, COUNTERCLOCKWISE };

bool move_servo;
enum Servo_Dir dir;
uint32_t led_state = 0;

int main(void){
  DisableInterrupts();
  Sensor_Init();    
  Servo_Init();
  LED_Init();
  SysTick_Init();

  // Set initial LED based on sensor
  LED = (SENSOR ? LED_RED : LED_GREEN);

  // Center servo at startup
  Drive_Servo(SERVO_CENTER_PULSE);

  EnableInterrupts();
  
  while(1){
    WaitForInterrupt();
    if (move_servo) {
      Drive_Servo_Direction(dir);
      LED = led_state;
      move_servo = false;
    }
  }
}

void SysTick_Init(void){
  NVIC_ST_CTRL_R = SYSTICK_DISABLE_MASK;
  NVIC_ST_RELOAD_R = 0x00FFFFFF;
  NVIC_ST_CURRENT_R = 0;
  NVIC_ST_CTRL_R = SYSTICK_ENABLE_MASK;
}

void LED_Init(void){
  SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R5; // Port F
  while((SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R5) == 0);
  GPIO_PORTF_LOCK_R = 0x4C4F434B;
  GPIO_PORTF_CR_R |= LED_ALL;
  GPIO_PORTF_DIR_R |= LED_ALL;
  GPIO_PORTF_DEN_R |= LED_ALL;
  GPIO_PORTF_AFSEL_R &= ~LED_ALL;
  GPIO_PORTF_AMSEL_R &= ~LED_ALL;
  GPIO_PORTF_PCTL_R &= ~0x0000F0F0;
}

void Sensor_Init(void){
  SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R4; // Port E
  while ((SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R4) == 0);
  GPIO_PORTE_DIR_R &= ~PE0_MASK;
  GPIO_PORTE_DEN_R |= PE0_MASK;
  GPIO_PORTE_AFSEL_R &= ~PE0_MASK;
  GPIO_PORTE_AMSEL_R &= ~PE0_MASK;
  GPIO_PORTE_IS_R &= ~PE0_MASK;
  GPIO_PORTE_IBE_R |= PE0_MASK;
  GPIO_PORTE_ICR_R = PE0_MASK;
  GPIO_PORTE_IM_R |= PE0_MASK;

  NVIC_PRI1_R = (NVIC_PRI1_R & GPIO_PORTE_PRI_MASK) | GPIO_PORTE_PRI_LEVEL;
  NVIC_EN0_R |= (1 << GPIO_PORTE_IRQ_NUM);
}

void Servo_Init(void){
  SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1; // Port B
  while((SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R1) == 0);
  GPIO_PORTB_AFSEL_R &= ~PB0_MASK;
  GPIO_PORTB_PCTL_R &= ~SERVO_PCTL_MASK;
  GPIO_PORTB_DIR_R |= PB0_MASK;
  GPIO_PORTB_AMSEL_R &= ~PB0_MASK;
  GPIO_PORTB_DEN_R |= PB0_MASK;
  move_servo = false;
}

void Drive_Servo(uint32_t angle){
  H = angle;
  L = SERVO_PERIOD_CYCLES - H;
  SERVO |= SERVO_BIT_MASK;
  NVIC_ST_RELOAD_R = H - 1;
  NVIC_ST_CURRENT_R = 0;
  NVIC_ST_CTRL_R |= NVIC_ST_CTRL_ENABLE;
}

void Drive_Servo_Direction(enum Servo_Dir dir){
  if (dir == CLOCKWISE)
    Drive_Servo(SERVO_CW_PULSE);
  else
    Drive_Servo(SERVO_CCW_PULSE);
}

void SysTick_Handler(void){
  NVIC_ST_CTRL_R &= ~NVIC_ST_CTRL_ENABLE;
  NVIC_ST_RELOAD_R = (SERVO & SERVO_BIT_MASK) ? (L - 1) : (H - 1);
  SERVO ^= SERVO_BIT_MASK;
  NVIC_ST_CURRENT_R = 0;
  NVIC_ST_CTRL_R |= NVIC_ST_CTRL_ENABLE;
}

void GPIOPortE_Handler(void){
  GPIO_PORTE_ICR_R = PE0_MASK;
  if (SENSOR) {
    dir = COUNTERCLOCKWISE;
    led_state = LED_RED;
  } else {
    dir = CLOCKWISE;
    led_state = LED_GREEN;
  }
  move_servo = true;
}
