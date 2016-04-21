/*
 * EEPROM_mem.h
 *
 *  Created on: 16.10.2015
 *      Author: anton
 */

#ifndef EEPROM_MEM_H_
#define EEPROM_MEM_H_

#include <avr/eeprom.h>

// 16-ми разрядная переменная в EEPROM
uint16_t eeprom_pulse_duration EEMEM = 2000;
uint16_t eeprom_pause_duration EEMEM = 1000;
uint16_t eeprom_impulse_count EEMEM = 1000;
uint16_t eeprom_mode EEMEM = 0;

int readPulseDuration() {
	return (int) eeprom_read_word(&eeprom_pulse_duration);
}

void writePulseDuration(int val) {
	eeprom_write_word(&eeprom_pulse_duration, val);
}

int readPauseDuration() {
	return (int) eeprom_read_word(&eeprom_pause_duration);
}

void writePauseDuration(int val) {
	eeprom_write_word(&eeprom_pause_duration, val);
}

int readImpulseCount() {
	return (int) eeprom_read_word(&eeprom_impulse_count);
}

void writeImpulseCount(int val) {
	eeprom_write_word(&eeprom_impulse_count, val);
}

int readMode() {
	return (int) eeprom_read_word(&eeprom_mode);
}

void writeMode(int val) {
	eeprom_write_word(&eeprom_mode, val);
}

#endif /* EEPROM_MEM_H_ */
