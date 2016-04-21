/*
 * main.c
 *
 *  Created on: 25.09.2015
 *      Author: anton
 *
 ************************************************************************
 * Всё путаюсь с битовыми операциями.
 *
 * Установить 7-й бит в байте (всего битов от 0 до 7)
 *
 * SREG |= (1<<7);
 *
 * Сбросить 7-й бит в байте:
 *
 * SREG &= ~ (1<<7);
 *
 * Проверить установлен ли 7-й бит в байте:
 *
 * if (SREG & 1 << 7)
 *
 * или макрос bit_test
 *
 ************************************************************************
 * Конфигурация железа - порты меги8
 ************************************************************************
 *
 *						   ________
 *			RESET - PC6 --|1*   28 |-- PC5 - Реле 3
 *		    Реле1 - PD0 --|2	27 |-- PC4 - Свободно
 *		    Реле2 - PD1	--|3	26 |-- PC3 - Кнопка вниз
 * Обязательно D+ - PD2	--|4	25 |-- PC2 - Кнопка вправо
 *		       D- - PD3	--|5	24 |-- PC1 - Кнопка выбор
 *		  LCD DB4 - PD4	--|6	23 |-- PC0 - Кнопка влево
 *		  Питание - VCC	--|7	22 |-- AGND - Соединён с GND
 *			Земля - GND	--|8	21 |-- AREF - Висит в воздухе
 *			Кварц - PB6	--|9	20 |-- AVCC - Соединён с VCC
 *			Кварц - PB7	--|10	19 |-- PB5 - SCK
 *		  LCD DB5 - PD5	--|11	18 |-- PB4 - MISO
 *		  LCD DB6 - PD6	--|12	17 |-- PB3 - MOSI
 *		  LCD DB7 - PD7	--|13	16 |-- PB2 - Кнопка вверх
 *		    LCD E - PB0	--|14	15 |-- PB1 - LCD A0
 *						  |--------|
 *
 ************************************************************************/

#define F_CPU 16000000UL

#include <math.h>
#include <avr/io.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>   /* нужно для usbdrv.h */
#include "usbconfig.h"
#include "usbdrv.h"
#include "oddebug.h"        /* This is also an example for using debug macros */
// Мои модули
#include "EEPROM_mem.h"
#include "sysfunc.h"
#include "LCDMelt.h"
#include "dialogs.h"
#include "Relay.h"
#include "Keyboard.h"
#include "menu.h"


struct dataexchange_t       // Описание структуры для передачи данных
{
   uchar b1;        // Я решил для примера написать структуру на 3 байта.
   uchar b2;        // На каждый байт подцепим ногу из PORTB. Конечно это
   uchar b3;        // не рационально (всего то 3 бита нужно).
};                  // Но в целях демонстрации в самый раз.
                    // Для наглядности прикрутить по светодиоду и созерцать :)


struct dataexchange_t pdata = {0, 0, 0};


/*PROGMEM*/ char usbHidReportDescriptor[22] = { // USB report descriptor         // Дескриптор описывает структуру пакета данных для обмена
    0x06, 0x00, 0xff,                       // USAGE_PAGE (Generic Desktop)
    0x09, 0x01,                             // USAGE (Vendor Usage 1)
    0xa1, 0x01,                             // COLLECTION (Application)
    0x15, 0x00,                             //    LOGICAL_MINIMUM (0)        // min. значение для данных
    0x26, 0xff, 0x00,                       //    LOGICAL_MAXIMUM (255)      // max. значение для данных, 255 тут не случайно, а чтобы уложиться в 1 байт
    0x75, 0x08,                             //    REPORT_SIZE (8)            // информация передается порциями, это размер одного "репорта" 8 бит
    0x95, sizeof(struct dataexchange_t),    //    REPORT_COUNT               // количество порций (у нашем примере = 3, описанная выше структура передастся за три репорта)
    0x09, 0x00,                             //    USAGE (Undefined)
    0xb2, 0x02, 0x01,                       //    FEATURE (Data,Var,Abs,Buf)
    0xc0                                    // END_COLLECTION
};
/* Здесь мы описали только один report, из-за чего не нужно использовать report-ID (он должен быть первым байтом).
 * С его помощью передадим 3 байта данных (размер одного REPORT_SIZE = 8 бит = 1 байт, их количество REPORT_COUNT = 3).
 */


/* Эти переменные хранят статус текущей передачи */
static uchar    currentAddress;
static uchar    bytesRemaining;


/* usbFunctionRead() вызывается когда хост запрашивает порцию данных от устройства
 * Для дополнительной информации см. документацию в usbdrv.h
 */
