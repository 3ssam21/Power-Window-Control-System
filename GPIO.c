#include "GPIO.h"

bool debounce(uchar8 pin){
  static volatile uint32 state = 0;
  state = (state<<1) | DIO_ReadPin(pin) | 0xFE00;
  return (state == 0xFF00);
}

void DIO_WritePort ( volatile unsigned long * port, uchar8 value)
{
  *port = value;
  
}
void DIO_WritePin( uchar8 pin, uchar8 value)
{
  
   if (value == 1)
   {
     set_bit(GPIO_PORTF_DATA_R, pin);
   }
   else if (value == 0)
   {
     clear_bit(GPIO_PORTF_DATA_R, pin);
   }
}

void DIO_Init(void)
{
  SYSCTL_RCGCGPIO_R |= 0X20;
  while ((SYSCTL_RCGCGPIO_R & 0x20) ==0x00){}
  GPIO_PORTF_LOCK_R = 0x4C4F434B;
  GPIO_PORTF_CR_R = 0x1F;
  GPIO_PORTF_DIR_R = 0x0E;
  GPIO_PORTF_PUR_R = 0x11;
  GPIO_PORTF_DEN_R = 0x1F;
  
  GPIO_PORTF_DATA_R = 0x00;
  
  SYSCTL_RCGCGPIO_R |= 0X02;
  while ((SYSCTL_RCGCGPIO_R & 0x02) ==0x00){}
  GPIO_PORTB_LOCK_R = 0x4C4F434B;
  GPIO_PORTB_CR_R = 0x7F;
  GPIO_PORTB_DIR_R = 0x3;
  GPIO_PORTB_PUR_R = 0x7F;
  GPIO_PORTB_DEN_R = 0x7F;
  GPIO_PORTB_PCTL_R &= ~0x7F;
  GPIO_PORTB_AMSEL_R &= ~0x7F;
  GPIO_PORTB_AFSEL_R &= ~0x7F;
  
  
  GPIO_PORTB_DATA_R = 0x00;
  
  SYSCTL_RCGCGPIO_R |= 0X01;
  while ((SYSCTL_RCGCGPIO_R & 0x01) ==0x00){}
  GPIO_PORTA_LOCK_R = 0x4C4F434B;
  GPIO_PORTA_CR_R = 0x6C;
  GPIO_PORTA_DIR_R = 0x0;
  GPIO_PORTA_PUR_R = 0x6C;
  GPIO_PORTA_DEN_R = 0x6C;
  GPIO_PORTA_PCTL_R &= ~0x6C;
  GPIO_PORTA_AMSEL_R &= ~0x6C;
  GPIO_PORTA_AFSEL_R &= ~0x6C;
  
  
  GPIO_PORTA_DATA_R = 0x00;
}

uint32 DIO_ReadPort(volatile unsigned long * port)
{
   return (*port); 
}

uint32 DIO_ReadPin(uchar8 pin)
{
   return (get_bit(GPIO_PORTF_DATA_R, pin));
}

void delay(uint32 number_of_millis)
{
    // Converting time into milli_seconds
    uint32 milli_seconds = number_of_millis/1000 ;
 
    // Storing start time
    clock_t start_time = clock();
 
    // looping till required time is not achieved
    while (clock() < start_time + milli_seconds){};
}