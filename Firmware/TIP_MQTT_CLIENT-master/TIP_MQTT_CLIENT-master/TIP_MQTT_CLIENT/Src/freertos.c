/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : freertos.c
 * Description        : Code for freertos applications
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under Ultimate Liberty license
 * SLA0044, the "License"; You may not use this file except in compliance with
 * the License. You may obtain a copy of the License at:
 *                             www.st.com/SLA0044
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */     
#include <string.h>
#include <stdlib.h>
#include "lwip.h"
#include "lwip/api.h"
#include "MQTTClient.h"
#include "MQTTInterface.h"
#include "tc74_sensor.h"
#include "hih_5031_sensor.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
static char str_T[5];
static char str_H[5];
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define BROKER_IP		"91.121.93.94" //ip mosqiutto broker
#define MQTT_PORT		1883
#define MQTT_BUFSIZE	1024
#define MQTT_TOPIC_HUM "AGGO/SS01/Humidity01"
#define MQTT_TOPIC_TEMP "AGGO/SS01/Temperature01"
char message[64];
uint8_t temperature, humidity;
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
extern struct netif gnetif; //extern gnetif

osThreadId mqttClientSubTaskHandle;  //mqtt client task handle
osThreadId mqttClientPubTaskHandle;  //mqtt client task handle

Network net; //mqtt network
MQTTClient mqttClient; //mqtt client

uint8_t sndBuffer[MQTT_BUFSIZE]; //mqtt send buffer
uint8_t rcvBuffer[MQTT_BUFSIZE]; //mqtt receive buffer
uint8_t msgBuffer[MQTT_BUFSIZE]; //mqtt message buffer
extern ADC_HandleTypeDef hadc1;
extern I2C_HandleTypeDef hi2c1;
/* USER CODE END Variables */
osThreadId defaultTaskHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void MqttClientSubTask(void const *argument); //mqtt client subscribe task function
void MqttClientPubTask(void const *argument); //mqtt client publish task function
int  MqttConnectBroker(void); 				//mqtt broker connect function
void MqttMessageArrived(MessageData* msg); //mqtt message callback function
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);

extern void MX_LWIP_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* Hook prototypes */
void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName);

/* USER CODE BEGIN 4 */
void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName)
{
	/* Run time stack overflow checking is performed if
   configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2. This hook function is
   called if a stack overflow is detected. */
	HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, GPIO_PIN_SET); //turn on red led when detects stack overflow

}
/* USER CODE END 4 */

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
	*ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
	*ppxIdleTaskStackBuffer = &xIdleStack[0];
	*pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
	/* place for user code */
}                   
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
	/* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
	/* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
	/* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
	/* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 512);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
	/* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
 * @brief  Function implementing the defaultTask thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* init code for LWIP */
  MX_LWIP_Init();
  /* USER CODE BEGIN StartDefaultTask */
	osThreadDef(mqttClientSubTask, MqttClientSubTask, osPriorityNormal, 0, configMINIMAL_STACK_SIZE); //subscribe task
	osThreadDef(mqttClientPubTask, MqttClientPubTask, osPriorityNormal, 0, configMINIMAL_STACK_SIZE); //publish task
	mqttClientSubTaskHandle = osThreadCreate(osThread(mqttClientSubTask), NULL);
	mqttClientPubTaskHandle = osThreadCreate(osThread(mqttClientPubTask), NULL);

	/* Infinite loop */
	for(;;)
	{
		HAL_GPIO_TogglePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin); //toggle running led
		osDelay(500);
	}
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
/* reverse:  reverse string s in place */

void MqttClientSubTask(void const *argument)
{
	while(1)
	{
		//waiting for valid ip address
		if (gnetif.ip_addr.addr == 0 || gnetif.netmask.addr == 0 || gnetif.gw.addr == 0) //system has no valid ip address
		{
			osDelay(1000);
			continue;
		}
		else
		{
			printf("DHCP/Static IP O.K.\n");
			break;
		}
	}

	while(1)
	{
		if(!mqttClient.isconnected)
		{
			//try to connect to the broker
			MQTTDisconnect(&mqttClient);
			MqttConnectBroker();
			osDelay(1000);
		}
		else
		{
			MQTTYield(&mqttClient, 1000); //handle timer
			osDelay(100);
		}
	}
}

void MqttClientPubTask(void const *argument)
{

	MQTTMessage message_temp;
	MQTTMessage message_hum;

	while(1)
	{
		 /* Get temperature from sensor */
		if (true == tc74_sensor_readTemperature(&hi2c1, &temperature))
		{
			 itoa(temperature,str_T,10);
		}

		/* Get humidity from sensor */
		if (true == hih_5031_sensor_readHumidity(&hadc1, &humidity))
		{
			itoa((int)humidity,str_H,10);
		}

		if(mqttClient.isconnected)
		{
			message_temp.payload =  (void*)str_T;
			message_temp.payloadlen = strlen(str_T);

			MQTTPublish(&mqttClient, MQTT_TOPIC_TEMP, &message_temp); //publish a temperature message

			message_hum.payload = (void*)str_H;
			message_hum.payloadlen = strlen(str_H);
			MQTTPublish(&mqttClient, MQTT_TOPIC_HUM, &message_hum); //publish a message
		}

		osDelay(1000);
	}
}

int MqttConnectBroker()
{
	int ret;

	NewNetwork(&net);
	ret = ConnectNetwork(&net, BROKER_IP, MQTT_PORT);
	if(ret != MQTT_SUCCESS)
	{
		printf("ConnectNetwork failed.\n");
		return -1;
	}

	MQTTClientInit(&mqttClient, &net, 1000, sndBuffer, sizeof(sndBuffer), rcvBuffer, sizeof(rcvBuffer));

	MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
	data.willFlag = 0;
	data.MQTTVersion = 3;
	data.clientID.cstring = "STM32F4";
	data.username.cstring = "STM32F4";
	data.password.cstring = "";
	data.keepAliveInterval = 60;
	data.cleansession = 1;

	ret = MQTTConnect(&mqttClient, &data);
	if(ret != MQTT_SUCCESS)
	{
		printf("MQTTConnect failed.\n");
		return ret;
	}

	ret = MQTTSubscribe(&mqttClient, "test", QOS0, MqttMessageArrived);
	if(ret != MQTT_SUCCESS)
	{
		printf("MQTTSubscribe failed.\n");
		return ret;
	}

	printf("MQTT_ConnectBroker O.K.\n");
	return MQTT_SUCCESS;
}

void MqttMessageArrived(MessageData* msg)
{
	HAL_GPIO_TogglePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin); //toggle pin when new message arrived

	MQTTMessage* message = msg->message;
	memset(msgBuffer, 0, sizeof(msgBuffer));
	memcpy(msgBuffer, message->payload,message->payloadlen);

	printf("MQTT MSG[%d]:%s\n", (int)message->payloadlen, msgBuffer);
}

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
