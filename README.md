# LCD
Wyświetlacz LCD na sterowniku HD44780
-pierwsza wersja biblioteki,
-tworzona na podstawie biblioteki dla AVR, oraz dla ESP32,
-komunikacja za pomocą 8 bitowego ekspandera I2C - PCF8574
DZIAŁANIE:
-na początek chodziło o poprawną inicjalizację wyświetlacza oraz wyświetlenie przykładowego napisu,
-znajdują się tutaj również funkcje do wygaszania wyświetlacza, jego włączania, aktywacji mrygania polem,
TODO:
-zamienić foo AL_I2C_Master_Transmit() na taką, która będzie przyjmować mniej argumentów,
-likwidacja HAL_Delay(),
-dodanie wyświetlania liczb,
-dodanie pozostałych funkcji do obsługi podkreślnika,
