#include "FreeRTOS.h"
#include "task.h"
#include "FreeRTOSConfig.h"
#include "tm4c123gh6pm.h"
#include "GPIO.h"
#include "types.h"
#include "queue.h"
#include "semphr.h"
#define mainSW_INTURRUPT_PortF ((IRQn_Type)30)

	

#define GPIO_PORTF_CLK_EN  0x20
#define GPIO_PORTF_PIN1_EN 0x02
#define GPIO_PORTF_PIN2_EN 0x04
#define GPIO_PORTF_PIN3_EN 0x08
#define RED                0x02
#define BLUE               0x04
#define GREEN              0x08

xSemaphoreHandle DUMSemaphore;
xSemaphoreHandle DDMSemaphore;
xSemaphoreHandle DUASemaphore;
xSemaphoreHandle DDASemaphore;
xSemaphoreHandle PUMSemaphore;
xSemaphoreHandle PDMSemaphore;
xSemaphoreHandle PUASemaphore;
xSemaphoreHandle PDASemaphore;

xQueueHandle JamQueue;



void Driver_Up_Manual(void *pvParameters);
void Driver_Down_Manual(void *pvParameters);
void Driver_Up_Auto(void *pvParameters);
void Driver_Down_Auto(void *pvParameters);
void Passenger_Up_Manual(void *pvParameters);
void Passenger_Down_Manual(void *pvParameters);
void Passenger_Up_Auto(void *pvParameters);
void Passenger_Down_Auto(void *pvParameters); 
void Jammed_Object(void *pvParameters);
void check_btn(void *pvParameters);

TaskHandle_t check_btn_handle;
static int Switch = 0;
static int value = 1;
static int tick = 10;


void Delay(void);

int main(void)
  {
	DIO_Init();
	GPIO_PORTB_DATA_R = 0x00; 

	DUMSemaphore = xSemaphoreCreateBinary();
	DDMSemaphore = xSemaphoreCreateBinary();
	DUASemaphore = xSemaphoreCreateBinary();
	DDASemaphore = xSemaphoreCreateBinary();
	PUMSemaphore = xSemaphoreCreateBinary();
	PDMSemaphore = xSemaphoreCreateBinary();
	PUASemaphore = xSemaphoreCreateBinary();
	PDASemaphore = xSemaphoreCreateBinary();
	 
	JamQueue = xQueueCreate(1, sizeof(int)); 
	
	
	xTaskCreate(check_btn,"check button",90,NULL,1,&	check_btn_handle);
	xTaskCreate(Driver_Up_Manual,"driver up manual",90,NULL,2,NULL);
	xTaskCreate(Driver_Down_Manual,"driver down manual",90,NULL,2,NULL);
	xTaskCreate(Driver_Up_Auto,"driver up auto",90,NULL,2,NULL);
	xTaskCreate(Driver_Down_Auto,"driver down auto",90,NULL,2,NULL);
	xTaskCreate(Passenger_Up_Manual,"passener up manual",90,NULL,2,NULL);
	xTaskCreate(Passenger_Down_Manual,"passenegr down manual",90,NULL,2,NULL);
	xTaskCreate(Passenger_Up_Auto,"passener up auto ",90,NULL,2,NULL);
	xTaskCreate(Passenger_Down_Auto,"passenger down auto",90,NULL,2,NULL); 
	xTaskCreate(Jammed_Object,"jam button",90,NULL,3,NULL);
	
	
	GPIO_PORTF_DATA_R = 0x00;   
	GPIO_PORTB_DATA_R = 0x00;   
	vTaskStartScheduler();
	
	for(;;){}
}


