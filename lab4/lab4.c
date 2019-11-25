// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <stdint.h>
#include <stdio.h>

#include "mouse.h"
#include "statemachine.h"
#include "i8042.h"
#include "i8254.h"

// Any header files included below this line should have been created by you

int main(int argc, char *argv[])
{
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need/ it]
  lcf_trace_calls("/home/lcom/labs/lab4/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab4/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(mouse_test_packet)(uint32_t cnt)
{

  uint8_t mouse_bit = MOUSE_IRQ_PIC2;
  int rec, ipcStatus;
  message msg;
  packetCounter = cnt;

  if (mouse_send_command(ENABLE_DATA_REPORT) != 0)
  {
    return 1;
  }
  if (mouse_subscribe_int(&mouse_bit) != 0)
  {
    return 1;
  }
  while (packetCounter > 0)
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
        if (msg.m_notify.interrupts & mouse_bit)
        {
          mouse_ih();
          if (mouse_evaluate_bytes())
          {
            initialize_packet(&pp, mouseBytes);
            mouse_print_packet(&pp);
          }
        }
        break;
      default:
        break;
      }
    }
  }
  if (mouse_unsubscribe_int() != 0)
  {
    return 1;
  }
  if (mouse_send_command(DISABLE_DATA_REPORT) != 0)
  {
    return 1;
  }

  return 0;
}

int(mouse_test_remote)(uint16_t period, uint8_t cnt)
{
  packetCounter = cnt;
  while (packetCounter > 0)
  {
    if (packetIndex == 0)
    {
      if (mouse_send_command(READ_DATA) != 0)
      {
        continue;
      }
    }
    mouse_read_byte();
    if (mouse_evaluate_bytes())
    {
      initialize_packet(&pp, mouseBytes);
      mouse_print_packet(&pp);
      tickdelay(micros_to_ticks(period * 1000));
    }
  }

  if (mouse_send_command(STREAM_MODE) != 0)
  {
    return 1;
  }
  if (mouse_send_command(DISABLE_DATA_REPORT) != 0)
  {
    return 1;
  }

  while (mouse_write_dflt_kbc_cmd_byte())
    ;

  return 0;
}

int(mouse_test_async)(uint8_t idle_time)
{
  int rec, ipcStatus;
  message msg;
  uint8_t timer_freq = sys_hz();
  uint8_t mouse_bit = MOUSE_IRQ_PIC2;
  uint8_t timer_bit = TIMER0_IRQ;
  packetIndex = 0;
  interruptCounter = 0;

  if (mouse_send_command(STREAM_MODE) != 0)
  {
    return 1;
  }

  if (mouse_send_command(ENABLE_DATA_REPORT) != 0)
  {
    return 1;
  }
  if (mouse_subscribe_int(&mouse_bit) || timer_subscribe_int(&timer_bit))
  {
    return 1;
  }

  while ((interruptCounter / timer_freq) < idle_time)
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
        if (msg.m_notify.interrupts & mouse_bit)
        {
          mouse_ih();
          if (mouse_evaluate_bytes())
          {
            initialize_packet(&pp, mouseBytes);
            mouse_print_packet(&pp);
          }
          interruptCounter = 0;
        }
        else if (msg.m_notify.interrupts & timer_bit)
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
  if (mouse_unsubscribe_int() || timer_unsubscribe_int())
  {
    return 1;
  }
  if (mouse_send_command(REMOTE_MODE) != 0)
  {
    return 1;
  }

  if (mouse_send_command(DISABLE_DATA_REPORT) != 0)
  {
    return 1;
  }

  return 0;
}

int(mouse_test_gesture)(uint8_t x_len, uint8_t tolerance)
{
  uint8_t mouse_bit = MOUSE_IRQ_PIC2;
  int rec, ipcStatus;
  message msg;

  if (mouse_send_command(ENABLE_DATA_REPORT) != 0)
  {
    return 1;
  }
  if (mouse_subscribe_int(&mouse_bit) != 0)
  {
    return 1;
  }
  while (get_mouse_state() != FINAL)
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
        if (msg.m_notify.interrupts & mouse_bit)
        {
          mouse_ih();
          mouse_gesture(x_len,tolerance);
        }
        break;
      default:
        break;
      }
    }
  }
  if (mouse_unsubscribe_int() != 0)
  {
    return 1;
  }
  if (mouse_send_command(DISABLE_DATA_REPORT) != 0)
  {
    return 1;
  }

  return 0;
}
