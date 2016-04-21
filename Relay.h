/*
 * Relay.h
 *
 *  Created on: 04.10.2015
 *      Author: anton
 */

#ifndef RELAY_H_
#define RELAY_H_

#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>
#include "sysfunc.h"

#define RELAY_BOTTOM 2
#define RELAY_MIDDLE 1
#define RELAY_TOP 0

void setRelayState(int RelayNum, boolean isOpen) {

    if (RelayNum == RELAY_BOTTOM)
    {
        // Не большой хак, так как нижнее реле теперь у нас PORTC5
        if (isOpen) {
            PORTC |= (1 << 5);
        } else {
            PORTC &= ~(1 << 5);
        }
    } else {
        if (isOpen) {
            PORTD |= (1 << RelayNum);
        } else {
            PORTD &= ~(1 << RelayNum);
        }
    }
}

#endif /* RELAY_H_ */