void check_btn(void *pvParameters){
	for(;;){
		//cheking for ON/OFF Switch for passenger
		if((GPIOB->DATA & (1<<6))== 0) {
			//change switch
			if (Switch == 0){							
				Switch = 1;
			}else{
				Switch = 0;
			}                             
		}
		
		//cheking the jam button
		if((GPIOF->DATA & (1<<4))== 0) {
			xQueueSendToBack(JamQueue,&value,0);
			
		}
		
		//driver window up manually
		if((GPIOB->DATA & (1<<3))== 0) {
			//vTaskPrioritySet(check_btn_handle,1);
			xSemaphoreGive(DUMSemaphore);
		}
		//driver window down manually
		if((GPIOB->DATA & (1<<2))== 0) {
			//vTaskPrioritySet(check_btn_handle,1);
			xSemaphoreGive(DDMSemaphore);
			
		}
		//driver window up automatic
		if((GPIOA->DATA & (1<<6))== 0) {
			//vTaskPrioritySet(check_btn_handle,1);
			xSemaphoreGive(DUASemaphore);
			
		}
		//driver window down automatic
		if((GPIOA->DATA & (1<<5))== 0) {
			//vTaskPrioritySet(check_btn_handle,1);
			xSemaphoreGive(DDASemaphore);
			
		}
		//passenger window up manually
		if((GPIOA->DATA & (1<<2))== 0 && Switch==0) {
			//vTaskPrioritySet(check_btn_handle,1);
			xSemaphoreGive(PUMSemaphore);
		}
		//passenger window down manually
		if((GPIOA->DATA & (1<<3))== 0 && Switch==0){
			//vTaskPrioritySet(check_btn_handle,1);
			xSemaphoreGive(PDMSemaphore);
		}
		//passenger window up automatic
		if((GPIOB->DATA & (1<<5))== 0 && Switch==0){
			//vTaskPrioritySet(check_btn_handle,1);
			xSemaphoreGive(PUASemaphore);
		}
		//passenger window down automatic
		if((GPIOB->DATA & (1<<4))== 0 && Switch==0){
		//	vTaskPrioritySet(check_btn_handle,1);
			xSemaphoreGive(PDASemaphore);
		}

		taskYIELD();
	}
}


void Driver_Up_Manual(void *pvParameters){
	for(;;)
	{
		xSemaphoreTake(DUMSemaphore, 0);
		xSemaphoreTake(DUMSemaphore, portMAX_DELAY);		//causing delay when semaphore is taken to get out after task is finished
		
		GPIO_PORTF_DATA_R = GPIO_PORTF_DATA_R|GREEN; 
		set_bit(GPIO_PORTB_DATA_R, 1);
		clear_bit(GPIO_PORTB_DATA_R, 0);
		
		while((GPIO_PORTB_DATA_R &(1<<3)) ==0 && (GPIOF->DATA & (1<<4)) != 0)
		{
				vTaskDelay(500);
		}
		GPIO_PORTF_DATA_R &= ~0x0E;	
		clear_bit(GPIO_PORTB_DATA_R, 1);
		clear_bit(GPIO_PORTB_DATA_R, 0);
		xSemaphoreGive(DUMSemaphore);
		xSemaphoreTake(DUMSemaphore, 0);                  //to make sure the functioin does't loop
		taskYIELD();
	}
}
void Driver_Down_Manual(void *pvParameters){
	for(;;)
	{
		xSemaphoreTake(DDMSemaphore, 0);
		xSemaphoreTake(DDMSemaphore, portMAX_DELAY);
		//GPIO_PORTF_DATA_R &= ~0x0E;				
		GPIO_PORTF_DATA_R = GPIO_PORTF_DATA_R|RED;	
		set_bit(GPIO_PORTB_DATA_R, 0);
		clear_bit(GPIO_PORTB_DATA_R, 1);
		
		
		
		while((GPIOB->DATA & (1<<2))== 0 && (GPIOF->DATA & (1<<4)) != 0)
		{
				vTaskDelay(500);
		}
		GPIO_PORTF_DATA_R &= ~0x0E;
		clear_bit(GPIO_PORTB_DATA_R, 1);
		clear_bit(GPIO_PORTB_DATA_R, 0);		
		xSemaphoreGive(DDMSemaphore);
		xSemaphoreTake(DDMSemaphore, 0);
		taskYIELD();
	}
}

