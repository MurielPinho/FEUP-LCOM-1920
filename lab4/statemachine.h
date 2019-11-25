enum state_t
{
    START,
    LINE1,
    VERTEX,
    LINE2,
    FINAL
};

enum state_t get_mouse_state();

void mouse_read_event();

void mouse_event_handler(uint8_t x_len, uint8_t tolerance);

int check_abs_value_tol(int16_t value, uint8_t tolerance, char type);

int check_slope(int16_t value_x, int16_t value_y);

int check_ovf(bool x, bool y);
