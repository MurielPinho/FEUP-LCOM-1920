#include <lcom/lcf.h>
#include <lcom/lab3.h>
#include <stdint.h>

#include "i8042.h"

uint32_t sysInbCounter =0;
uint8_t scannedKey;
uint8_t size = 0;
uint8_t scanBytes[2];
int kbdHookID = KEYBOARD_IRQ;

int kbd_subscribe_int(uint8_t *bit_no)
{
  kbdHookID = (int)*bit_no;

  if (sys_irqsetpolicy(KEYBOARD_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &kbdHookID) != OK)
  {
    return 1;
  }
  *bit_no = (uint8_t)BIT(*bit_no);
  return 0;
}

int kbd_unsubscribe_int()
{
  if (sys_irqrmpolicy(&kbdHookID) != OK)
  {
    return 1;
  }
  return 0;
}

void(kbc_ih)()
{
  uint8_t status, tempKey;
  util_sys_inb(KBC_STAT_REG, &status);
  if ((status & (PAR_ERR | TO_ERR)) == 0)
  {
    if (util_sys_inb(KBC_OUT_BUF, &tempKey) == 0)
    {
      scannedKey = tempKey;
    }
  }
}

int kbc_read_keycode()
{

  uint8_t status;

  if (util_sys_inb(KBC_STAT_REG, &status) == 1)
  {
    return 1;
  }
  if (status & OBF)
  {
    kbc_ih();
    return 0;
  }
  return 1;
}

int kbc_interrupts_enable()
{
  uint8_t kbc_Status, kbc_Command;

  if (util_sys_inb(KBC_STAT_REG, &kbc_Status) != 0)
  {
    return 1;
  }

  if ((kbc_Status & IBF) == 1)
  {
    return 1;
  }

  if (sys_outb(KBC_CMD_REG, READ_COM_BYTE) != 0)
  {
    return 1;
  }
  if (util_sys_inb(KBC_OUT_BUF, &kbc_Command) != 0)
  {
    return 1;
  }

  kbc_Command = kbc_Command | ENABLE_INT_KBD;

  if (sys_outb(KBC_CMD_REG, WRITE_COM_BYTE) != 0)
  {
    return 1;
  }

  if (sys_outb(KBC_IN_BUF, kbc_Command) != 0)
  {
    return 1;
  }

  return 0;
}

void kbd_scancode()
{
  scanBytes[size] = scannedKey;
  size++;
  if (scannedKey != FIRST_BYTE_SCAN)
  {
    kbd_print_scancode((scannedKey & BREAK_CODE) == 0, size, scanBytes);
    size = 0;
  }
}
