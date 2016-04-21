TEMPLATE = app

HEADERS += EEPROM_mem.h \
            sysfunc.h \
            LCDMelt.h \
            dialogs.h \
            Relay.h \
            Keyboard.h \
            menu.h \
            usbconfig.h \
            usbdrv.h

SOURCES += main.c \
            usbdrvasm.S \
            usbdrv.c \
            oddebug.c
