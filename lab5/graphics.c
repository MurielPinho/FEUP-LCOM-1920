#include <lcom/lcf.h>
#include <math.h>

#include "i8254.h"
#include "graphics.h"

static uint16_t xRes;
static uint16_t yRes;
static uint8_t bitsPerPixel;
static uint8_t bytesPerPixel;
static uint8_t *video_mem;
static uint8_t redMask;
static uint8_t greenMask;
static uint8_t blueMask;
vbe_mode_info_t vmi;

int vg_set_mode(uint16_t mode)
{

    reg86_t reg;
    memset(&reg, 0, sizeof(reg));
    reg.ah = VBE_CALL;
    reg.al = VBE_SET_MODE;
    reg.bx = VBE_LINEAR_FRAME | mode;
    reg.intno = VBE_INT;
    if (sys_int86(&reg) != OK)
    {
        printf("set_vbe_mode: sys_int86() failed \n", mode);
        return 1;
    }
    return 0;
}

void *(vg_init)(uint16_t mode)
{

    int r;
    struct minix_mem_range mr;
    unsigned int vram_base;
    unsigned int vram_size;

    vbeGetModeInfo(mode, &vmi);

    bitsPerPixel = vmi.BitsPerPixel;
    bytesPerPixel = bitsPerPixel / 8;
    if ((bitsPerPixel % 8 != 0))
    {
        bytesPerPixel++;
    }
    xRes = vmi.XResolution;
    yRes = vmi.YResolution;
    redMask = vmi.RedMaskSize;
    greenMask = vmi.GreenMaskSize;
    blueMask = vmi.BlueMaskSize;
    vram_base = vmi.PhysBasePtr;
    vram_size = xRes * yRes * bytesPerPixel;
    mr.mr_base = (phys_bytes)vmi.PhysBasePtr;
    mr.mr_limit = mr.mr_base + vram_size;

    if (OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)))
        panic("sys_privctl (ADD_MEM) failed: %d\n", r);
    video_mem = vm_map_phys(SELF, (void *)mr.mr_base, vram_size);

    if (video_mem == MAP_FAILED)
        panic("couldn’t map video memory");

    vg_set_mode(mode);

    return video_mem;
}

int vbeGetModeInfo(uint16_t mode, vbe_mode_info_t *vmi_p)
{
    phys_bytes physAddr;
    reg86_t reg;
    mmap_t m;
    memset(&reg, 0, sizeof(reg));
    lm_alloc(sizeof(vbe_mode_info_t), &m);
    physAddr = m.phys;
    reg.ah = VBE_CALL;
    reg.al = VBE_MODE_INFO;
    reg.es = PB2BASE(physAddr);
    reg.di = PB2OFF(physAddr);
    reg.cx = mode;
    reg.intno = VBE_INT;
    if (sys_int86(&reg) != OK)
    {
        return 1;
    }
    *vmi_p = *(vbe_mode_info_t *)(m.virt);
    lm_free(&m);
    return 0;
}

void fillPixel(uint16_t x, uint16_t y, uint32_t color)
{
    if ((x < 0) || (x >= xRes) || (y < 0) || (y >= yRes))
        return;

    uint8_t *memPTR = video_mem;
    uint8_t shift = SIZEOF_UINT32 - bitsPerPixel;
    memPTR += (y * xRes + x) * bytesPerPixel;
    color <<= shift;
    color >>= shift;
    for (uint8_t i = 0; i < bytesPerPixel; i++)
    {
        *memPTR = color >> (SIZEOF_BYTE * i);
        memPTR++;
    }
}

int(vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color)
{
    len += x - 1;
    for (; x <= len; x++)
    {
        fillPixel(x, y, color);
    }

    return 0;
}

int(vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color)
{
    height += y - 1;
    for (; y <= height; y++)
    {
        vg_draw_hline(x, y, width, color);
    }
    return 0;
}

int vg_draw_pattern(uint8_t no_rectangles, uint32_t first, uint8_t step)
{
    uint16_t x, y, width, height;
    uint32_t color;
    x = 0;
    y = 0;
    width = xRes / no_rectangles;
    height = yRes / no_rectangles;

    for (uint8_t row = 0; row < no_rectangles; row++)
    {
        for (uint8_t col = 0; col < no_rectangles; col++)
        {
            x = col * width;
            y = row * height;
            if (bitsPerPixel == 8)
            {
                color = getIndexColor(row, col, no_rectangles, first, step);
            }
            else
            {
                color = getRGB(row, col, first, step);
            }
            //printf("color: %#08X\n",color);
            vg_draw_rectangle(x, y, width, height, color);
        }
    }

    return 0;
}

uint8_t get_LSB(uint32_t val)
{
    uint8_t byte = 0xFF;
    byte = byte & val;
    return byte;
}
uint8_t getIndexColor(uint8_t row, uint8_t col, uint8_t no_rectangles, uint32_t first, uint8_t step)
{
    uint8_t colorByte = get_LSB(first);
    colorByte = (colorByte + (row * no_rectangles + col) * step) % (1 << bitsPerPixel);
    return colorByte;
}
uint8_t getRedComponent(uint8_t row, uint8_t col, uint32_t first, uint8_t step)
{
    uint8_t colorByte = get_LSB(first >> (blueMask + greenMask));
    colorByte = (colorByte + col * step) % (1 << redMask);
    return colorByte;
}
uint8_t getGreenComponent(uint8_t row, uint8_t col, uint32_t first, uint8_t step)
{
    uint8_t colorByte = get_LSB(first >> blueMask);
    colorByte = (colorByte + row * step) % (1 << greenMask);
    return colorByte;
}
uint8_t getBlueComponent(uint8_t row, uint8_t col, uint32_t first, uint8_t step)
{
    uint8_t colorByte = get_LSB(first);
    colorByte = (colorByte + (col + row) * step) % (1 << blueMask);
    return colorByte;
}
uint32_t getRGB(uint8_t row, uint8_t col, uint32_t first, uint8_t step)
{
    uint32_t color;
    uint32_t r, g, b;
    r = (getRedComponent(row, col, first, step) << (blueMask + greenMask));
    g = (getGreenComponent(row, col, first, step) << blueMask);
    b = getBlueComponent(row, col, first, step);
    color = r | g | b;
    return color;
}
