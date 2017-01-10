/*
 * inputs.h
 *
 *  Created on: 1. 12. 2016
 *      Author: Jozef Kozarik
 */

#ifndef INPUTS_H_
#define INPUTS_H_

#include "stm32l1xx.h"

/*		HARDWARE:
	 * GPIOA:
	 * GPIO_Pin_6 =		Encoder pin "B"
	 * GPIO_Pin_1 =		Encoder pin "A"
	 * GPIO_Pin_0 =		Encoder button
	 * */

/*
 * priority preruseni od ovladacich prvkov
 */

#define ENCODER_ROTATION_PREEMPTION	0x00
#define ENCODER_ROTATION_SUB		0x01

#define ENCODER_BUTTON_PREEMPTION	0x00
#define ENCODER_BUTTON_SUB			0x02


/*
 * Globalne premenne, ktorych hodnoty sa menia v preruseniach
 */
extern volatile int32_t encoder_rotations;
extern volatile uint8_t encoder_button;


void EncoderInit(void);
	//- Zavola sa na zaciatku programu, nastavi sa GPIO a prerusenia pre otacanie a stlacanie enkodera

int32_t EncoderPosition(void);
	//- Vrati pocet otoceni enkodera od posledneho citania, po zavolani funkcie sa resetuje pocitadlo polohy
	//- Kladne cisla su v smere hodin

uint8_t EncoderRelease(void);
	//- Vrati pocet uvolneni tlacidla na enkoderi, po zavolani funkcie sa resetuje pocitadlo uvolneni tlacidla

void EXTI1_IRQHandler(void);
	//- Prerusenie od otocenia enkodera

void EXTI2_IRQHandler(void);
	//- Prerusenie od stlacenia enkodera


#endif /* INPUTS_H_ */
