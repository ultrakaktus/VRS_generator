#include "stm32l1xx.h"
#include "display.h"

#define LCD_Port 			GPIOB
#define RS 				GPIO_Pin_8
#define E 				GPIO_Pin_10
#define D4 				GPIO_Pin_4
#define D5 				GPIO_Pin_5
#define D6 				GPIO_Pin_3
#define D7 				GPIO_Pin_9

static volatile uint32_t Cursor_Last = 0;

void DisplayInit(void) 		//inicializacia displeja
{
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);			//GPIOB clock enable

	GPIO_InitTypeDef GPIO_InitStructure;						//GPIOB inicializacia
	GPIO_InitStructure.GPIO_Pin   = RS | E | D4 | D5 | D6 | D7;
	GPIO_ResetBits(LCD_Port, RS | E | D4 | D5 | D6 | D7);
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(LCD_Port, &GPIO_InitStructure);

	GPIO_ResetBits(LCD_Port, RS | E | D4 | D5 | D6 | D7);
	Delay(0xffff);
	SendCMD(0x02);
	Delay(0x3FFFC);  //pockaj 20ms
	SendCMD(0x28);  //konfiguracia LCD
	SendCMD(0x06);
	SendCMD(0x01);
	SendCMD(0x0F);
	Delay(0xffff);

	DisplayWaveform(0);
	DisplayFrequency(10);
	DisplayUnit(1);
	DisplayCursor(0, 0);
}

void DisplayCursor(uint8_t Position, uint8_t Show)	//nastavenie kurzora na poziciu
{
	if (Position == 0) SendCMD(0x80);
	if (Position == 1) SendCMD(0xC0);
	if (Position == 2) SendCMD(0xC1);
	if (Position == 3) SendCMD(0xC2);
	if (Position == 4) SendCMD(0xC4);
	if (Position == 5) SendCMD(0xC6);
	if (Show == 0) SendCMD(0x0C);	//vypnutie kurzora
	if (Show == 1) SendCMD(0x0E);	//zapnutie kurzora

	Cursor_Last = Position;
}

void DisplayWaveform(uint8_t Waveform)		//zobrazenie funkcie
{
	CursorPosition(1,1);
	PrintString("           ");
	CursorPosition(1,1);
	if (Waveform == 0) PrintString("sinus");
	if (Waveform == 1) PrintString("trojuholnik");
	if (Waveform == 2) PrintString("obdlznik");
	CursorPosition(1,1);
}

void DisplayFrequency(uint16_t Frequency)	//zobrazenie frekvencie
{
	uint16_t pom;
	CursorPosition(1,2);
	PrintString("     ");
	if (Frequency > 999) CursorPosition(1,2);
	if (Frequency < 1000) CursorPosition(2,2);
	if (Frequency < 100) CursorPosition(3,2);
	pom=Frequency/10;
	PrintString(num2text(pom));
	PrintChar('.');
	pom=Frequency%10;
	PrintString(num2text(pom));
	DisplayCursor(Cursor_Last,1);
}

void DisplayUnit(uint8_t Unit)			//zobrazenie jednotky
{
	CursorPosition(7,2);
	if (Unit == 0) PrintChar(' ');
	if (Unit == 1) PrintChar('k');
	if (Unit == 2) PrintChar('M');
	PrintString("Hz");
	CursorPosition(7,2);
}

void strobeEN(void) {	//generacia hodinoveho signalu
	Delay(0x3ff);
	GPIO_SetBits(LCD_Port, E);
	Delay(0x3ff);
	GPIO_ResetBits(LCD_Port, E);
}

void upNib(uint8_t c) {	//nastavenie hornych 4 bitov
	if(c & 0x80)
		GPIO_SetBits(LCD_Port, D7);
	else
		GPIO_ResetBits(LCD_Port, D7);
	if(c & 0x40)
		GPIO_SetBits(LCD_Port, D6);
	else
		GPIO_ResetBits(LCD_Port, D6);
	if(c & 0x20)
		GPIO_SetBits(LCD_Port, D5);
	else
		GPIO_ResetBits(LCD_Port, D5);
	if(c & 0x10)
		GPIO_SetBits(LCD_Port, D4);
	else
		GPIO_ResetBits(LCD_Port, D4);
}

void downNib(uint8_t c) {	//nastavenie dolnych 4 bitov
	if(c & 0x8)
		GPIO_SetBits(LCD_Port, D7);
	else
		GPIO_ResetBits(LCD_Port, D7);
	if(c & 0x4)
		GPIO_SetBits(LCD_Port, D6);
	else
		GPIO_ResetBits(LCD_Port, D6);
	if(c & 0x2)
		GPIO_SetBits(LCD_Port, D5);
	else
		GPIO_ResetBits(LCD_Port, D5);
	if(c & 0x1)
		GPIO_SetBits(LCD_Port, D4);
	else
		GPIO_ResetBits(LCD_Port, D4);
}

void SendCMD(uint8_t c) {	//odoslanie prikazu
	GPIO_ResetBits(LCD_Port, RS);
	upNib(c);
	strobeEN();
	downNib(c);
	strobeEN();
}

void PrintChar(uint8_t c) {	//zobrazenie znaku
	if(((c>=0x20)&&(c<=0x7F)) || ((c>=0xA0)&&(c<=0xFF))) {	//check if 'c' is within display boundry
		GPIO_SetBits(LCD_Port, RS);
		upNib(c);
		strobeEN();
		downNib(c);
		strobeEN();
		GPIO_ResetBits(LCD_Port, RS);
	}
}

void PrintString(uint8_t *s) {	//zobrazenie retazca
	uint8_t i=0;
	//while (s[i] == '\0') i++;
	while(s[i] != '\0') {
		PrintChar(s[i]);
		i++;
	}
}

void ClearDisplay(void) {	//vycistenie displeja
	SendCMD(0x01);
}

void CursorPosition(int col, int row){	//nastavenie pozicie kurzora
	if (row == 1) col += 0x7F;
	if (row == 2) col += 0xBF;
	SendCMD(col);
}

int num2text(uint16_t cislo) {		//premena cisla na text

	static uint8_t i, j, k;
	static char text[16], pom;

	for (i=0;i<16;i++) text[i] = '\0';

	if (cislo == 0)	text[0] = '0';

	i = 15;
	while (cislo > 0) {
		text[15-i] = cislo%10 + '0';
		cislo /= 10;
		i--;
	}

	i = 15;
	while (text[i] == '\0') i--;

	k = i;

	for (j=0;j<(i+1)/2;j++) {
		pom = text[j];
		text[j] = text[k];
		text[k] = pom;
		k--;
	}
	return text;
}

void Delay(uint32_t nCount)
{
  for(; nCount != 0; nCount--);
}
