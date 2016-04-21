/*
 * LCDMelt.h
 *
 *  Created on: 03.10.2015
 *      Author: anton
 ************************************************************************
 * 				Модуль для работы с LCD Melt 15S2
 ************************************************************************
 */


/*
 *  							АЛФАВИТ МЭЛТ
 *
 * 	  А   а   Б   б   В   в   Г   г   Д   д   Е   е   Ё   ё   Ж   ж
 * "\xC0\xE0\xC1\xE1\xC2\xE2\xC3\xE3\xC4\xE4\xC5\xE5\xA8\xB8\xC6\xE6"
 *
 *    З   з   И   и   Й   й   К   к   Л   л   М   м   Н   н   О   о
 * "\xC7\xE7\xC8\xE8\xC9\xE9\xCA\xEA\xCB\xEB\xCC\xEC\xCD\xED\xCE\xEE"
 *
 *    П   п   Р   р   С   с   Т   т   У   у   Ф   ф   Х   х   Ц   ц
 * "\xCF\xEF\xD0\xF0\xD1\xF1\xD2\xF2\xD3\xF3\xD4\xF4\xD5\xF5\xD6\xF6"
 *
 *    Ч   ч   Ш   ш   Щ   щ   Ъ   ъ   Ы   ы   Ь   ь   Э   э   Ю   ю
 * "\xD7\xF7\xD8\xF8\xD9\xF9\xDA\xFA\xDB\xFB\xDC\xFC\xDD\xFD\xDE\xFE"
 *
 *    Я   я
 * "\xDF\xFF"
 *
 */

#ifndef LCDMELT_H_
#define LCDMELT_H_

#include <avr/io.h>
#include <util/delay.h>
#include "sysfunc.h"

#define MAX_LCD_LINE 16

#define byte char
#define bit_test(var, bitno)((var) & 1 << (bitno))
#define LCD_E_CLR (PORTB &= ~(1 << P_E))
#define LCD_E_SET (PORTB |= (1 << P_E))
#define LCD_A0_CLR (PORTB &= ~(1 << P_A0))
#define LCD_A0_SET (PORTB |= (1 << P_A0))

// Символы
#define SYMB_ARROW_RIGHT "\x90"
#define SYMB_SPACE "\x20"

// Линия данных для ЖКИ расположена в PD
#define P_DB7	7
#define P_DB6	6
#define P_DB5	5
#define P_DB4	4
#define P_DB3	3
#define P_DB2	2
#define P_DB1	1
#define P_DB0	0

// Линия управления для ЖКИ расположена в PB, RW всегда нуль
#define P_E		0
#define P_A0	1

void WriteByte(byte b, byte /*bit*/cd) {
// Записать байт данных в экран

    _delay_us(40);
    if (cd == 1) {
        LCD_A0_SET;
    } else {
        LCD_A0_CLR;
    }


    if (b & (1 << 7 /*позиция бита*/)) {
        PORTD |= (1 << 7);
    } else {
        PORTD &= ~(1 << 7);
    }
    if (b & (1 << 6 /*позиция бита*/)) {
        PORTD |= (1 << 6);
    } else {
        PORTD &= ~(1 << 6);
    }
    if (b & (1 << 5 /*позиция бита*/)) {
        PORTD |= (1 << 5);
    } else {
        PORTD &= ~(1 << 5);
    }
    if (b & (1 << 4 /*позиция бита*/)) {
        PORTD |= (1 << 4);
    } else {
        PORTD &= ~(1 << 4);
    }
    _delay_us(1); //Это время предустановки адреса (tAS)
    LCD_E_SET;
    _delay_us(1); //Время предустановки данных попало сюда (tDSW)
    LCD_E_CLR;
    _delay_us(1); //Минимально допустимый интервал между сигналами E=1

    if (b & (1 << 3 /*позиция бита*/)) {
        PORTD |= (1 << 7);
    } else {
        PORTD &= ~(1 << 7);
    }
    if (b & (1 << 2 /*позиция бита*/)) {
        PORTD |= (1 << 6);
    } else {
        PORTD &= ~(1 << 6);
    }
    if (b & (1 << 1 /*позиция бита*/)) {
        PORTD |= (1 << 5);
    } else {
        PORTD &= ~(1 << 5);
    }
    if (b & (1 << 0 /*позиция бита*/)) {
        PORTD |= (1 << 4);
    } else {
        PORTD &= ~(1 << 4);
    }

    _delay_us(1); //Это время предустановки адреса (tAS)
    LCD_E_SET;
    _delay_us(1); //Время предустановки данных попало сюда (tDSW)
    LCD_E_CLR;
    _delay_us(1); //Минимально допустимый интервал между сигналами E=1

    /*При необходимости настроить здесь шину данных на вывод
        LCD.RW=0; LCD.A0=cd; LCD.D=b>>4;
        Delay(>40ns);		//Это время предустановки адреса (tAS)
        LCD.E=1; Delay(>230ns);	//Время предустановки данных попало сюда (tDSW)
        LCD.E=0; Delay(>270ns);	//Минимально допустимый интервал между сигналами E=1
        LCD.D=b&0x0F;		//Выдадим младшие 4 бита (при этом биты RW и A0 измениться не должны!)
        LCD.E=1; Delay(>230ns);	//Время предустановки данных попало сюда (tDSW)
        LCD.E=0; Delay(>270ns);	//Минимально допустимый интервал между сигналами E=1


     **
     * 8 битный режим, работает
     **
    _delay_us(40);
	//LCD.RW = 0;
	if (cd == 1) {
		LCD_A0_SET;
	} else {
		LCD_A0_CLR;
	}
	PORTD = b;
	_delay_us(1); //Это время предустановки адреса (tAS)
	LCD_E_SET;
	_delay_us(1); //Время предустановки данных попало сюда (tDSW)
	LCD_E_CLR;
	_delay_us(1); //Минимально допустимый интервал между сигналами E=1

    */
}

