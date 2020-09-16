#include <Arduino.h>

#define KEYBOARD_DATA 32
#define KEYBOARD_CLK 33

unsigned int shift = 0;
byte lastcode = 0;
boolean keyup = false;
boolean shift_presed = false;
boolean symbol_pressed = false;
byte rc = 0;
//volatile byte keymap[256];
//volatile byte oldKeymap[256];
volatile unsigned char keymap[256];
volatile unsigned char oldKeymap[256];

void IRAM_ATTR kb_interruptHandler(void) {
    static uint8_t bitcount = 0;
    static uint8_t incoming = 0;
    static uint32_t prev_ms = 0;
    uint32_t now_ms;
    uint8_t n, val;

    int clock = digitalRead(KEYBOARD_CLK);
    if (clock == 1)
        return;

    val = digitalRead(KEYBOARD_DATA);
    now_ms = millis();
    if (now_ms - prev_ms > 250) {
        bitcount = 0;
        incoming = 0;
    }
    prev_ms = now_ms;
    n = bitcount - 1;
    if (n <= 7) {
        incoming |= (val << n);
    }
    bitcount++;
    if (bitcount == 11) {

        if (1) {
            if (keyup == true) {
                if (keymap[incoming] == 0) {
                    keymap[incoming] = 1;
                } else {
                    // Serial.println("WARNING: Keyboard cleaned");
                    for (int gg = 0; gg < 256; gg++)
                        keymap[gg] = 1;
                }
                keyup = false;
            } else
                keymap[incoming] = 0;

            if (incoming == 240)
                keyup = true;
            else
                keyup = false;
        }
        bitcount = 0;
        incoming = 0;
    }
}

void kb_begin() {
    pinMode(KEYBOARD_DATA, INPUT_PULLUP);
    pinMode(KEYBOARD_CLK, INPUT_PULLUP);
    digitalWrite(KEYBOARD_DATA, true);
    digitalWrite(KEYBOARD_CLK, true);
    attachInterrupt(digitalPinToInterrupt(KEYBOARD_CLK), kb_interruptHandler, FALLING);

    memset((unsigned char *)keymap, 1, sizeof(keymap));
    memset((unsigned char *)oldKeymap, 1, sizeof(oldKeymap));
    //}
}

// Check if keymatrix is changed
boolean isKeymapChanged() { return (keymap != oldKeymap); }

// Check if key is pressed and clean it
boolean checkAndCleanKey(uint8_t scancode) {
    if (keymap[scancode] == 0) {
        keymap[scancode] = 1;
        return true;
    }
    return false;
}

//*****************************************
boolean checkKey(uint8_t scancode) {
    if (keymap[scancode] == 0)        
        return true;
    return false;
}
