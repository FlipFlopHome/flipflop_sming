/*
 * main_task.cpp
 *
 *  Created on: Feb 7, 2016
 *      Author: juliendarrah
 */

#include <user_config.h>
#include <SmingCore/SmingCore.h>
#include "app_task.h"
#include "AppSettings.h"
#include "module_config.h"

Timer AppTaskTimer;

static volatile uint16_t buttonState;
static volatile uint16_t buttonStateSame;
static volatile uint16_t buttonStatePrevious;
static volatile uint8_t  buttonStateLatchValue;
static volatile uint16_t buttonRefreshStateCntr;

static volatile uint16_t TempRefreshStateCntr;
static volatile uint16_t TempSensorRefreshCntr;



void AppTask_Init(void)
{
	/* Init GPIO */
	Gpio_Init();
	/* Init Temperature Sensor */
	TempSensor.Init(GPIO_TEMP_SENSOR);
	TempSensor.StartMeasure(); // first measure start,result after 1.2 seconds * number of sensors


	/* Init Variables */
	buttonState = 0;
	buttonRefreshStateCntr = 0;
	TempRefreshStateCntr = 0;
	TempSensorRefreshCntr = 0;
	/* Init application timer task */
	AppTaskTimer.initializeMs(APP_TASK_RATE, AppTask_Task).start();

}

void AppTask_Task(void)
{



	//Invert State, Save button State
	buttonState = buttonState + (~digitalRead(GPIO_PUSH_BUTTON) & 0x01);


    if(buttonRefreshStateCntr >= BUTTON_REFRESH_STATE_RATE)
    {
    	/* Active High Current State */
    	if(buttonState >= BUTTON_REFRESH_STATE_RATE)
    	{
    		/* Falling Edge Detection on the button */
    		if(buttonState != buttonStatePrevious)
    		{
    			//Toggle Output
    			buttonStateLatchValue = ~buttonStateLatchValue & 0x01;
    			digitalWrite(GPIO_RELAY_CTRL, buttonStateLatchValue);
    			Serial.print("Toggle\r\n");
    		}
    		else
    		{
    			buttonStateSame++;
    		}
    		//buttonStatePrevious = 0;
    		if(buttonStateSame >= 50)
    		{
    			//Go into Configuration in AP mode
    			//system_os_post(user_procTaskPrio, 1, 0);
    			digitalWrite(GPIO_BUTTON_LED, HIGH);
    			ModuleConfig_init();
    			buttonStateSame = 0;
    		}
    	}
    	else
    	{
    		//buttonStatePrevious = 1;
    	}

    	buttonStatePrevious = buttonState;
    	buttonState = 0;
    	buttonRefreshStateCntr = 0;

    }
    else
    {
    	//uart0_sendStr("GPIO12: LOW\r\n");
    	//GPIO_OUTPUT_SET(4, 0);
    	buttonRefreshStateCntr++;
    }

    if(TempSensorRefreshCntr >= TEMP_SENSOR_REFRESH_RATE)
    {
    	TempSensorRefreshCntr = 0;
    	TempSensor.StartMeasure();

    }
    else
    {
    	TempSensorRefreshCntr++;
    }
    if(TempRefreshStateCntr >= 500)
    {
#if(0)
    	uint16_t adcData;
    	char adcResultString[10];

    	TempRefreshStateCntr = 0;
    	os_bzero(adcResultString, sizeof(adcResultString));

    	adcData = system_adc_read();

    	os_sprintf(adcResultString, "ADC Data: %d \r\n", adcData);

    	uart0_sendStr(adcResultString);

    	char tempResultString[40];
    	uint16_t tempResult;
    	uint16_t Tinteger;
    	uint16_t Tfraction;


    	TempRefreshStateCntr = 0;
    	tempResult = (MAX_DS18B20_GetTemp(14));
    	Tinteger = tempResult >> 4;
    	Tfraction = (uint16_t)((float)(tempResult & 0x000F) * 0.0625);
    	os_sprintf(tempResultString, "Temp Data: %d.%i Celcius\r\n", Tinteger, Tfraction);

    	uart0_sendStr(tempResultString);
#endif

    }
    else
    {
    	TempRefreshStateCntr++;
    }


}

void Gpio_Init(void)
{
	/* Config Input GPIO channels */
	pinMode(GPIO_PUSH_BUTTON, INPUT);

	/* Config Output GPIO channels */
	pinMode(GPIO_BUTTON_LED, OUTPUT);
	pinMode(GPIO_RELAY_CTRL, OUTPUT);

	/* Default state */
	digitalWrite(GPIO_BUTTON_LED, LOW);
	digitalWrite(GPIO_RELAY_CTRL, LOW);


}

