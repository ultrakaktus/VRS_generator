
/* Includes */
#include <stddef.h>
#include "stm32l1xx.h"
#include "inputs.h"
#include "display.h"
#include "AD9833.h"


/* Private typedef */
/* Private define  */
#define BLICK_TIME 500000 //uint32_t pocet opakovani hlavnej slucky po ktorom zhasne kurzor

/* Private macro */
/* Private variables */
/* Private function prototypes */
/* Private functions */
uint32_t Power(uint16_t x, uint8_t n){
    uint32_t number = 1;
	uint8_t i = 0;

    for (i = 0; i < n; ++i)
        number *= x;

    return(number);
}
void ChangeFrequency(uint32_t *Frequency, int32_t Encoder, uint8_t *Cursor, uint8_t Unit){
	if(Encoder != 0){//ak sa pootocil enkoder, 0 ak sa menila jednotka a chceme vyuzit iba orezanie rozsahu
		(*Frequency) += Encoder*Power(10,4-(*Cursor))*Power(1000,Unit);

		if((*Frequency) > 999999999){ // pretecenie do zapornych cisel
			(*Frequency) -= Encoder*Power(10,4-(*Cursor))*Power(1000,Unit); //vratime krok spat
			if((*Cursor) != 4){ //ak niesme na desatinnom mieste tak posunieme kurzor a odcitame 1
				(*Cursor)++;
				ChangeFrequency(Frequency, Encoder, Cursor, Unit);
			}
		}
	}
	if((*Frequency) == 0)(*Frequency) = Power(1000,Unit); // ak je frekvencia 0 nastavy sa 000.1 xHz

	if(Unit != 2){	// orezanie rozsahu na 999.9 (k)Hz
		if((*Frequency) > 9999*Power(1000,Unit)){
			(*Frequency) = 9999*Power(1000,Unit);
		}
	}
	else{	//orezanie rozsahu na 10 MHz
		if((*Frequency) > 100000000){
			(*Frequency) = 100000000;
		}
	}
	DisplayFrequency((*Frequency) / Power(1000,Unit));
}
uint8_t EncoderButton = 0;
int32_t Encoder = 0;

uint8_t Cursor = 0;		//0 priebeh, 1 stovky, 2 desiatky, 3 jednotky, 4 desatinne miesto, 5 jednotka(rozsah)
uint8_t Unit = 1;		//0 Hz, 1 kHz, 2 MHz
uint8_t State = 0;		//0 posuvanie sa v menu, 1 zmena parametru
uint8_t Waveform = 0;	//0 sinus, 1 trojuholnik, 2 obdlznik
uint32_t Frequency = 10000; //10^-1 HZ
uint32_t Timer = 0;

/**
**===========================================================================
**
**  Abstract: main program
**
**===========================================================================
*/
int main(void)
{


	DisplayInit();
	DDSInit();
	EncoderInit();

  /* Infinite loop */
  while (1)
  {
	  	EncoderButton += EncoderRelease();
		Encoder += EncoderPosition();

		if(EncoderButton){
			DisplayCursor(Cursor, 1); //zobrazi sa kurzor
			Timer = 0;

			if(EncoderButton & 0x01){ //neparny pocet stlaceni ( pri parnom bez zmeny), posuvanie v menu/zmena hodnoty
				State = !State;
				if(State == 0){ //ak sa ukoncilo editovanie parametru zapise sa do DDS
					if(Cursor)DDSFrequency(Frequency); //ak je poloha kurzora nenulova tak sa zapise frekvencia inak priebeh
					else DDSWaveform(Waveform);
				}
			}
			EncoderButton = 0;
		}
		else if(Encoder){
			if(State){
				DisplayCursor(Cursor, 1); //zobrazi sa kurzor
				Timer = 0;
				if(Cursor == 0){ //Waveform
					Encoder = Encoder % 3; // upravenie rozsahu na 0 az 2
					if(Encoder < 0)Encoder += 3;

					Waveform += Encoder; // zmena priebehu a upravenie rozsahu na 0 az 2
					Waveform = Waveform % 3;

					DisplayWaveform(Waveform);
				}
				else if(Cursor == 5){ //Unit
					int8_t Pom = Unit;

					Encoder = Encoder % 3; // upravenie rozsahu na 0 az 2
					if(Encoder < 0)Encoder += 3;

					Unit += Encoder; // zmena jednotky a upravenie rozsahu na 0 az 2
					Unit = Unit % 3;

					DisplayUnit(Unit);

					Pom -= Unit; // zmena frekvencie
					if(Pom < 0)Frequency *= Power(1000,-1*Pom);
					else Frequency /= Power(1000,Pom);
					ChangeFrequency(&Frequency, 0, &Cursor, Unit);
				}
				else{ //Frequency
					ChangeFrequency(&Frequency, Encoder, &Cursor, Unit);
				}
			}
			else{
				Encoder = Encoder % 6; // upravenie rozsahu na 0 az 5
				if(Encoder < 0)Encoder += 6;

				Cursor += Encoder; // zmena priebehu a upravenie rozsahu na 0 az 2
				Cursor = Cursor % 6;

				DisplayCursor(Cursor, 1); //zobrazi sa kurzor
				Timer = 0;
			}
 			Encoder = 0;
		}
		else{
			if(Timer == BLICK_TIME && State == 0){
				DisplayCursor(Cursor, 0); //schova sa kurzor
			}
			Timer++;
		}
	}
  return 0;
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/*
 * Minimal __assert_func used by the assert() macro
 * */
void __assert_func(const char *file, int line, const char *func, const char *failedexpr)
{
  while(1)
  {}
}

/*
 * Minimal __assert() uses __assert__func()
 * */
void __assert(const char *file, int line, const char *failedexpr)
{
   __assert_func (file, line, NULL, failedexpr);
}
