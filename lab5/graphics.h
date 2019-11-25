void *(vg_init)(uint16_t mode);
int vbeGetModeInfo(uint16_t mode, vbe_mode_info_t *vmi_p);
int vg_set_mode(uint16_t mode);
void fillPixel(uint16_t x, uint16_t y, uint32_t color);
int(vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color);
int(vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color);
int vg_draw_pattern(uint8_t no_rectangles, uint32_t first, uint8_t step);
uint32_t getRGB(uint8_t row, uint8_t col, uint32_t first, uint8_t step);
uint8_t getBlueComponent(uint8_t row, uint8_t col, uint32_t first, uint8_t step);
uint8_t getGreenComponent(uint8_t row, uint8_t col, uint32_t first, uint8_t step);
uint8_t getRedComponent(uint8_t row, uint8_t col, uint32_t first, uint8_t step);
uint8_t getIndexColor(uint8_t row, uint8_t col, uint8_t no_rectangles, uint32_t first, uint8_t step);
uint8_t get_LSB(uint32_t val);

#define VBE_CALL 0x4F
#define VBE_CONTROL_INFO 0x00
#define VBE_MODE_INFO 0x01
#define VBE_SET_MODE 0x02
#define VBE_INT 0x10
#define VBE_LINEAR_FRAME BIT(14)
#define INDEXED_MODE 0x105
#define SIZEOF_BYTE 8
#define SIZEOF_UINT32 8 * sizeof(uint32_t)