uchar   usbFunctionRead(uchar *data, uchar len)
{
    if(len > bytesRemaining)
        len = bytesRemaining;

    uchar *buffer = (uchar*)&pdata;

    if(!currentAddress)        // Ни один кусок данных еще не прочитан.
    {                          // Заполним структуру для передачи
        if ( PINB & _BV(1) )
            pdata.b1 = 1;
        else
            pdata.b1 = 0;


        if ( PINB & _BV(2) )
            pdata.b2 = 1;
        else
            pdata.b2 = 0;


        if ( PINB & _BV(3) )
            pdata.b3 = 1;
        else
            pdata.b3 = 0;
    }

    uchar j;
    for(j=0; j<len; j++)
        data[j] = buffer[j+currentAddress];

    currentAddress += len;
    bytesRemaining -= len;
    return len;
}


/* usbFunctionWrite() вызывается когда хост отправляет порцию данных к устройству
 * Для дополнительной информации см. документацию в usbdrv.h
 */
uchar   usbFunctionWrite(uchar *data, uchar len)
{
    if(bytesRemaining == 0)
        return 1;               /* конец передачи */

    if(len > bytesRemaining)
        len = bytesRemaining;

    uchar *buffer = (uchar*)&pdata;

    uchar j;
    for(j=0; j<len; j++)
        buffer[j+currentAddress] = data[j];

    currentAddress += len;
    bytesRemaining -= len;

    if(bytesRemaining == 0)     // Все данные получены
    {                           // Выставим значения на PORTB
        if ( pdata.b1 )
            setRelayState(RELAY_TOP, true);
        else
            setRelayState(RELAY_TOP, false);


        if ( pdata.b2 )
            setRelayState(RELAY_MIDDLE, true);
        else
            setRelayState(RELAY_MIDDLE, false);


        if ( pdata.b3 )
            setRelayState(RELAY_BOTTOM, true);
        else
            setRelayState(RELAY_BOTTOM, false);
    }

    return bytesRemaining == 0; /* 0 означает, что есть еще данные */
}

/* ------------------------------------------------------------------------- */

usbMsgLen_t usbFunctionSetup(uchar data[8])
{
usbRequest_t    *rq = (void *)data;

    if((rq->bmRequestType & USBRQ_TYPE_MASK) == USBRQ_TYPE_CLASS){    /* HID устройство */
        if(rq->bRequest == USBRQ_HID_GET_REPORT){  /* wValue: ReportType (highbyte), ReportID (lowbyte) */
            // у нас только одна разновидность репорта, можем игнорировать report-ID
            bytesRemaining = sizeof(struct dataexchange_t);
            currentAddress = 0;
            return USB_NO_MSG;  // используем usbFunctionRead() для отправки данных хосту
        }else if(rq->bRequest == USBRQ_HID_SET_REPORT){
            // у нас только одна разновидность репорта, можем игнорировать report-ID
            bytesRemaining = sizeof(struct dataexchange_t);
            currentAddress = 0;
            return USB_NO_MSG;  // используем usbFunctionWrite() для получения данных от хоста
        }
    }else{
        /* остальные запросы мы просто игнорируем */
    }
    return 0;
}
/* ------------------------------------------------------------------------- */


int def_pulse_duration;
int def_pause_duration;
int def_impulse_count;
int def_mode;

#define COUNTOFMODES 4
char listOfModes[][MAX_LCD_LINE] =
		{ "NAMUR 2.2K-5.6K", "NAMUR 2.2K-40K",
				"\xD0\xE0\xE7\xF0\xFB\xE2"/*Разрыв*/,
				"\xC2\xF1\xE5 \xF0\xE5\xEB\xE5" /*Все реле*/};

// Инициализация портов ввода\вывода
void initIO() {

	// Порт передачи данных в экран
	PORTD = 0;
	DDRD = 0xFF;

	// На вывод пины LCD E и A0, остальные на вход
	PORTB = 0;
	DDRB = 0b00000011;

	// На вывод пины отвечающие за управление реле, на вход кнопки
	PORTC = 0;
    DDRC = 0b00100000;
}

