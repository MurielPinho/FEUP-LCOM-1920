#include <lcom/lcf.h>
#include <lcom/xpm.h>

#include "sprite.h"
#include "graphics.h"

#include <stdint.h>
#include <stdio.h>

/** Creates a new sprite with pixmap "pic", with specified
* position (within the screen limits) and speed;
* Does not draw the sprite on the screen
* Returns NULL on invalid pixmap.
*/
Sprite *create_sprite(xpm_map_t pic, int x, int y, int xspeed, int yspeed)
{
  Sprite *sp = (Sprite *)malloc(sizeof(Sprite));
  xpm_image_t img;
  if (sp == NULL)
    return NULL;
  sp->map = xpm_load(pic, XPM_INDEXED, &img);
  sp->x = x;
  sp->y = y;
  sp->height = img.height;
  sp->width = img.width;
  sp->xspeed = xspeed;
  sp->yspeed = yspeed;

  if (sp->map == NULL)
  {
    free(sp);
    return NULL;
  }

  return sp;
}

int draw_sprite(Sprite *sp)
{
  uint8_t *colorPTR;
  uint16_t x, y, xf, yf;

  colorPTR = sp->map;
  x = sp->x;
  y = sp->y;
  xf = sp->x + sp->width;
  yf = sp->y + sp->height;
  for (; y < yf; y++)
  {
    for (; x < xf; x++)
    {
      fillPixel(x, y, *(colorPTR));
      colorPTR++;
    }
    x -= sp->width;
  }
  return 0;
}
void destroy_sprite(Sprite *sp)
{
  clear_sprite(sp);
  if (sp == NULL)
    return;

  if (sp->map)
    free(sp->map);

  free(sp);
  sp = NULL;
}
void clear_sprite(Sprite *sp)
{
  uint16_t x, y, xf, yf;
  x = sp->x;
  y = sp->y;
  xf = sp->x + sp->width;
  yf = sp->y + sp->height;
  for (; y < yf; y++)
  {
    for (; x < xf; x++)
    {
      fillPixel(x, y, 0x0);
    }
    x -= sp->width;
  }
}
void set_sprite_speed(Sprite *sp, uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf, int16_t speed)
{
  if (xi == xf && yi != yf)
  {
    if (yf > yi)
    {
      sp->yspeed = speed;
    }
    else
    {
      sp->yspeed = -speed;
    }
    sp->xspeed = 0;
  }
  else if (yi == yf && xi != xf)
  {
    if (xf > xi)
    {
      sp->xspeed = speed;
    }
    else
    {
      sp->xspeed = -speed;
    }
    sp->yspeed = 0;
  }
  else
  {
    sp->xspeed = 0;
    sp->yspeed = 0;
  }
}

void move_sprite(Sprite *sprite, uint8_t fr_rate, uint16_t xf, uint16_t yf, int16_t speed)
{
  static uint32_t Frames = 0;
  int intPerFrame = 60 / fr_rate;
  if (interruptCounter % intPerFrame == 0)
  {
    Frames++;
    if (sprite->x != xf || sprite->y != yf)
    {
      clear_sprite(sprite);
      if (speed >= 0)
      {
        sprite->x += sprite->xspeed;
        sprite->y += sprite->yspeed;
      }
      else if (speed < 0 && (Frames % abs(speed) == 0))
      {

        if (sprite->xspeed)
        {
          sprite->x++;
        }
        else if (sprite->yspeed)
        {
          sprite->y++;
        }
      }
      if (sprite->x > xf || sprite->y > yf)
      {
        sprite->x = xf;
        sprite->y = yf;
      }
      draw_sprite(sprite);
    }
  }
}

int animate_sprite(Sprite *UNUSED(sp))
{
  //...

  printf("function not implemented");
  return 1;
}

/* Some useful non-visible functions */
//static int draw_sprite(Sprite *sp, char *base) {
//...
//}
//static int check_collision(Sprite *sp, char *base) {
//...
//}
