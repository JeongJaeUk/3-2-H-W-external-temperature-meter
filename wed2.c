/*
flash load "C:\Users\hw_2\Desktop\wed2\wed2\flashclear.axf"
flash load "C:\Users\hw_2\Desktop\wed2\wed2\Debug\wed2.axf"
 */
#include <stm32f10x.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_usart.h>
#include <stm32f10x_exti.h>
#include "stm32f10x_adc.h"
#include "stm32f10x_dma.h"
#include <stm32f10x_tim.h>
#include "misc.h"
#include "lcd.h"
#include "delay.h"
#include "DHT11.h"

u8 temperature;
u8 humidity;
int checkauto = 0;
char onetemp, tentemp;
char temp[2];
char hum[2];
char onehum, tenhum;
char buffer;
char bbb[1];
int a;
uint32_t ARRAY[4];
GPIO_InitTypeDef GPIO_InitStructure;
GPIO_InitTypeDef GPIO_USART1_rx_InitStructure;
GPIO_InitTypeDef GPIO_USART1_tx_InitStructure;
GPIO_InitTypeDef GPIO_USART2_rx_InitStructure;
GPIO_InitTypeDef GPIO_USART2_tx_InitStructure;
USART_InitTypeDef USART1_InitStructure;
USART_InitTypeDef USART2_InitStructure;
DMA_InitTypeDef DMA_InitStructure;
ADC_InitTypeDef ADC_InitStructure;
NVIC_InitTypeDef NVIC_InitStructure;
EXTI_InitTypeDef EXTI_InitStructure;

void delay(unsigned int nCount) {
	unsigned int i;
	for (i = 0; i < nCount; i++) { }
}

void waitSend() {
	while(!(USART2->SR & 0X00000040));
}

void RCC_Configu() {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
}

void GPIO_Configu() {

	// 거리 센서
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	// LED 출력
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	// 온습도 센서

	// 자외선 센서
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11; // 자외선 센서
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	// 릴레이 모듈
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9; // 릴레이 모듈
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	// usart1
	GPIO_USART1_tx_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_USART1_tx_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_USART1_tx_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_USART1_tx_InitStructure);
	GPIO_USART1_rx_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_USART1_rx_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_USART1_rx_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_USART1_rx_InitStructure);
	// usart2
	GPIO_USART2_tx_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_USART2_tx_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_USART2_tx_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_USART2_tx_InitStructure);
	GPIO_USART2_rx_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_USART2_rx_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_USART2_rx_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_USART2_rx_InitStructure);
}

void ADC_Configu() {
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_NbrOfChannel = 4;
	ADC_Init(ADC1, &ADC_InitStructure);

	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 2, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 3, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_7, 4, ADC_SampleTime_55Cycles5);
	ADC_DMACmd(ADC1, ENABLE);
	ADC_Cmd(ADC1, ENABLE);

	ADC_ResetCalibration(ADC1);
	while (ADC_GetResetCalibrationStatus(ADC1))
		;
	ADC_StartCalibration(ADC1);
	while (ADC_GetCalibrationStatus(ADC1))
		;
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

void DMA_Configu() {
	DMA_DeInit(DMA1_Channel1);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) &ADC1->DR;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t) ARRAY;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = 4;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word; // 32bit
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word; // 32bit
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);
	DMA_Cmd(DMA1_Channel1, ENABLE);
}

void USART1_IRQHandler(void){
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
		buffer = USART_ReceiveData(USART1) & 0xFF;
		USART_SendData(USART2, buffer);
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	}
}

