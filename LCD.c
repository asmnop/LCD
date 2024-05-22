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




#define LCD_BUF_X_MAX		20		//	Ilość pól w wierszu,
#define LCD_BUF_Y_MAX		4		//	Ilość wierszy,
#define LCD_LINE_0_ADDRESS	0x00	//	Adres pierwszego pola w linii nr 0,
#define LCD_LINE_1_ADDRESS	0x40	//	Adres pierwszego pola w linii nr 1,
#define LCD_LINE_2_ADDRESS	0x14	//	Adres pierwszego pola w linii nr 2,
#define LCD_LINE_3_ADDRESS	0x54	//	Adres pierwszego pola w linii nr 3,

void LCD_init(void);
void LCD_demo(LCD_t *ptr);
static inline void LCD_cmd(const uint8_t command);
static inline void LCD_I2C_write(const char data, const uint8_t mode_RS);
void LCD_write_data(char data);
void LCD_text(const char *string);

void LCD_clear_display(void);
void LCD_return_home(void);
void LCD_off(void);
void LCD_on(void);
void LCD_cursor(void);
void LCD_blink(void);
void LCD_cursor_blink(void);
void LCD_set_CGRAM(const uint8_t address);
void LCD_set_DDRAM(const uint8_t line, const uint8_t position);
void LCD_line_1(LCD_t *ptr);
void LCD_line_2(LCD_t *ptr);
void LCD_line(LCD_t *ptr, const uint8_t line);

void LCD_move_cursor(const uint8_t direction);
void LCD_move_display(const uint8_t direction);
void LCD_fill_CGRAM(LCD_t *ptr, const uint8_t address, const char arr[]);
void LCD_show_display_modes(void);
void LCD_show_shift_modes(void);

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

