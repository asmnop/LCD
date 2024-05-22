/*
 * LCD.c
 *
 *  Created on: 21 maj 2024
 *      Author: asmnop
 */


#include "LCD.h"
#include "main.h"
#include "IIC.h"

#define	LCD_DELAY_MS	5	//	Czas oczekiwania na wykonanie instrukcji,

void LCD_init(void);
static inline void LCD_cmd(const uint8_t command);
static inline void LCD_I2C_write(const char data, const uint8_t mode_RS);
void LCD_off(void);
void LCD_on(void);
void LCD_clear_display(void);
void LCD_write_data(char data);
void LCD_text(const char *string);
void LCD_blink(void);


//	##############################################################################################################################
void LCD_init(void)
{
	//	"If the power supply conditions for correctly operating the internal reset circuit are not met, initialization by
	//	instructions becomes necessary."
	//	-inicjalizacja wyświetlacza zawsze ma miejsce po włączeniu zasilania oraz kiedy tylko będzie to konieczne,
	//	-po włączeniu zasilania sterownik HD44780 inicjalizuje się samoczynnie w tryb 8-bitowy, jedna linia, matryca 5x8,
	//	ekran i kursor wygaszone. Tyle teoria. W praktyce zawsze powinno się sterownik zainicjalizować programowo,
	//	-podczas resetu uC zasilanie nie jest odcinane więc i tak trzeba wykonać inicjalizację programowo,
	//	-domyślny stan na wszystkich liniach sterujących oraz transmitujących dane to wyjścia w stanie niskim,

	printf("INICJALIZACJA LCD\n");
	HAL_Delay(50);		//	Poczekaj dłużej niż 40 [ms] aż zasilanie Vcc wzrośnie do 2,7 [V],

	//	$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
	//	-w rzeczywistości jest to wysłanie polecenia "Function set" w postaci D=1, N=0, F=0,
	//	-interfejs 8-bitowy, wyświetlacz jednowierszowy, rozmiar znaku 5x7 punktów,
	//	-flaga zajętości BF nie może być sprawdzana przed tą instrukcją,

	uint8_t data = 0b00111100;
	HAL_I2C_Master_Transmit(&hi2c1, (PCF8574_ADDR<<1), &data, 1, 1000);
	HAL_Delay(5);	//	Poczekaj dłużej niż 4,1 [ms],

	data = 0b00111000;
	HAL_I2C_Master_Transmit(&hi2c1, (PCF8574_ADDR<<1), &data, 1, 1000);
	HAL_Delay(5);	//	Poczekaj dłużej niż 4,1 [ms],

	//	$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
	//	-w rzeczywistości jest to wysłanie polecenia "Function set" w postaci D=1, N=0, F=0,
	//	-interfejs 8-bitowy, wyświetlacz jednowierszowy, rozmiar znaku 5x7 punktów,
	//	-flaga zajętości BF nie może być sprawdzana przed tą instrukcją,
	//	-nie ustawiamy na nowo stanów na liniach bo przesyłamy takie same,
	//	-należy zastosować opóźnienie w czasie trwania stanu wysokiego na linii E, PW_EH = 230 [ns],

	data = 0b00111100;
	HAL_I2C_Master_Transmit(&hi2c1, (PCF8574_ADDR<<1), &data, 1, 1000);
	HAL_Delay(5);	//	Poczekaj dłużej niż 4,1 [ms],

	data = 0b00111000;
	HAL_I2C_Master_Transmit(&hi2c1, (PCF8574_ADDR<<1), &data, 1, 1000);
	HAL_Delay(1);	//	Poczekaj dłużej niż 100 [us],

	//	$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
	//	-flaga zajętości BF może być sprawdzana po wysłaniu poniższych instrukcji,
	//	-jeśli flaga zajętości BF nie jest sprawdzana to czas oczekiwania pomiędzy kolejnymi instrukcjami jest dłuższy niż
	//	faktyczny czas wykonania instrukcji,
	//	-ustawienie funkcji wyświetlacza "Function set" na tryb pracy 4-bitowy,
	//	-podczas wysyłania tej instrukcji jesteśmy jeszcze w trybie 8-bitowym,
	//	-jesteśmy w trybie 8-bitowym ale wysyłamy tylko starszą część bajtu danych,

	data = 0b00101100;
	HAL_I2C_Master_Transmit(&hi2c1, (PCF8574_ADDR<<1), &data, 1, 1000);
	HAL_Delay(5);	//	Poczekaj dłużej niż 4,1 [ms],

	data = 0b00101000;
	HAL_I2C_Master_Transmit(&hi2c1, (PCF8574_ADDR<<1), &data, 1, 1000);
	HAL_Delay(1);	//	Poczekaj dłużej niż 100 [us],

	//	$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
	//	-jesteśmy w trybie 4-bitowym,
	//	-ustawienie parametrów takich jak ilość	wierszy wyświetlacza oraz rozmiaru znaku,
	//	-ustawiamy dwa wiersze (N=1) oraz rozmiar znaku na 5x7 punktów (F=0),

	LCD_cmd( LCD_FUNCTION_SET | LCD_4_BIT_MODE | LCD_2_LINES | LCD_5x7_DOTS );
	LCD_off();
	LCD_clear_display();
	LCD_cmd(LCD_MODE | LCD_INC | LCD_DISP_STOP );

	LCD_on();
	LCD_blink();
	LCD_text("MATRIX HAS YOU !!!");
}

