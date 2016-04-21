/*
 * Keyboard.h
 *
 *  Created on: 04.10.2015
 *      Author: anton
 */

#ifndef KEYBOARD_H_
#define KEYBOARD_H_

#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>

// На порту С
#define BTN_PIN_LEFT 0
#define BTN_PIN_SELECT 1
#define BTN_PIN_RIGHT 2
#define BTN_PIN_BOTTOM 3

// На порту B
#define BTN_PIN_TOP 2


int ReadButtonFromPORTB(int PinBtn) {
// Если кнопка нажата возвращает 1
	int result = 0;
	if (PINB & (1 << PinBtn)) { // Берём первый замер
		_delay_ms(20); // ждём 0.020 сек
		if (PINB & (1 << PinBtn)) { // Делаем второй замер
// Если они совпадают, то кнопка нажата
			result = 1;
// Однако мы будем ждать пока юзер не отпустит кнопку
			while (PINB & (1 << PinBtn)) {
// А надо это что бы повторно не срабатывало событие нажатия кнопки
			}
		}
	}
	return result;
}

int ReadButtonFromPORTC(int PinBtn) {
// Аналогична предыдущей
	int result = 0;
	if (PINC & (1 << PinBtn)) {
		_delay_ms(20);
		if (PINC & (1 << PinBtn)) {
			result = 1;
			while (PINC & (1 << PinBtn)) {
			}
		}
	}
	return result;
}

// Читаем состояние кнопки вверх
int ReadBtnTop() {
	return ReadButtonFromPORTB(BTN_PIN_TOP);
}

// Читаем состояние кнопки слева
int ReadBtnLeft() {
	return ReadButtonFromPORTC(BTN_PIN_LEFT);
}

// Читаем состояние кнопки вниз
int ReadBtnBottom() {
	return ReadButtonFromPORTC(BTN_PIN_BOTTOM);
}

// Читаем состояние кнопки вправо
int ReadBtnRight() {
	return ReadButtonFromPORTC(BTN_PIN_RIGHT);
}

// Читаем состояние кнопки выбрать
int ReadBtnSelect() {
	return ReadButtonFromPORTC(BTN_PIN_SELECT);
}

#endif /* KEYBOARD_H_ */
