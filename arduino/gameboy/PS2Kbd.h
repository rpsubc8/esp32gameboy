#include "keys.h"
#include <Arduino.h>
#include "gbConfig.h"

#ifdef lib_compile_vga32

 #define KB_INT_START attachInterrupt(digitalPinToInterrupt(KEYBOARD_CLK), kb_interruptHandler, FALLING)
 #define KB_INT_STOP detachInterrupt(digitalPinToInterrupt(KEYBOARD_CLK))

 extern byte lastcode;

 void IRAM_ATTR kb_interruptHandler(void);
 void kb_begin();
 boolean isKeymapChanged();
 boolean checkAndCleanKey(uint8_t scancode);
 boolean checkKey(uint8_t scancode);

#endif
