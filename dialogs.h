/*
 * dialogs.h
 *
 *  Created on: 09.10.2015
 *      Author: anton
 */

#ifndef DIALOGS_H_
#define DIALOGS_H_

#include <stdlib.h>
#include "menu.h"
#include "LCDMelt.h"
#include "Keyboard.h"

#define cursorUnderNum 0
#define cursorUnderOk 1
#define cursorUnderCancel 2

#define TEXT_OK "[\xCE\xEA]"
#define TEXT_CANCEL "[\xCE\xF2\xEC\xE5\xED\xE0]"

#define ARROW_RIGHT "\x90"
#define ARROW_LEFT "\x91"
#define ARROW_BOTTOM "\x92"
#define ARROW_TOP "\x93"
#define SYMB_SPACE "\x20"

/**
 * @brief selectFromList - позволяет выбрать элемент из списка
 * @param blist - список строк
 * @param count - количество строк в списке
 * @return - индекс выбранной строки
 */
int selectFromList(char blist[][MAX_LCD_LINE], int count) {
	LCDClrScr();
	LCDFirstLine();
    LCDReturnHome();

	// Печатаем приглашение
	boolean CanUpdate = true;
    int list_index = 0;
    int old_menu_duration = menu_duration;
    menu_duration = 0;

	while (1) {

		if (CanUpdate) {            
			printMenuEx(blist, list_index, count);
			CanUpdate = false;
		}

		// Нажата левая кнопка
        if (ReadBtnTop()) {
			if (list_index > 0) {
				list_index -= 1;
				CanUpdate = true;
			}
        }
		// Нажата левая кнопка которая ближе к центру
        if (ReadBtnBottom()) {
			if (list_index < (count - 1)) {
				list_index += 1;
				CanUpdate = true;
			}
		}

		// Нажата любая правая кнопка
        if (ReadBtnSelect()) {
			break;
        }
	}
    menu_duration = old_menu_duration;
	return list_index;
}

/**
 * @brief msgBox - показывает диалог пользователю
 * @param text - текст вопроса
 * @return - возвращает выбранное значение
 */
int msgBox(char text[]) {

    char cursorState = cursorUnderOk;
	LCDClrScr();
	// Печатаем сообщение
	LCDFirstLine();
	LCDPrint(text);
	// Выводим варианты ответа
	LCDLastLine();
    LCDPrint(ARROW_RIGHT);
    LCDPrint("[\xCE\xEA]   [\xCE\xF2\xEC\xE5\xED\xE0]"); // Ок Отмена
	while (1) {
		// Двигаем курсор
        if (ReadBtnRight() | ReadBtnLeft()) {
			if (cursorState == cursorUnderOk) {
				LCDLastLine();
                LCDPrint(SYMB_SPACE);
				cursorState = cursorUnderCancel;
				LCDCursorShift(6);
                LCDPrint(ARROW_RIGHT);
			} else {
				LCDLastLine();
				LCDCursorShift(7);
                LCDPrint(SYMB_SPACE);
				cursorState = cursorUnderOk;
				LCDLastLine();
                LCDPrint(ARROW_RIGHT);
			}
		}
		// Делаем выбор
        if (ReadBtnSelect()) {
			if (cursorState == cursorUnderOk) {
				return 1;
			} else {
				return 0;
			}
        }

	}
	return 1;

}

/**
 * @brief getNumber - Функция выводит диалог выбора числа
 * @param defaultNum - Дефолтное значение
 * @return - Возвращает выбранное число или дефолтное значение
 */
