/*
 * menu.h
 *
 *  Created on: 06.10.2015
 *      Author: anton
 */

#ifndef MENU_H_
#define MENU_H_

#include <avr/io.h>
#include <util/delay.h>
#include "sysfunc.h"

#define MENUSTATE_MAIN 0
char menustate = MENUSTATE_MAIN;
char main_menu_index = 0;
char menu_duration = 0;

#define MENU_ITEMS_COUNT 7
char main_menu[][MAX_LCD_LINE] = { /* Импульс */
"\xC8\xEC\xEF\xF3\xEB\xFC\xF1",
/* Пауза */
"\xCF\xE0\xF3\xE7\xE0",
/* Число импульсов */
"\xD7\xE8\xF1\xEB\xEE \xE8\xEC\xEF\xF3\xEB\xFC\xF1\xEE\xE2",
/* Связь с ПК */
"\xD1\xE2\xFF\xE7\xFC \xF1 \xCF\xCA",
/* Режим */
"\xD0\xE5\xE6\xE8\xEC",
/* Запуск */
"\xC7\xE0\xEF\xF3\xF1\xEA",
/*О устройстве*/
"\xCE \xF3\xF1\xF2\xF0\xEE\xE9\xF1\xF2\xE2\xE5" };

void printMenuEx(char text[][MAX_LCD_LINE], int index, int count) {
	LCDClrScr();
	LCDFirstLine();
	// стрелка сверху
	if (menu_duration == 0) {
        LCDPrint(SYMB_ARROW_RIGHT);
		LCDPrint(text[index]);
		LCDLastLine();
        LCDPrint(SYMB_SPACE);
		if ((index + 1) < count) {
			LCDPrint(text[index + 1]);
		}
	} else {
		// стрелка снизу
        LCDPrint(SYMB_SPACE);
		LCDPrint(text[index - 1]);
		LCDLastLine();
        LCDPrint(SYMB_ARROW_RIGHT);
		LCDPrint(text[index]);
	}
}

void printMenu(char text[][MAX_LCD_LINE], int index) {

	printMenuEx(text, index, MENU_ITEMS_COUNT);
}

#endif /* MENU_H_ */
