#include <lcom/lcf.h>
#include "statemachine.h"
#include "mouse.h"
#include <minix/sysutil.h>

struct mouse_ev mouse_event;

enum state_t mouse_state;

static int length_x = 0;
static int length_y = 0;

enum state_t get_mouse_state()
{
    return mouse_state;
}
void set_mouse_state(enum state_t state)
{
    length_x = 0;
    length_y = 0;
    switch (state)
    {
    case START:
        mouse_state = START;
        break;
    case LINE1:
        mouse_state = LINE1;
        break;
    case LINE2:
        mouse_state = LINE2;
        break;
    case VERTEX:
        mouse_state = VERTEX;
        break;
    case FINAL:
        mouse_state = FINAL;
        break;
    default:
        break;
    }
}

void mouse_read_event()
{

    mouse_event.delta_x = pp.delta_x;
    mouse_event.delta_y = pp.delta_y;

    if (mouse_event.delta_x || mouse_event.delta_y)
    {
        mouse_event.type = MOUSE_MOV;
    }
    else if (pp.lb && !pp.mb && !pp.rb)
    {
        mouse_event.type = LB_PRESSED;
    }
    else if (pp.rb && !pp.mb && !pp.lb)
    {
        mouse_event.type = RB_PRESSED;
    }
    else if (pp.rb || pp.mb || pp.lb)
    {
        mouse_event.type = BUTTON_EV;
    }
    else if (mouse_state == LINE1)
    {
        mouse_event.type = LB_RELEASED;
    }
    else if (mouse_state == LINE2)
    {
        mouse_event.type = RB_RELEASED;
    }
    else
    {
        mouse_event.type = BUTTON_EV;
    }
}

void mouse_event_handler(uint8_t x_len, uint8_t tolerance)
{

    switch (mouse_state)
    {
    case START:
        if (mouse_event.type == LB_PRESSED)
        {
            set_mouse_state(LINE1);
        }
        break;

    case LINE1:
        if (mouse_event.type == MOUSE_MOV)
        {
            if (check_abs_value_tol(pp.delta_x, tolerance, '<'))
            {
                set_mouse_state(START);
                break;
            }
            if (check_abs_value_tol(pp.delta_y, tolerance, '<'))
            {
                set_mouse_state(START);
                break;
            }

            if (check_slope(pp.delta_x, pp.delta_y))
            {
                set_mouse_state(START);
                break;
            }
            if (check_ovf(pp.x_ov, pp.y_ov))
            {
                set_mouse_state(START);
                break;
            }

            length_x += pp.delta_x;
            length_y += pp.delta_y;

            break;
        }
        else if (mouse_event.type == LB_RELEASED)
        {
            if (length_x >= x_len)
            {
                set_mouse_state(VERTEX);
                break;
            }
            else
            {
                set_mouse_state(START);
                break;
            }
        }
        else
        {
            set_mouse_state(START);
            break;
        }

    case VERTEX:
        if (mouse_event.type == MOUSE_MOV)
        {
            if ((abs(pp.delta_x) > tolerance) || (abs(pp.delta_y) > tolerance))
            {
                set_mouse_state(START);
                break;
            }
            else
            {
                length_x += pp.delta_x;
                length_y += pp.delta_y;
                break;
            }
        }
        else if (mouse_event.type == RB_PRESSED)
        {
            set_mouse_state(LINE2);
            break;
        }
        else
        {
            set_mouse_state(START);
            break;
        }

        break;

    case LINE2:
        if (mouse_event.type == MOUSE_MOV)
        {
            if (check_abs_value_tol(pp.delta_x, tolerance, '<'))
            {
                set_mouse_state(START);
                break;
            }
            if (check_abs_value_tol(pp.delta_x, tolerance, '>'))
            {
                set_mouse_state(START);
                break;
            }
            if (check_slope(pp.delta_x, pp.delta_y))
            {
                set_mouse_state(START);
                break;
            }
            if (check_ovf(pp.x_ov, pp.y_ov))
            {
                set_mouse_state(START);
                break;
            }

            length_x += pp.delta_x;
            length_y += pp.delta_y;

            break;
        }
        else if (mouse_event.type == RB_RELEASED)
        {
            if (length_x >= x_len)
            {
                set_mouse_state(FINAL);

                break;
            }
            else
            {
                set_mouse_state(START);
            }
            break;
        }

        else
        {
            set_mouse_state(START);
            break;
        }
        break;

    case FINAL:
        break;
    }
}

int check_abs_value_tol(int16_t value, uint8_t tolerance, char type)
{
    int result = 0;
    switch (type)
    {
    case '<':
        result = value < 0 && (abs(value) > tolerance);
        break;
    case '>':
        result = value > 0 && (abs(value) > tolerance);
        break;
    default:
        break;
    }
    return result;
}
int check_slope(int16_t value_x, int16_t value_y)
{
    enum state_t currentState = get_mouse_state();
    bool result = 0;
    switch (currentState)
    {
    case LINE1:
        result = value_y <= value_x;
        break;
    case LINE2:
        result = value_y > -value_x;
        break;
    default:
        break;
    }
    return result;
}

int check_ovf(bool x, bool y)
{
    return x || y;
}