int getNumber(int defaultNum) {

	char buf[10];
	char count = 0;
	char pos = 0;
	char cursorState = cursorUnderNum;
	int workNum = defaultNum;

	LCDClrScr();
	LCDFirstLine();

	itoa(workNum, buf, 10);
	LCDPrint(buf);
	count = strlen(buf);

	while (1) {
		LCDLastLine();
		// Рисуем курсор
		if (cursorState == cursorUnderNum) {
			for (int i = 0; i < pos; i++) {
                LCDPrint(SYMB_SPACE);
			}
            LCDPrint(ARROW_TOP);
		}
		if (cursorState == cursorUnderOk) {
			LCDFirstLine();
			LCDCursorShift(11);
            LCDPrint(ARROW_RIGHT);
		}
		if (cursorState == cursorUnderCancel) {
			LCDLastLine();
			LCDCursorShift(7);
            LCDPrint(ARROW_RIGHT);
		}

		// Двигаем курсор влево
		if (ReadBtnLeft()) {
			if (cursorState == cursorUnderNum) {
				if (pos > 0) {
					LCDLastLine();
					LCDClearCurrLine();
					pos -= 1;
				} else {
					cursorState = cursorUnderCancel;
					LCDLastLine();
					LCDClearCurrLine();
				}
			} else if (cursorState == cursorUnderOk) {
				LCDFirstLine();
				LCDCursorShift(11);
                LCDPrint(SYMB_SPACE);
				cursorState = cursorUnderNum;
				pos = count - 1;
			} else {
				LCDLastLine();
				LCDCursorShift(7);
                LCDPrint(SYMB_SPACE);
				cursorState = cursorUnderOk;
			}
		}
		// Двигаем курсор вправо
        if (ReadBtnRight()) {
			if (cursorState == cursorUnderNum) {
				if (pos < (count - 1)) {
					pos += 1;
				} else {
					cursorState = cursorUnderOk;
					LCDLastLine();
					LCDClearCurrLine();
				}
			} else if (cursorState == cursorUnderOk) {
				LCDFirstLine();
				LCDCursorShift(11);
                LCDPrint(SYMB_SPACE);
				cursorState = cursorUnderCancel;
			} else {
				LCDLastLine();
				LCDCursorShift(7);
                LCDPrint(SYMB_SPACE);
				cursorState = cursorUnderNum;
				pos = 0;
			}
        }
		// Увеличиваем выделенный разряд или выходим
        if (ReadBtnTop()) {

			if (cursorState == cursorUnderNum) {
				char lastcount = count;
				int workSumm = power(10, count - pos - 1);
				if ((workNum + workSumm) < 10001) {
					LCDFirstLine();
					LCDClearCurrLine();
					LCDFirstLine();

					workNum += workSumm;
					itoa(workNum, buf, 10);
					LCDPrint(buf);
					lastcount = count;
					count = strlen(buf);
					// Если число стало длиннее
					if (lastcount < count) {
						LCDLastLine();
						LCDClearCurrLine();
						pos = pos + 1;
					}

				}
            }
		}

		// Уменьшаем выделенный раздел
        if (ReadBtnBottom()) {

			if (cursorState == cursorUnderNum) {
				char lastcount = count;
				int workSumm = power(10, count - pos - 1);
				if ((workNum - workSumm) >= 0) {
					LCDFirstLine();
					LCDClearCurrLine();
					LCDFirstLine();

					workNum -= workSumm;
					itoa(workNum, buf, 10);
					LCDPrint(buf);
					lastcount = count;
					count = strlen(buf);
				}
				// Если число стало короче
				if (lastcount > count) {
					LCDLastLine();
					LCDClearCurrLine();
					if (pos > (count - 1)) {
						pos = count - 1;
					}
				}
            }
        }

        // Делаем выбор
        if (ReadBtnSelect()) {
            if (cursorState != cursorUnderCancel) {
                return workNum;
            } else {
                return defaultNum;
            }
        }

		LCDFirstLine();
		LCDCursorShift(12);
        LCDPrint(TEXT_OK);

		LCDLastLine();
		LCDCursorShift(8);
        LCDPrint(TEXT_CANCEL);
	}
	return workNum;
}

#endif /* DIALOGS_H_ */
