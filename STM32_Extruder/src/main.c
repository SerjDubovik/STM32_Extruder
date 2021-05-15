#include "stm32f10x.h"
#include "modbus.h"
#include "GPIO.h"
#include "pwm.h"
#include "adc.h"
#include "I2C.h"


/* LM75 defines */
	#define  LM75_ADDR1  0x90
	#define  LM75_ADDR2  0x92
	#define  LM75_ADDR3  0x94

/* LM75 registers */
#define LM75_REG_TEMP                 0x00 // Temperature
#define LM75_REG_CONF                 0x01 // Configuration
#define LM75_REG_THYS                 0x02 // Hysteresis
#define LM75_REG_TOS                  0x03 // Overtemperature shutdown


unsigned short reg_i2c		= LM75_REG_TEMP;
unsigned short addr_i2c		= LM75_ADDR1;

// i2c interrapt
char data_write[20];
char data_read[20]={0,};




unsigned short	array_mb[100]						= {};			// массив для хранения принятых/переданных слов
			

int main(void)
{

	init_GPIO();
    init_modbus();
    init_adc();
//    Init_I2C1();
    init_timer3();

    relay_2(off);
    relay_1(off);

    red_led(off);
    blue_led(off);


	for(;;)
	{





	    if(array_mb[2] == 0)										// подтверждение приёма команды. готовность принимать следующую
	    {
	    	array_mb[3] = 0;
	    }

		array_mb[8]++;

		Modbus_slave();

	} // скобка бесконечного цикла
} // скобка мейна