void Driver_Up_Auto(void *pvParameters){
	for(;;)
	{
		
		xSemaphoreTake(DUASemaphore,0);
		xSemaphoreTake(DUASemaphore, portMAX_DELAY);
		//xSemaphoreTake(xMutex,portMAX_DELAY);
    
		//GPIO_PORTF_DATA_R &= ~0x0E;				
		GPIO_PORTF_DATA_R = GPIO_PORTF_DATA_R|GREEN;
		set_bit(GPIO_PORTB_DATA_R, 1);
		clear_bit(GPIO_PORTB_DATA_R, 0);

		while((tick>1) && (GPIOF->DATA & (1<<4)) != 0 ){
			vTaskDelay(500);
			tick--;
			if((GPIO_PORTF_DATA_R & 0x0E) == 0){
				tick =1;
			}
		}
		tick = 10;
		GPIO_PORTF_DATA_R &= ~0x0E;
		clear_bit(GPIO_PORTB_DATA_R, 1);
		clear_bit(GPIO_PORTB_DATA_R, 0);
		xSemaphoreGive(DUASemaphore);
		xSemaphoreTake(DUASemaphore, 0);
		taskYIELD();

	}
}
void Driver_Down_Auto(void *pvParameters){
	for(;;)
	{
		xSemaphoreTake(DDASemaphore,0);
		xSemaphoreTake(DDASemaphore, portMAX_DELAY);

		
		//GPIO_PORTF_DATA_R &= ~0x0E;				
		GPIO_PORTF_DATA_R = GPIO_PORTF_DATA_R|RED;
		set_bit(GPIO_PORTB_DATA_R, 0);
		clear_bit(GPIO_PORTB_DATA_R, 1);

		while((tick>1) && (GPIOF->DATA & (1<<4)) != 0 ){
			vTaskDelay(500);
			tick--;
			if((GPIO_PORTF_DATA_R & 0x0E) == 0){
				tick =1;
			}
		}
		tick = 10;
		GPIO_PORTF_DATA_R &= ~0x0E;
		clear_bit(GPIO_PORTB_DATA_R, 1);
		clear_bit(GPIO_PORTB_DATA_R, 0);
		xSemaphoreGive(DDASemaphore);
		xSemaphoreTake(DDASemaphore, 0);
		taskYIELD();

	}
}

void Passenger_Up_Manual(void *pvParameters){
	for(;;)
	{
		xSemaphoreTake(PUMSemaphore, 0);
		xSemaphoreTake(PUMSemaphore, portMAX_DELAY);
		//GPIO_PORTF_DATA_R &= ~0x0E;				
		GPIO_PORTF_DATA_R = GPIO_PORTF_DATA_R|GREEN;			
		set_bit(GPIO_PORTB_DATA_R, 1);
		clear_bit(GPIO_PORTB_DATA_R, 0);
		
		while((GPIOA->DATA & (1<<2))== 0 && (Switch==0) && (GPIOF->DATA & (1<<4)) != 0)
		{
			if((GPIOB->DATA & (1<<6))== 0) {
					Switch = 1;
			}
			vTaskDelay(500);
		}
		GPIO_PORTF_DATA_R &= ~0x0E;
		clear_bit(GPIO_PORTB_DATA_R, 1);
		clear_bit(GPIO_PORTB_DATA_R, 0);		
		xSemaphoreGive(PUMSemaphore);
		xSemaphoreTake(PUMSemaphore, 0);
		taskYIELD();
	}
}

