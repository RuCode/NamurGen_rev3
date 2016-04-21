/*
 * sysfunc.h
 *
 *  Created on: 06.10.2015
 *      Author: anton
 */

#ifndef SYSFUNC_H_
#define SYSFUNC_H_

#define boolean char
#define false 0
#define true 1

// Возведение в степень
int power(int x, int n) {
	if (n == 0)
		return 1;
	if (n < 0)
		return power(1.0 / x, -n);
	return x * power(x, n - 1);
}

// Длина строки
int strlen(char buff[]) {
	int i = 0;
	while (buff[i] != 0) {
		i += 1;
	}
	return i;
}

void delay_ms(int ms)
{
  while (0 < ms)
  {
    _delay_ms(1);
    --ms;
  }
}


#endif /* SYSFUNC_H_ */
