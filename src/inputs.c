/*
 * inputs.c
 *
 *  Created on: 1. 12. 2016
 *      Author: Jozef Kozarik
 */
#include "inputs.h"

volatile int32_t encoder_rotations = 0;
volatile uint8_t encoder_button = 0;
volatile uint8_t output_button = 0;


/*
 * 				ROTARY ENCODER INITIALIZATION
 */
void EncoderInit(void) {
	/*
	 * Enable clocks
	 */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	/*
	 * GPIOA initialization:
	 * GPIO_Pin_6 =		Encoder pin "B"
	 * GPIO_Pin_1 =		Encoder pin "A"
	 * GPIO_Pin_0 =		Encoder button
	 * */
	GPIO_InitTypeDef      GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/*
	 * INTERRUPTS initialization
	 * */
	EXTI_InitTypeDef EXTI_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource1);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);

	EXTI_InitStruct.EXTI_Line = EXTI_Line1;
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_Init(&EXTI_InitStruct);

	EXTI_InitStruct.EXTI_Line = EXTI_Line0;
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_Init(&EXTI_InitStruct);

	NVIC_InitStruct.NVIC_IRQChannel = EXTI1_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = ENCODER_ROTATION_PREEMPTION;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = ENCODER_ROTATION_SUB;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);

	NVIC_InitStruct.NVIC_IRQChannel = EXTI0_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = ENCODER_BUTTON_PREEMPTION;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = ENCODER_BUTTON_SUB;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
}

/*
 * 				ENCODER ROTATION INTERRUPT HANDLER
 * increments or decrements encoder rotation counter when encoder is turned clockwise or counter-clockwise, respectively
 */
void EXTI1_IRQHandler(void) {

	if (EXTI_GetITStatus(EXTI_Line1) != RESET) {
		if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6)){
			encoder_rotations++;
		}
		else {
			encoder_rotations--;
		}
		EXTI_ClearITPendingBit(EXTI_Line1);
	}
}

/*
 * 				ENCODER BUTTON INTERRUPT HANDLER
 * increments button release counter when button is released
 */
void EXTI0_IRQHandler(void) {

	if (EXTI_GetITStatus(EXTI_Line0) != RESET) {
			encoder_button++;
		EXTI_ClearITPendingBit(EXTI_Line0);
	}
}

/*
 * 				ENCODER POSITION
 * returns encoder rotation relative to its rotation when this function was last called
 */
int32_t EncoderPosition(void) {

	int32_t rotations = encoder_rotations;
	encoder_rotations = 0;
	return rotations;
}

/*
 *				ENCODER BUTTON RELEASE
 * returns number of encoder button releases since this function was last called
 */
uint8_t EncoderRelease(void) {

	uint8_t releases = encoder_button;
	encoder_button = 0;
	return releases;
}