static inline void LCD_cmd(const uint8_t command)
{
	LCD_I2C_write(command, 0);
}

static inline void LCD_I2C_write(const char data, const uint8_t mode_RS)
{
	uint8_t data_temp = (data & 0b11110000) | 8 | 4 | mode_RS;
	HAL_I2C_Master_Transmit(&hi2c1, (PCF8574_ADDR<<1), &data_temp, 1, 1000);

	data_temp = (data & 0b11110000) | 8 | mode_RS;
	HAL_I2C_Master_Transmit(&hi2c1, (PCF8574_ADDR<<1), &data_temp, 1, 1000);

	data_temp = ((data & 0b00001111)<<4) | 8 | 4 | mode_RS;
	HAL_I2C_Master_Transmit(&hi2c1, (PCF8574_ADDR<<1), &data_temp, 1, 1000);

	data_temp = ((data & 0b00001111)<<4) | 8 | mode_RS;
	HAL_I2C_Master_Transmit(&hi2c1, (PCF8574_ADDR<<1), &data_temp, 1, 1000);
}

void LCD_off(void)
{
	//	-wyłączenie wyświetlacza, pozycja kursora w pamięci DDRAM pozostaje bez zmian,

	LCD_cmd( LCD_DISPLAY | LCD_DISPLAY_OFF | LCD_CURSOR_OFF | LCD_BLINK_OFF);
	HAL_Delay(LCD_DELAY_MS);
}

void LCD_on(void)
{
	//	-włączenie wyświetlacza bez bajerów,
	//	-odpalenie polecenia "Display on/off control",

	LCD_cmd( LCD_DISPLAY | LCD_DISPLAY_ON | LCD_CURSOR_OFF | LCD_BLINK_OFF );
	HAL_Delay(LCD_DELAY_MS);
}

void LCD_clear_display(void)
{
	//	-wypełnienie pamięci DDRAM znakami spacji i przesunięcie kursora oraz okna do pozycji początkowej na DDRAM = 0,
	//	-innymi słowy wyczyszczenie wyświetlacza,

	LCD_cmd(LCD_CD);
	HAL_Delay(LCD_DELAY_MS);	//	Czas wykonywania polecenia: 1,53 [ms],
}

void LCD_write_data(char data)
{
	//	-funkcja wysyłająca jeden bajt danych do pamięci DDRAM,

	LCD_I2C_write(data, 1);
}

void LCD_text(const char *string)
{
	//	-funkcja służąca do wyświetlania łańcucha znaków zdefiniowanego w tablicy,
	//	-poprzednia nazwa funkcji: LCD_write_str,
	//	-argumentem funkcji jest nazwa tablicy czyli adres pierwszego elementu tablicy,
	//	OPIS DZIAŁANIA FUNKCJI:
	//	-na początku następuje pobranie zawartości rejestru z pod podanego adresu i sprawdzenie czy to nie zero,
	//	zero oznacza koniec tablicy czyli tablica jest już pusta. Następnie odpalana jest funkcja do zapisu znaku
	//	do pamięci DD RAM. Znakiem, który zapiszemy będzie wartość znajdująca pod pierwszym adresem. Do funkcji
	//	wchodzi wartość przed zwiększeniem. Następnie operator '++' zwiększa adres, aby po ponownym wejściu do
	//	warunku pętli została sprawdzona kolejna wartość elementu w tablicy.

	while(*string)					//	Pobranie zawartości rejestru z pod podanego adresu i sprawdzenie czy to nie zero,
		LCD_write_data(*string++);	//	Zapisanie znaku do pamięci DD RAM czyli wyświetlenie go,
}

void LCD_blink(void)
{
	//	-włączenie wyświetlacza wraz z mrygającym polem,
	//	-odpalenie polecenia "Display on/off control",

	LCD_cmd( LCD_DISPLAY | LCD_DISPLAY_ON | LCD_CURSOR_OFF | LCD_BLINK_ON );
	HAL_Delay(LCD_DELAY_MS);
}