void USART2_IRQHandler(void){
	buffer=' ';
	if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) {
		buffer = USART_ReceiveData(USART2) & 0xFF;
		if (buffer == 'w') {
			GPIO_SetBits(GPIOD, GPIO_Pin_2);
			GPIO_SetBits(GPIOD, GPIO_Pin_3);
			GPIO_SetBits(GPIOB, GPIO_Pin_8);
			GPIO_SetBits(GPIOB, GPIO_Pin_9);
		} else if (buffer == 'a') {
			GPIO_ResetBits(GPIOD, GPIO_Pin_2);
			GPIO_SetBits(GPIOD, GPIO_Pin_3);
			GPIO_ResetBits(GPIOB, GPIO_Pin_8);
			GPIO_SetBits(GPIOB, GPIO_Pin_9);
			// 왼쪽으로 가는거
		} else if (buffer == 'd') {
			GPIO_SetBits(GPIOD, GPIO_Pin_2);
			GPIO_ResetBits(GPIOD, GPIO_Pin_3);
			GPIO_SetBits(GPIOB, GPIO_Pin_8);
			GPIO_ResetBits(GPIOB, GPIO_Pin_9);
		} else if (buffer == 's') {
			GPIO_ResetBits(GPIOD, GPIO_Pin_2);
			GPIO_ResetBits(GPIOD, GPIO_Pin_3);
			GPIO_ResetBits(GPIOB, GPIO_Pin_8);
			GPIO_ResetBits(GPIOB, GPIO_Pin_9);
			// 전부 멈춤
		} else if (buffer == 'i') {
			USART_SendData(USART2, 'T');
			waitSend();
			delay(10000);
			USART_SendData(USART2, 'e');
			waitSend();
			delay(10000);
			USART_SendData(USART2, 'm');
			waitSend();
			delay(10000);
			USART_SendData(USART2, 'p');
			waitSend();
			delay(10000);
			USART_SendData(USART2, 'e');
			waitSend();
			delay(10000);
			USART_SendData(USART2, 'r');
			waitSend();
			delay(10000);
			USART_SendData(USART2, 'a');
			waitSend();
			delay(10000);
			USART_SendData(USART2, 't');
			waitSend();
			delay(10000);
			USART_SendData(USART2, 'u');
			waitSend();
			delay(10000);
			USART_SendData(USART2, 'r');
			waitSend();
			delay(10000);
			USART_SendData(USART2, 'e');
			waitSend();
			delay(10000);
			USART_SendData(USART2, ' ');
			waitSend();
			delay(10000);
			USART_SendData(USART2, ':');
			waitSend();
			delay(10000);
			USART_SendData(USART2, ' ');
			waitSend();
			delay(10000);
			USART_SendData(USART2, temp[0]);
			waitSend();
			delay(10000);
			USART_SendData(USART2, temp[1]);
			waitSend();
			delay(10000);
			USART_SendData(USART2, 'C');
			waitSend();
			delay(10000);
			USART_SendData(USART2, ' ');
			waitSend();
			delay(10000);
			USART_SendData(USART2, 'H');
			waitSend();
			delay(10000);
			USART_SendData(USART2, 'u');
			waitSend();
			delay(10000);
			USART_SendData(USART2, 'm');
			waitSend();
			delay(10000);
			USART_SendData(USART2, 'i');
			waitSend();
			delay(10000);
			USART_SendData(USART2, 'd');
			waitSend();
			delay(10000);
			USART_SendData(USART2, 'i');
			waitSend();
			delay(10000);
			USART_SendData(USART2, 't');
			waitSend();
			delay(10000);
			USART_SendData(USART2, 'y');
			waitSend();
			delay(10000);
			USART_SendData(USART2, ' ');
			waitSend();
			delay(10000);
			USART_SendData(USART2, ':');
			waitSend();
			delay(10000);
			USART_SendData(USART2, ' ');
			waitSend();
			delay(10000);
			USART_SendData(USART2, hum[0]);
			waitSend();
			delay(10000);
			USART_SendData(USART2, hum[1]);
			waitSend();
			delay(10000);
			USART_SendData(USART2, '%');
			waitSend();
			delay(10000);
		} else if (buffer == 'j') {
			checkauto = 1;
		} else if (buffer == 'u') {
			checkauto = 0;
			GPIO_ResetBits(GPIOD, GPIO_Pin_2);
			GPIO_ResetBits(GPIOD, GPIO_Pin_3);
			GPIO_ResetBits(GPIOB, GPIO_Pin_8);
			GPIO_ResetBits(GPIOB, GPIO_Pin_9);
		}
		USART_SendData(USART1, buffer);
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);
	}
}

void USART_Configu() {
	USART2_InitStructure.USART_BaudRate = 9600;
	USART2_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART2_InitStructure.USART_StopBits = USART_StopBits_1;
	USART2_InitStructure.USART_Parity = USART_Parity_No;
	USART2_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART2_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART2, &USART2_InitStructure);
	USART_Cmd(USART2, ENABLE);
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

	USART1_InitStructure.USART_BaudRate = 9600;
	USART1_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART1_InitStructure.USART_StopBits = USART_StopBits_1;
	USART1_InitStructure.USART_Parity = USART_Parity_No;
	USART1_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART1_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART1_InitStructure);
	USART_Cmd(USART1, ENABLE);
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

//	USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
//	USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
}

int main(void) {
	SystemInit();
	LCD_Init();
	LCD_Clear(WHITE);
	delay_init();
	while(DHT11_Init());
	RCC_Configu();
	GPIO_Configu();
	ADC_Configu();
	DMA_Configu();
	USART_Configu();
	while (1) {
		DHT11_Read_Data(&temperature, &humidity);
		sprintf(temp, "%u", (unsigned)temperature);
		sprintf(hum, "%u", (unsigned)humidity);
		LCD_ShowNum(100, 100, temperature, 10, BLACK, WHITE);
		LCD_ShowNum(100, 150, humidity, 10, BLACK, WHITE);
		LCD_ShowNum(60, 100, ARRAY[0], 10, BLACK, WHITE);
		LCD_ShowNum(60, 130, ARRAY[1], 10, BLACK, WHITE);
		LCD_ShowNum(60, 160, ARRAY[2], 10, BLACK, WHITE);
		LCD_ShowNum(60, 190, ARRAY[3], 10, BLACK, WHITE);
		LCD_ShowNum(120, 70, checkauto, 10, BLACK, WHITE);
		if(ARRAY[3] < 500) {
			GPIO_SetBits(GPIOD, GPIO_Pin_7);
		}
		else {
			GPIO_ResetBits(GPIOD, GPIO_Pin_7);
		}
		if(checkauto == 1){
			GPIO_SetBits(GPIOD, GPIO_Pin_2);
			GPIO_SetBits(GPIOD, GPIO_Pin_3);
			GPIO_SetBits(GPIOB, GPIO_Pin_8);
			GPIO_SetBits(GPIOB, GPIO_Pin_9);
			if(ARRAY[1] < 1200) {
				if(ARRAY[0]>ARRAY[2]){
					GPIO_ResetBits(GPIOD, GPIO_Pin_2);
					GPIO_SetBits(GPIOD, GPIO_Pin_3);
					GPIO_ResetBits(GPIOB, GPIO_Pin_8);
					GPIO_SetBits(GPIOB, GPIO_Pin_9);
					delay(5000000);
				}
				else{
					GPIO_SetBits(GPIOD, GPIO_Pin_2);
					GPIO_ResetBits(GPIOD, GPIO_Pin_3);
					GPIO_SetBits(GPIOB, GPIO_Pin_8);
					GPIO_ResetBits(GPIOB, GPIO_Pin_9);
					delay(5000000);
				}
			}
		}
		else {}
	}
}
