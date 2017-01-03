#include "stm32l1xx.h"

void DisplayInit(void);
void DisplayCursor(uint8_t Position, uint8_t Show);
void DisplayWaveform(uint8_t Waveform);
void DisplayFrequency(uint16_t Frequency);
void DisplayUnit(uint8_t Unit);
void SendCMD(uint8_t c);
void PrintChar(uint8_t c);
void PrintString(uint8_t *s);
void ClearDisplay(void);
void CursorPosition(int col, int row);
int num2text(uint16_t cislo);
void Delay(uint32_t nCount);
