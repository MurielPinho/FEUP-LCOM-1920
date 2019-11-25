extern uint8_t scannedKey;
extern uint8_t size;
extern uint32_t interruptCounter;

int kbd_subscribe_int(uint8_t *bit_no);

int kbd_unsubscribe_int();

int kbc_read_keycode();

int kbc_interrupts_enable();

void kbd_scancode();
