// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <lcom/lab5.h>
#include "graphics.h"
#include "keyboard.h"
#include "sprite.h"
#include "i8042.h"
#include "i8254.h"

#include <stdint.h>
#include <stdio.h>

// Any header files included below this line should have been created by you

int main(int argc, char *argv[])
{
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab5/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab5/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(video_test_init)(uint16_t mode, uint8_t delay)
{

  if (vg_set_mode(mode))
  {
    return 1;
  }

  sleep(delay);

  if (vg_exit())
  {
    return 1;
  }
  return 0;
}

int(video_test_rectangle)(uint16_t mode, uint16_t x, uint16_t y,
                          uint16_t width, uint16_t height, uint32_t color)
{
  uint8_t kbd_bit = KEYBOARD_IRQ;
  int ipc_status, rec;
  message msg;

  if (vg_init(mode) == NULL)
    return 1;

  vg_draw_rectangle(x, y, width, height, color);

  if (kbd_subscribe_int(&kbd_bit))
  {
    return 1;
  }

  while (scannedKey != ESC)
  {
    if ((rec = driver_receive(ANY, &msg, &ipc_status)) != 0)
    {
      printf("driver_receive failed with: %d", rec);
      continue;
    }
    if (is_ipc_notify(ipc_status))
    {
      switch (_ENDPOINT_P(msg.m_source))
      {
      case HARDWARE:
        if (msg.m_notify.interrupts & kbd_bit)
        {
          kbc_ih();
        }
        break;
      default:
        break;
      }
    }
  }

  if (kbd_unsubscribe_int())
  {
    return 1;
  }

  if (vg_exit())
    return 1;

  return 0;
}

int(video_test_pattern)(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step)
{
  uint8_t kbd_bit = KEYBOARD_IRQ;
  int ipc_status, rec;
  message msg;

  if (vg_init(mode) == NULL)
    return 1;
  vg_draw_pattern(no_rectangles, first, step);

  if (kbd_subscribe_int(&kbd_bit))
  {
    return 1;
  }

  while (scannedKey != ESC)
  {
    if ((rec = driver_receive(ANY, &msg, &ipc_status)) != 0)
    {
      printf("driver_receive failed with: %d", rec);
      continue;
    }
    if (is_ipc_notify(ipc_status))
    {
      switch (_ENDPOINT_P(msg.m_source))
      {
      case HARDWARE:
        if (msg.m_notify.interrupts & kbd_bit)
        {
          kbc_ih();
        }
        break;
      default:
        break;
      }
    }
  }

  if (kbd_unsubscribe_int())
  {
    return 1;
  }

  if (vg_exit())
    return 1;

  return 0;
}

int(video_test_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y)
{
  uint8_t kbd_bit = KEYBOARD_IRQ;
  int ipc_status, rec;
  message msg;
  Sprite *sprite;

  if (kbd_subscribe_int(&kbd_bit))
  {
    return 1;
  }

  if (vg_init(INDEXED_MODE) == NULL)
    return 1;

  sprite = create_sprite(xpm, x, y, 0, 0);
  draw_sprite(sprite);

  while (scannedKey != ESC)
  {
    if ((rec = driver_receive(ANY, &msg, &ipc_status)) != 0)
    {
      printf("driver_receive failed with: %d", rec);
      continue;
    }
    if (is_ipc_notify(ipc_status))
    {
      switch (_ENDPOINT_P(msg.m_source))
      {
      case HARDWARE:
        if (msg.m_notify.interrupts & kbd_bit)
        {
          kbc_ih();
        }
        break;
      default:
        break;
      }
    }
  }

  if (kbd_unsubscribe_int())
  {
    return 1;
  }

  if (vg_exit())
    return 1;

  return 0;
}

int(video_test_move)(xpm_map_t xpm, uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf, int16_t speed, uint8_t fr_rate)
{

  int ipc_status, rec;
  message msg;
  uint8_t timerBit = TIMER0_IRQ;
  uint8_t keyboardBit = KEYBOARD_IRQ;
  Sprite *sprite;

  if (timer_subscribe_int(&timerBit) || kbd_subscribe_int(&keyboardBit))
    return 1;

  if (vg_init(INDEXED_MODE) == NULL)
    return 1;

  sprite = create_sprite(xpm, xi, yi, 0, 0);
  sprite = create_sprite(xpm, xi, yi, 0, 0);
  set_sprite_speed(sprite, xi, yi, xf, yf, speed);
  draw_sprite(sprite);
  draw_sprite(sprite);
  scannedKey = 0;
  while (scannedKey != ESC)
  {
    if ((rec = driver_receive(ANY, &msg, &ipc_status)) != 0)
    {
      printf("driver_receive failed with: %d", rec);
      continue;
    }
    if (is_ipc_notify(ipc_status))
    {
      switch (_ENDPOINT_P(msg.m_source))
      {
      case HARDWARE:
        if (msg.m_notify.interrupts & keyboardBit)
        {
          kbc_ih();
        }
        else if (msg.m_notify.interrupts & timerBit)
        {
          timer_int_handler();
          move_sprite(sprite, fr_rate, xf, yf, speed);
        }
        break;
      default:
        break;
      }
    }
  }
  destroy_sprite(sprite);
  sprite = NULL;
  vg_exit();
  if (timer_unsubscribe_int() || kbd_unsubscribe_int())
  {
    return 1;
  }
  return 0;
}

int(video_test_controller)()
{
  /* To be completed */
  printf("%s(): under construction\n", __func__);

  return 1;
}
