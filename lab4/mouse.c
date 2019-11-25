#include <lcom/lcf.h>
#include <lcom/lab4.h>
#include <stdint.h>
#include <math.h>

#include "i8042.h"
#include "mouse.h"
#include "statemachine.h"

int mouseHookID = MOUSE_IRQ;
uint8_t mouseData;
uint32_t packetCounter;
uint8_t mouseBytes[3];
uint8_t packetIndex = 0;
struct packet pp;

int mouse_subscribe_int(uint8_t *bit_no)
{
    mouseHookID = (int)*bit_no;

    if (sys_irqsetpolicy(MOUSE_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &mouseHookID) != 0)
    {
        return 1;
    }
    *bit_no = (uint8_t)BIT(*bit_no);
    return 0;
}

int mouse_unsubscribe_int()
{
    if (sys_irqrmpolicy(&mouseHookID) != 0)
    {
        return 1;
    }
    return 0;
}

void(mouse_ih)()
{
    uint8_t kbc_status;
    kbc_read_byte(KBC_STAT_REG, &kbc_status);
    if ((kbc_status & (PAR_ERR | TO_ERR)) == 0)
    {
        kbc_read_byte(KBC_OUT_BUF, &mouseData);
    }
}

int mouse_read_byte()
{

    uint8_t kbc_status;

    kbc_read_byte(KBC_STAT_REG, &kbc_status);

    if ((kbc_status & OBF) && (kbc_status & AUX))
    {
        mouse_ih();
        return 0;
    }
    return 1;
}
void initialize_packet(struct packet *pp, uint8_t packetBytes[])
{

    for (int i = 0; i < 3; i++)
    {
        pp->bytes[i] = packetBytes[i];
    }

    pp->lb = util_get_bit(packetBytes[0], LB_BIT);
    pp->rb = util_get_bit(packetBytes[0], RB_BIT);
    pp->mb = util_get_bit(packetBytes[0], MB_BIT);
    pp->x_ov = util_get_bit(packetBytes[0], X_OV_BIT);
    pp->y_ov = util_get_bit(packetBytes[0], Y_OV_BIT);

    if (packetBytes[0] & X_MSB)
    {
        pp->delta_x = util_twos_complement(packetBytes[1]);
    }
    else
    {
        pp->delta_x = packetBytes[1];
    }

    if (packetBytes[0] & Y_MSB)
    {
        pp->delta_y = util_twos_complement(packetBytes[2]);
    }
    else
    {
        pp->delta_y = packetBytes[2];
    }
}
int mouse_evaluate_bytes()
{
    if ((packetIndex == 0) && ((mouseData & BIT(3)) == 0))
    {
        return 0;
    }

    mouseBytes[packetIndex] = mouseData;
    if (packetIndex == 2)
    {
        packetCounter--;
        packetIndex = 0;
        return 1;
    }
    packetIndex++;
    return 0;
}

int mouse_send_command(uint8_t cmd)
{
    uint8_t kbc_status, ack;

    do
    {

        kbc_write_byte(KBC_CMD_REG, WRITE_MOUSE_BYTE);

        kbc_write_byte(KBC_IN_BUF, cmd);

        kbc_read_byte(KBC_STAT_REG, &kbc_status);

        if ((kbc_status & OBF) && (kbc_status & AUX))
        {
            kbc_read_byte(KBC_OUT_BUF, &ack);

            if (ack == ERROR)
                return 1;
        }
    } while (ack != ACK);

    return 0;
}

int kbc_write_byte(uint8_t port, uint8_t byte)
{
    uint8_t kbc_status;
    uint8_t tryCounter = 0;

    while (1)
    {

        if (kbc_read_byte(KBC_STAT_REG, &kbc_status) != 0)
            continue;

        if ((kbc_status & IBF) == 0)
        {
            return sys_outb(port, byte);
        }
        else
        {
            tryCounter++;
        }

        if (tryCounter == KBC_N_TRIES)
            return 1;

        tickdelay(micros_to_ticks(POLL_DELAY));
    }
}

int kbc_read_byte(uint8_t port, uint8_t *byte)
{
    uint8_t kbc_data;

    if (util_sys_inb(port, &kbc_data) != 0)
        return 1;
    else
    {
        *byte = kbc_data;
        return 0;
    }
}

int mouse_write_dflt_kbc_cmd_byte()
{
    uint8_t kbc_dflt_byte = minix_get_dflt_kbc_cmd_byte();

    kbc_write_byte(KBC_CMD_REG, WRITE_COM_BYTE);
    kbc_write_byte(KBC_IN_BUF, kbc_dflt_byte);

    return 0;
}

bool util_get_bit(uint8_t val, uint8_t bit_no)
{
    bool bit_value;
    bit_value = ((val & BIT(bit_no)) >> bit_no);
    return bit_value;
}

int16_t util_twos_complement(uint8_t value)
{
    return -((value ^ 0xFF) + 1);
}

void mouse_gesture(uint8_t x_len, uint8_t tolerance)
{
    if (mouse_evaluate_bytes())
    {
        initialize_packet(&pp, mouseBytes);
        mouse_read_event();
        mouse_event_handler(x_len, tolerance);
        mouse_print_packet(&pp);
    }
}

