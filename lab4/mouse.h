
extern uint8_t mouseData;
extern struct packet pp;
extern uint32_t packetCounter;
extern uint8_t packetIndex;
extern int mouseHookID;
extern uint32_t interruptCounter;
extern uint8_t mouseBytes[3];


int16_t util_twos_complement(uint8_t value);

bool util_get_bit(uint8_t val, uint8_t bit_no);

int mouse_subscribe_int(uint8_t *bit_no);

int mouse_unsubscribe_int();

int mouse_read_byte();

int mouse_evaluate_bytes();

int mouse_send_command(uint8_t cmd);

int mouse_write_dflt_kbc_cmd_byte();

void initialize_packet(struct packet *pp, uint8_t bytes[]);

int kbc_write_byte(uint8_t port, uint8_t byte);

int kbc_read_byte(uint8_t port, uint8_t *byte);

void mouse_gesture(uint8_t x_len, uint8_t tolerance);
