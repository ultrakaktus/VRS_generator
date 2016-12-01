/*
 * inputs.h
 *
 *  Created on: 1. 12. 2016
 *      Author: Jozef Kozarik
 */

#ifndef INPUTS_H_
#define INPUTS_H_

#include "stm32l1xx.h"

/*
 * priority preruseni od ovladacich prvkov
 * LUBO, nastav podla vlastneho uvazenia
 */

#define ENCODER_ROTATION_PREEMPTION	0x00
#define ENCODER_ROTATION_SUB		0x01

#define ENCODER_BUTTON_PREEMPTION	0x00
#define ENCODER_BUTTON_SUB			0x02

#define OUTPUT_BUTTON_PREEMPTION	0x00
#define OUTPUT_BUTTON_SUB			0x00

/*
 * Globalne premenne, ktorych hodnoty sa menia v preruseniach
 */
extern volatile int32_t encoder_rotations;
extern volatile uint8_t encoder_button;
extern volatile uint8_t output_button;

void EncoderInit(void);
	//- Zavola sa na zaciatku programu, nastavi sa GPIO a prerusenia pre otacanie a stlacanie enkodera

int32_t EncoderPosition(void);
	//- Vrati pocet otoceni enkodera od posledneho citania, po zavolani funkcie sa resetuje pocitadlo polohy
	//- Kladne cisla su v smere hodin

uint8_t EncoderRelease(void);
	//- Vrati pocet uvolneni tlacidla na enkoderi, po zavolani funkcie sa resetuje pocitadlo uvolneni tlacidla

void LEDInit(void);
	//- Zavola sa na zaciatku programu, nastavi sa GPIO pre LED signalizujucu zapnuty vystup generatora

void LEDState(uint8_t State);
	//- Nastavuje signalizacnu LED, 0 nesvieti, 1 svieti

void ButtonInit(void);
	//- Zavola sa na zaciatku programu, nastavi sa GPIO a prerusenia pre tlacidlo zapinania/vypinania vystupu

uint8_t ButtonRelease(void);
	//- Vrati pocet uvolneni tlacidla na zapinanie vystupu, po zavolani funkcie sa resetuje pocitadlo uvolneni tlacidla

void EXTI1_IRQHandler(void);
	//- Prerusenie od otocenia enkodera

void EXTI2_IRQHandler(void);
	//- Prerusenie od stlacenia enkodera

void EXTI3_IRQHandler(void);
	//- Prerusenie od tlacidla na zap/vyp. vystupu

#endif /* INPUTS_H_ */
