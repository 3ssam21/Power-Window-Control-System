#include "tm4c123gh6pm.h"
#include "types.h"
#include "bitwise_operation.h"
#include <time.h>
#include <stdbool.h>


#define LED_RED 0x02
#define LED_BLUE 0x04
#define LED_GREEN 0x08
#define LED_WHITE 0x0E
#define pb1 4
#define pb2 0

#define PORTF_ADRESS ((volatile unsigned long *) 0x400253FC)

void DIO_WritePort ( volatile unsigned long * port, uchar8 value);
void DIO_WritePin ( uchar8 pin, uchar8 value);
void DIO_Init (void);
uint32 DIO_ReadPort(volatile unsigned long * port);
uint32 DIO_ReadPin(uchar8 pin);
bool debounce(uchar8 pin);
void delay(uint32 number_of_millis);