void WriteCmd(byte b) {
	// Передаём команду в контроллер экрана
	WriteByte(b, 0);
}

void WriteData(byte b) {
	// Передаём символ в контроллер экрана
	WriteByte(b, 1);
}

void LCDinit(void) {
	// Инициализация дисплея

    LCD_E_CLR;
    _delay_ms(20);
    LCD_A0_CLR;
    WriteCmd(0x20); //Устанавливаем 4-х битный режим
    WriteCmd(0x2A); //Настройка правильного режима ЖКИ
    WriteCmd(0x0C); //Включение индикатора, курсор выключен
    WriteCmd(0x01); //Очистка индикатора
    _delay_ms(1.5);
    WriteCmd(0x06); //Установка режима ввода данных: сдвигать курсор вправо

/**
 * 8 битный режим, работает
 *
	LCD_E_CLR;
	_delay_ms(20);
//При необходимости настроить здесь шину данных на вывод
//LCD.RW = 0; всегда нуль
	LCD_A0_CLR;
	WriteCmd(0x30);
    //WriteCmd(0x30);
    //WriteCmd(0x30);
//Здесь индикатор входит в рабочий режим с установленным типом интерфейса и можно подавать команды как обычно
	WriteCmd(0x3A); //Настройка правильного режима ЖКИ
	WriteCmd(0x0C); //Включение индикатора, курсор выключен
	WriteCmd(0x01); //Очистка индикатора
	_delay_ms(1.5);
	WriteCmd(0x06); //Установка режима ввода данных: сдвигать курсор вправо
  **/
}

void LCDFirstLine() {
	// Переводим курсор в начало первой строки

	WriteCmd(0x80); //Установка курсора в начало первой строки индикатора
	_delay_ms(1.5);
}

void LCDLastLine() {
	// Переводим курсор в начало второй строки

	LCDFirstLine();
	WriteCmd(0x80 + 0x40); //Установка курсора в начало второй строки индикатора
	_delay_ms(1.5);
}

void LCDClrScr() {
	//Очистка индикатора

	WriteCmd(0x01);
	_delay_ms(1.5);
}

void LCDPrint(char str[]) {
	// Печатаем строку на экране

	int i = 0;
	while (str[i] != '\x00') {
		WriteData(str[i]);
		i += 1;
	}
}

/* Не прокатило((((
 *
void LCDPrintRu(char str[]) {

#define SYMB_COUNT 66
	// Печатаем русскую строку на экране

	char local_ru[] =
			"АаБбВвГгДдЕеЁёЖжЗзИиЙйКкЛлМмНнОоПпРрСсТтУуФфХхЦцЧчШшЩщЪъЫыЬьЭэЮюЯя";
	char local_melt[] =
			"\xC0\xE0\xC1\xE1\xC2\xE2\xC3\xE3\xC4\xE4\xC5\xE5\xA8\xB8\xC6\xE6" // АаБбВвГгДдЕеЁёЖж
					"\xC7\xE7\xC8\xE8\xC9\xE9\xCA\xEA\xCB\xEB\xCC\xEC\xCD\xED\xCE\xEE"// ЗзИиЙйКкЛлМмНнОо
					"\xCF\xEF\xD0\xF0\xD1\xF1\xD2\xF2\xD3\xF3\xD4\xF4\xD5\xF5\xD6\xF6"// ПпРрСсТтУуФфХхЦц
					"\xD7\xF7\xD8\xF8\xD9\xF9\xDA\xFA\xDB\xFB\xDC\xFC\xDD\xFD\xDE\xFE"// ЧчШшЩщЪъЫыЬьЭэЮю
					"\xDF\xFF";// Яя

	// Проходим по всему тексту
	for (int i = 0; i < 16; i++) {

		// Проверяем не наткнулись ли мы на конец строки
		if (str[i] == 0) {
			break;
		}

		// Проходим по локали
		char isAvaibleSymb = false;
		for (int j = 0; j < SYMB_COUNT; j++) {
			// Если в локали есть наш символ выводим его
			if (local_ru[j] == str[i]) {
				isAvaibleSymb = true;
				WriteData(local_melt[j]);
				break;
			}
		}
		// Если в локали не было символа то выводим оригинальный
		if (isAvaibleSymb == false) {
			WriteData(str[i]);
		}
	}
}
 */

// Возвращает курсор вначало строки
void LCDReturnHome() {
	WriteCmd(0b00000011);
	_delay_ms(1);
}

// Очищаем строку
void LCDClearCurrLine() {

	for (int i = 0; i < 16; i++) {
		WriteData(' ');
	}
}

// Сдвинуть курсор вправа
void LCDCursorShift(int posShift) {

	for (int i = 0; i < posShift; i++) {
		WriteCmd(0b00010100);
		_delay_ms(1);
	}
}
#endif /* LCDMELT_H_ */
