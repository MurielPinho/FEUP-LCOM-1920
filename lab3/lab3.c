#include <lcom/lcf.h>
#include <lcom/lab3.h>
#include <stdint.h>

#include "keyboard.h"
#include "i8042.h"
#include "i8254.h"

extern uint8_t scannedKey;
extern uint32_t sysInbCounter;
extern uint32_t interruptCounter;
extern uint8_t size;
extern uint8_t scanBytes[2];
extern int hookID;

int main(int argc, char *argv[])
{
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab3/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab3/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(kbd_test_scan)()
{
  uint8_t kbd_bit = KEYBOARD_IRQ;
  int rec, ipcStatus;
  message msg;
  size = 0;
  sysInbCounter = 0;

  if (kbd_subscribe_int(&kbd_bit) != 0)
  {
    return 1;
  }

  while (scannedKey != ESC)
  {
    if ((rec = driver_receive(ANY, &msg, &ipcStatus)) != 0)
    {
      printf("driver_receive failed with: %d", rec);
      continue;
    }
    if (is_ipc_notify(ipcStatus))
    {
      switch (_ENDPOINT_P(msg.m_source))
      {
      case HARDWARE:
        if (msg.m_notify.interrupts & kbd_bit)
        {
          kbc_ih();
          kbd_scancode();
        }
        break;
      default:
        break;
      }
    }
  }

  kbd_print_no_sysinb(sysInbCounter);

  if (kbd_unsubscribe_int() != 0)
  {
    return 1;
  }
  return 0;
}

int(kbd_test_poll)()
{
  sysInbCounter = 0;
  size = 0;
  while (scannedKey != ESC)
  {
    while (kbc_read_keycode())
    {
      tickdelay(micros_to_ticks(POLL_DELAY));
    }
    kbd_scancode();
  }

  kbc_interrupts_enable();

  kbd_print_no_sysinb(sysInbCounter);

  return 0;
}

int(kbd_test_timed_scan)(uint8_t n)
{

  int ipcStatus, rec;
  message msg;
  uint8_t timerBit = TIMER0_IRQ;
  uint8_t keyboardBit = KEYBOARD_IRQ;
  size = 0;
  if (n < 0)
    return 1;

  if (timer_subscribe_int(&timerBit) || kbd_subscribe_int(&keyboardBit))
    return 1;

  while (scannedKey != ESC && interruptCounter / 60 < n)
  {
    if ((rec = driver_receive(ANY, &msg, &ipcStatus)) != 0)
    {
      printf("driver_receive failed with: %d", rec);
      continue;
    }
    if (is_ipc_notify(ipcStatus))
    {
      switch (_ENDPOINT_P(msg.m_source))
      {
      case HARDWARE:
        if (msg.m_notify.interrupts & keyboardBit)
        {
          kbc_ih();
          interruptCounter = 0;
          kbd_scancode();
        }
        else if (msg.m_notify.interrupts & timerBit)
        {

          timer_int_handler();

          continue;
        }
        break;
      default:
        break;
      }
    }
  }
  if (timer_unsubscribe_int() || kbd_unsubscribe_int())
  {
    return 1;
  }

  kbd_print_no_sysinb(sysInbCounter);
  return 0;
}