void LCD_demo(LCD_t *ptr)
{
	//	-obsługa testowa wyświetlacza,

	LCD_clear_display();			//	Na wejściu czyścimy wyświetlacz,
	LCD_blink();


	LCD_text("SIEMANKO!!!");
	LCD_line_2(ptr);
	LCD_text("yo, yo");
	LCD_blink();

	LCD_cursor_blink();
	LCD_line(ptr, 2);
	LCD_text("Linia numer 3");
	LCD_line(ptr, 3);
	LCD_text("==KONIEC==");

	LCD_on();

	/*
	HAL_Delay(1000);

	LCD_show_display_modes();
	LCD_show_shift_modes();

	LCD_return_home();
	LCD_text("      THE END");
	*/
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



//	##############################################################################################################################
void LCD_clear_display(void)
{
	//	-wypełnienie pamięci DDRAM znakami spacji i przesunięcie kursora oraz okna do pozycji początkowej na DDRAM = 0,
	//	-innymi słowy wyczyszczenie wyświetlacza,

	LCD_cmd(LCD_CD);
	HAL_Delay(LCD_DELAY_MS);	//	Czas wykonywania polecenia: 1,53 [ms],
}

void LCD_return_home(void)
{
	//	-przesunięcie kursora oraz okna do pozycji początkowej na DDRAM = 0,
	//	-zawartość DDRAM pozostaje bez zmian,

	LCD_cmd(LCD_RH);
	HAL_Delay(LCD_DELAY_MS);
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

void LCD_cursor(void)
{
	//	-włączenie wyświetlacza wraz podkreślnikiem,
	//	-odpalenie polecenia "Display on/off control",

	LCD_cmd( LCD_DISPLAY | LCD_DISPLAY_ON | LCD_CURSOR_ON | LCD_BLINK_OFF );
	HAL_Delay(LCD_DELAY_MS);
}

void LCD_blink(void)
{
	//	-włączenie wyświetlacza wraz z mrygającym polem,
	//	-odpalenie polecenia "Display on/off control",

	LCD_cmd( LCD_DISPLAY | LCD_DISPLAY_ON | LCD_CURSOR_OFF | LCD_BLINK_ON );
	HAL_Delay(LCD_DELAY_MS);
}

void LCD_cursor_blink(void)
{
	//	-włączenie wyświetlacza z podkreślnikiem oraz mrygającym polem,
	//	-odpalenie polecenia "Display on/off control",

	LCD_cmd( LCD_DISPLAY | LCD_DISPLAY_ON | LCD_CURSOR_ON | LCD_BLINK_ON );
	HAL_Delay(LCD_DELAY_MS);
}

void LCD_set_CGRAM(const uint8_t address)
{
	//	-ustawienie adresu CG RAM,
	//	-wpisywane adresy powinny być w zakresie 0x00 - 0x3F (0 - 63),

	LCD_cmd( LCD_SET_CGRAM | address);
	HAL_Delay(LCD_DELAY_MS);
}

void LCD_set_DDRAM(const uint8_t line, const uint8_t position)
{
	//	-ustawienie adresu DD RAM,
	//	-w praktyce chodzi o ustawienie adresu na konkretne pole w wyświetlaczu,
	//	-w trybie 2-liniowym adresy w zakresie:
	//	pierwsza linia	--> 0x00 - 0x27 (0 - 39),
	//	druga linia		--> 0x40 - 0x67 (64 - 103),


	//	WYŚWIETLACZ 4x16:
	//	linia nr 1 --> 0x00 - 0x0F (0 - 15)
	//	linia nr 2 --> 0x40 - 0x4F (64 - 79)
	//	linia nr 3 --> 0x10 - 0x1F (16 - 31)
	//	linia nr 4 --> 0x50 - 0x5F (80 - 95)

	//	WYŚWIETLACZ 4x20:
	//	linia nr 1 --> 0x00 - 0x13 (0 - 19)
	//	linia nr 2 --> 0x40 - 0x53 (64 - 83)
	//	linia nr 3 --> 0x14 - 0x27 (20 - 39)
	//	linia nr 4 --> 0x54 - 0x67 (84 - 103)

	uint8_t pos_ram = 0;

	switch(line)
	{
		case 0: pos_ram = LCD_LINE_0_ADDRESS + position; break;
		case 1: pos_ram = LCD_LINE_1_ADDRESS + position; break;
		case 2: pos_ram = LCD_LINE_2_ADDRESS + position; break;
		case 3: pos_ram = LCD_LINE_3_ADDRESS + position; break;
	}

	//uint8_t gu[454]

	/*
	uint8_t pos_ram = (line)*64 + (position);

	if(line==1)
	{
		pos_ram = 0x40 + position;
	}

	if(line==2)
	{
		pos_ram = 0x14 + position;
	}

	if(line==3)
	{
		pos_ram = 0x54 + position;
	}
*/
	LCD_cmd( LCD_SET_DDRAM | ( pos_ram ) );
	HAL_Delay(LCD_DELAY_MS);
}

void LCD_line_1(LCD_t *ptr)
{
	//	-ustawienie adresu DDRAM na 0x00, czyli na pierwszą pozycję w pierwszej linii,

	LCD_cmd( LCD_SET_DDRAM | ptr->line[0] );
	HAL_Delay(LCD_DELAY_MS);
}

void LCD_line_2(LCD_t *ptr)
{
	//	-ustawienie adresu DD RAM na 0x40, czyli na pierwszą pozycję w drugiej linii,

	LCD_cmd( LCD_SET_DDRAM | ptr->line[1] );
	HAL_Delay(LCD_DELAY_MS);
}

void LCD_line(LCD_t *ptr, const uint8_t line)
{
	//	-ustawienie adresu DD RAM na pierwszą pozycję w wybranej linii wyświetlacza LCD,

	LCD_cmd( LCD_SET_DDRAM | ptr->line[line] );
	HAL_Delay(LCD_DELAY_MS);
}


void LCD_move_cursor(const uint8_t direction)
{
	//	-przesunięcie kursora w wybranym kierunku,
	//	-odpalenie polecenia "Cursor or display shift",

	LCD_cmd( LCD_SHIFT | LCD_CURSOR_MOVE | direction );
	HAL_Delay(LCD_DELAY_MS);
}

void LCD_move_display(const uint8_t direction)
{
	//	-przesunięcie adresu pamięci DDRAM,
	//	-odpalenie polecenia "Cursor or display shift",

	LCD_cmd( LCD_SHIFT | LCD_DISPLAY_SHIFT | direction );
	HAL_Delay(LCD_DELAY_MS);
}

void LCD_fill_CGRAM(LCD_t *ptr, const uint8_t address, const char arr[])
{
	//	-funkcja zapisująca pojedynczy znak do pamięci CGRAM pod wskazany adres,
	//	-jako adres należy wpisać wielokrotność ósemki,

	//uint8_t buffor;

	LCD_set_CGRAM(address);		//	Ustawienie adresu na który "patrzymy" w CGRAM,

	for(uint8_t i=0; i<8; i++)
	{
	//	buffor = pgm_read_byte(arr++);
		//LCD_write_data(buffor);
	}

	LCD_line_1(ptr);				//	Powrót wskaźnika na pamięć DDRAM,
}

void LCD_show_display_modes(void)
{
	//	-wyświetlenie czterech dostępnych trybów pracy wyświetlacza,
	//	1. włącz tylko podkreślnik,
	//	2. włącz tylko mryganie,
	//	3. wyłącz wszystko,
	//	4. włącz podkreślnik i mryganie,

	const uint16_t DELAY = 2500;	//	Wartość opóźnienia w ms,

	LCD_clear_display();			//	Na wejściu czyścimy wyświetlacz,
	LCD_on();						//	Włączamy wyświetlacz,
	LCD_set_DDRAM(2,5);				//	Ustawiamy adres początku strzałki
	LCD_text("-->   <--");			//	Wyświetlamy strzałki,


	LCD_return_home();				//	Powrót adresu na początek pamięci DDRAM,
	LCD_text("CURSOR=1 BLINK=0");	//	Wyświetlenie nazwy opcji,
	LCD_set_DDRAM(2,9);				//	Adres na właściwą pozycję,
	LCD_cursor();					//	Włączenie odpowiedniego trybu,
	HAL_Delay(DELAY);				//	Czas prezentacji trybu :)

	LCD_return_home();;
	LCD_text("CURSOR=0 BLINK=1");
	LCD_set_DDRAM(2,9);
	LCD_blink();
	HAL_Delay(DELAY);

	LCD_return_home();
	LCD_text("CURSOR=0 BLINK=0");
	LCD_set_DDRAM(2,9);
	LCD_on();
	HAL_Delay(DELAY);

	LCD_return_home();
	LCD_text("CURSOR=1 BLINK=1");
	LCD_set_DDRAM(2,9);
	LCD_cursor_blink();
	HAL_Delay(DELAY);

	LCD_on();						//	Usunięcie pokreślnika i migającego kursora,
	LCD_clear_display();			//	Porządki,
}

void LCD_show_shift_modes(void)
{
	//	-wyświetlenie czterech rodzajów przesuwania kursora lub pamięci DDRAM,
	//	1. kursor w lewo,
	//	2. kursor w prawo,
	//	3. DDRAM w lewo,
	//	4. DDRAM w prawo,

	const uint16_t DELAY = 2500;	//	Wartość opóźnienia w ms,

	//	$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
	LCD_clear_display();			//	Na wejściu czyścimy wyświetlacz,
	LCD_set_DDRAM(2,5);				//	Ustawiamy adres początku strzałki
	LCD_text("-->   <--");			//	Wyświetlamy strzałki,
	LCD_cursor();					//	Włączenie wyświetlacza z kursorem

	//	$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
	LCD_return_home();				//	Powrót adresu na początek pamięci DDRAM,
	LCD_text("CURSOR LEFT   ");		//	Wyświetlenie nazwy opcji,
	LCD_set_DDRAM(2,9);				//	Adres na właściwą pozycję,
	LCD_move_cursor(LCD_LEFT);		//	Włączenie odpowiedniego trybu,
	HAL_Delay(DELAY);				//	Czas prezentacji trybu :)

	LCD_return_home();
	LCD_text("CURSOR RIGHT  ");
	LCD_set_DDRAM(2,9);
	LCD_move_cursor(LCD_RIGHT);
	HAL_Delay(DELAY);

	LCD_return_home();
	LCD_text("DDRAM LEFT  ");
	LCD_set_DDRAM(2,9);
	LCD_move_display(LCD_LEFT);
	HAL_Delay(DELAY);

	LCD_return_home();
	LCD_text("DDRAM RIGHT ");
	LCD_set_DDRAM(2,9);
	LCD_move_display(LCD_RIGHT);
	HAL_Delay(DELAY);

	LCD_on();						//	Usunięcie podkreślnika z wyświetlacza,
	LCD_clear_display();			//	Porządki,
}