int main(void) {

	// Настраиваем порты
	initIO();

	// Загружаем переменные
    def_pulse_duration = readPulseDuration();
	def_pause_duration = readPauseDuration();
	def_impulse_count = readImpulseCount();
    def_mode = readMode();

    if (def_pulse_duration == -1) {
        def_pulse_duration = 100;
    }
    if (def_pause_duration == -1) {
        def_pause_duration = 100;
    }
    if (def_impulse_count == -1) {
        def_impulse_count = 100;
    }

	// Настраиваем экран
	LCDinit();
	LCDClrScr();
	LCDFirstLine();

	// Печатаем приглашение
	boolean CanUpdate = true;
	main_menu_index = 0;

// Основной цикл программы
	while (1) {

		if (CanUpdate) {
			printMenu(main_menu, main_menu_index);
			CanUpdate = false;
		}

        // Листаем вверх
		if (ReadBtnTop())
		{
            if ((menustate == MENUSTATE_MAIN) && (main_menu_index > 0)) {
                if (menu_duration == 1) {
                    menu_duration = 0;
                }
                main_menu_index -= 1;
                CanUpdate = true;
            }
		}

        // Листаем вниз
		if (ReadBtnBottom())
		{
            if ((menustate == MENUSTATE_MAIN) && (main_menu_index < (MENU_ITEMS_COUNT - 1))) {
                if (menu_duration == 0) {
                    menu_duration = 1;
                }
                main_menu_index += 1;
                CanUpdate = true;
			}
		}

		// Нажата правая кнопка которая ближе к центру
		if (ReadBtnSelect()) {
			switch (main_menu_index) {
			case 0: // Длительсность импульса
				def_pulse_duration = getNumber(def_pulse_duration);
				writePulseDuration(def_pulse_duration);
				break;
			case 1: // Длительность паузы
				def_pause_duration = getNumber(def_pause_duration);
				writePauseDuration(def_pause_duration);
				break;
			case 2: // Количество импульсов
				def_impulse_count = getNumber(def_impulse_count);
				writeImpulseCount(def_impulse_count);
				break;
			case 3: // Связь с ПК
                LCDClrScr();
                LCDFirstLine();
                LCDPrint("HID-USB - START");

                usbInit();
                usbDeviceConnect();     // подключаемся

                sei();                  // разрешаем прерывания

                for(;;){               // главный цикл программы
                    usbPoll();          // эту функцию надо регулярно вызывать с главного цикла, максимальная задержка между вызовами - 50 ms
                }

				break;
			case 4: // Режим
				def_mode = selectFromList(listOfModes, COUNTOFMODES);
				writeMode(def_mode);
				break;
			case 5: // Запустить
                if (msgBox("\xC7\xE0\xEF\xF3\xF1\xF2\xE8\xF2\xFC?") == true) {

					LCDClrScr();
					LCDFirstLine();

					// Определим из скольких импульсов состоит один шаг индикатора
					int step = round(def_impulse_count / 16);
					int pos = 0;
					int progress = 0;

					for (int i = 0; i < def_impulse_count; i++) {
						// Включаем реле
						double pause_ms = def_pulse_duration;

						// NAMUR 2.2K-5.6K
						if (def_mode == 0) {
							setRelayState(RELAY_BOTTOM, true);
						} else
						// NAMUR 2.2K-40K
						if (def_mode == 1) {
							setRelayState(RELAY_MIDDLE, true);
						} else
						// Разрыв
						if (def_mode == 2) {
							setRelayState(RELAY_TOP, true);
						} else // Все
						{
							setRelayState(RELAY_BOTTOM, true);
							setRelayState(RELAY_MIDDLE, true);
							setRelayState(RELAY_TOP, true);
						}
						// Ждём
						delay_ms(pause_ms);

						// Выключаем реле

						// NAMUR 2.2K-5.6K
						if (def_mode == 0) {
							setRelayState(RELAY_BOTTOM, false);
						} else
						// NAMUR 2.2K-40K
						if (def_mode == 1) {
							setRelayState(RELAY_MIDDLE, false);
						} else
						// Разрыв
						if (def_mode == 2) {
							setRelayState(RELAY_TOP, false);
						} else // Все
						{
							setRelayState(RELAY_BOTTOM, false);
							setRelayState(RELAY_MIDDLE, false);
							setRelayState(RELAY_TOP, false);
						}

						delay_ms(def_pause_duration);

						pos += 1;
						if (pos > step) {
							LCDFirstLine();
							LCDCursorShift(progress);
                            LCDPrint(".");
							progress += 1;
							LCDLastLine();
							char buf[40];
							itoa(i, buf, 10);
							LCDPrint(buf);
							pos = 0;
						}
					}
				}
				break;
			case 6:
				//Об устройстве
				LCDClrScr();
				LCDFirstLine();
                LCDPrint(" Anton Sinitsyn");
				LCDLastLine();
                LCDPrint("Russia 2015 oct");
				while (1) {
					if (ReadBtnLeft() | ReadBtnTop() | ReadBtnBottom() | ReadBtnRight() | ReadBtnSelect()) {
						break;
					}
				}
				break;
			default:
				break;
			}
			CanUpdate = true;
		}
	}
}
