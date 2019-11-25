#include <lcom/lcf.h>

typedef struct
{
    uint16_t x, y;          // current position
    uint16_t width, height; // dimensions
    int xspeed, yspeed;     // current speed
    uint8_t *map;           // the pixmap
} Sprite;

extern uint32_t interruptCounter;
/** Creates a new sprite with pixmap "pic", with specified
* position (within the screen limits) and speed;
* Does not draw the sprite on the screen
* Returns NULL on invalid pixmap.
*/
Sprite *create_sprite(xpm_map_t pic, int x, int y, int xspeed, int yspeed);
void destroy_sprite(Sprite *sp);
int animate_sprite(Sprite *sp);
int draw_sprite(Sprite *sp);
void clear_sprite(Sprite *sp);
void set_sprite_speed(Sprite *sp, uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf, int16_t speed);
void move_sprite(Sprite *sprite, uint8_t fr_rate, uint16_t xf, uint16_t yf, int16_t speed);