void Passenger_Down_Manual(void *pvParameters){
	for(;;)
	{
			xSemaphoreTake(PDMSemaphore, 0);
			xSemaphoreTake(PDMSemaphore, portMAX_DELAY);
			
			//GPIO_PORTF_DATA_R &= ~0x0E;		
			GPIO_PORTF_DATA_R = GPIO_PORTF_DATA_R|RED;	
			set_bit(GPIO_PORTB_DATA_R, 0);
			clear_bit(GPIO_PORTB_DATA_R, 1);
		
	while((GPIOA->DATA & (1<<3))== 0 && (Switch==0) && (GPIOF->DATA & (1<<4)) != 0)
	{
			if((GPIOB->DATA & (1<<6))== 0) {
					Switch = 1;
			}
			vTaskDelay(500);
		}
		GPIO_PORTF_DATA_R &= ~0x0E;	
		clear_bit(GPIO_PORTB_DATA_R, 1);
		clear_bit(GPIO_PORTB_DATA_R, 0);
		xSemaphoreGive(PDMSemaphore);
		xSemaphoreTake(PDMSemaphore, 0);
		taskYIELD();
	}
}

void Passenger_Up_Auto(void *pvParameters){
	for(;;)
	{
		
		xSemaphoreTake(PUASemaphore, 0);
		xSemaphoreTake(PUASemaphore, portMAX_DELAY);

    
		//GPIO_PORTF_DATA_R &= ~0x0E;				
		GPIO_PORTF_DATA_R = GPIO_PORTF_DATA_R|GREEN;
		set_bit(GPIO_PORTB_DATA_R, 1);
		clear_bit(GPIO_PORTB_DATA_R, 0);

		while((tick>1) && (Switch == 0) && (GPIOF->DATA & (1<<4)) != 0 ){
			vTaskDelay(500);
			tick--;
			if((GPIO_PORTF_DATA_R & 0x0E) == 0){
				tick =1;
			}
		}
		tick = 10;
		GPIO_PORTF_DATA_R &= ~0x0E;
			clear_bit(GPIO_PORTB_DATA_R, 1);
			clear_bit(GPIO_PORTB_DATA_R, 0);
		xSemaphoreGive(PUASemaphore);
		xSemaphoreTake(PUASemaphore, 0);
		taskYIELD();

	}
}

void Passenger_Down_Auto(void *pvParameters){
	for(;;)
	{
		xSemaphoreTake(PDASemaphore, 0);
		xSemaphoreTake(PDASemaphore, portMAX_DELAY);

    
		//GPIO_PORTF_DATA_R &= ~0x0E;				
		GPIO_PORTF_DATA_R = GPIO_PORTF_DATA_R|RED;
		set_bit(GPIO_PORTB_DATA_R, 0);
		clear_bit(GPIO_PORTB_DATA_R, 1);

		while((tick>1) && (Switch == 0) && (GPIOF->DATA & (1<<4)) != 0 ){
			vTaskDelay(500);
			tick--;
			if((GPIO_PORTF_DATA_R & 0x0E) == 0){
				tick =1;
			}
		}
		
		tick = 10;
		GPIO_PORTF_DATA_R &= ~0x0E;
		clear_bit(GPIO_PORTB_DATA_R, 1);
		clear_bit(GPIO_PORTB_DATA_R, 0);
		xSemaphoreGive(PDASemaphore);
		xSemaphoreTake(PDASemaphore, 0);
		taskYIELD();

	}
}


void Jammed_Object(void *pvParameters) 
{
	for (;;)
	{
		
		xQueueReceive(JamQueue,&value,portMAX_DELAY); // Recieve from empty queue to block at the start of the task 
	
		while (((GPIOF -> DATA & (1<<4))) == 0) 
		{
			//clear led                     
			GPIO_PORTF_DATA_R &= ~0x0E;
			// Clear motor pins
			clear_bit(GPIO_PORTB_DATA_R, 1);
			clear_bit(GPIO_PORTB_DATA_R, 0);
		}
			xQueueReceive(JamQueue,&value,portMAX_DELAY);
			taskYIELD();
	}
}

