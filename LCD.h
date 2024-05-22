/*
 * LCD.h
 *
 *  Created on: 21 maj 2024
 *      Author: asmnop
 */

#ifndef INC_LCD_H_
#define INC_LCD_H_

#include <stdio.h>
#include "i2c.h"
#include "IIC.h"

//	DEVICE ADDRESS PCF8574 - CONVERTER I2C:
#define PCF8574_ADDR	0b00100111	//	0x27


//	DEFINIOWANIE OPCJI DO UŻYCIA W WYWOŁANIACH FUNKCJI:
#define LCD_CD			1	//	Czyszczenie wyświetlacza, ustawienie kursora na pozycję początkową (adres=0)

#define LCD_RH			2	//	Kursor na pozycj� pocz�tkow� (adres=0), ustawienie napisu na pozycj� oryginaln�, DD RAM bez zmian,

#define LCD_MODE		4	//	Wybór instrukcji "Entry mode set", domyślnie inkrementacja adresu i okno wyświetlacza stoi w miejscu,
#define LCD_INC			2	//	Inkrementacja wskaźnika adresu DD RAM po zapisie znaku do tej pamięci, kursor przesuwa się w prawo,
#define LCD_DEC			0	//	Dekrementacja wskaźnika adresu DD RAM po zapisie znaku do tej pamięci, kursor przesuwa się w lewo,
#define LCD_DISP_SHIFT	1	//	Przesuwanie okna wyświetlacza wraz ze zmianą wskaźnika adresu, kierunek zależny od kierunku zmian adresu,
#define LCD_DISP_STOP	0	//	Okno wyświetlacza zawsze stoi w miejscu,

#define	LCD_DISPLAY			8	//	Wybór instrukcji "Display on/off control",
#define LCD_DISPLAY_ON		4	//	Włączenie wyświetlacza,
#define LCD_DISPLAY_OFF		0	//	Wyłączenie wyświetlacza,
#define LCD_CURSOR_ON		2	//	Włączenie kursora,
#define LCD_CURSOR_OFF		0	//	Wyłączenie kursora,
#define	LCD_BLINK_ON		1	//	Kursor miga,
#define	LCD_BLINK_OFF		0	//	Kursor nie miga,

#define	LCD_SHIFT			16	//	Wybór instrukcji "Cursor or display shift",
#define LCD_DISPLAY_SHIFT	8	//	Przesuwanie okna wyświetlacza,
#define LCD_CURSOR_MOVE		0	//	Przesuwanie kursora,
#define LCD_RIGHT			4	//	Przesuwanie w prawo,
#define LCD_LEFT			0	//	Przesuwanie w lewo,

#define LCD_FUNCTION_SET	32	//	Wybór instrukcji "Function set",
#define	LCD_8_BIT_MODE		16	//	Tryb 8-bitowy,
#define LCD_4_BIT_MODE		0	//	Tryb 4-bitowy,
#define	LCD_2_LINES			8	//	Dwie linie wyświetlacza,
#define LCD_1_LINE			0	//	Jedna linia wyświetlacza,
#define	LCD_5x10_DOTS		4	//	Pole wyświetlacza 5x10 kropek,
#define	LCD_5x7_DOTS		0	//	Pole wyświetlacza 5x7 kropek,

#define LCD_SET_CGRAM		64	//	Wybór instrukcji "Set CGRAM address",

#define LCD_SET_DDRAM		128	//	Wybór instrukcji "Set DDRAM address",


void LCD_init(void);
void LCD_demo(void);
void LCD_write_data(char data);
void LCD_text(const char *string);

void LCD_clear_display(void);
void LCD_off(void);
void LCD_on(void);
void LCD_cursor(void);
void LCD_blink(void);
void LCD_cursor_blink(void);
void LCD_set_CGRAM(const uint8_t address);
void LCD_set_DDRAM(const uint8_t line, const uint8_t position);
void LCD_line_1(void);
void LCD_line_2(void);
void LCD_line(const uint8_t line);


#endif /* INC_LCD_H_ */









