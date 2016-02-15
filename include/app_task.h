/*
 * main_task.h
 *
 *  Created on: Feb 7, 2016
 *      Author: juliendarrah
 */

#ifndef INCLUDE_APP_TASK_H_
#define INCLUDE_APP_TASK_H_

#include <Libraries/DS18S20/ds18s20.h>

#define APP_TASK_RATE			(10) /* 10ms */

#define BUTTON_REFRESH_STATE_RATE				(10)
#define BUTTON_MIN_REFRESH_STATE_RATE			(10) /* 10 * 10ms = 100ms */
#define BUTTON_MAX_REFRESH_STATE_RATE			(200)
#define BUTTON_SET_CONFIG_STATE_RATE			(500)

#define TEMP_SENSOR_REFRESH_RATE				(500)

/* Digital Input */
#define GPIO_PUSH_BUTTON		(4)
/* Digital Output */
#define GPIO_BUTTON_LED			(5)
#define GPIO_RELAY_CTRL			(12)

/* Temperature Sensor GPIO channel */
#define GPIO_TEMP_SENSOR		(14)


extern DS18S20 TempSensor;
extern HttpServer server;

void AppTask_Init(void);
void AppTask_Task(void);
void Gpio_Init(void);

#endif /* INCLUDE_APP_TASK_H_ */
