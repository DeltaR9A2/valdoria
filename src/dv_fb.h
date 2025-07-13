#pragma once

#include <stdint.h>

#define RGBA(r,g,b,a) ((uint32_t)( ((a) << 24) | ((b) << 16) | ((g) << 8) | (r) ))

typedef struct{
    uint32_t w, h;
    uint32_t *pixels;
} dv_fb_t;

dv_fb_t *dv_fb_create(uint32_t w, uint32_t h);
dv_fb_t *dv_get_image(const char *fn);

void dv_fb_blit(dv_fb_t *dest, uint32_t dx, uint32_t dy, dv_fb_t *src);
void dv_fb_blit_part(dv_fb_t* dest, uint32_t dx, uint32_t dy, dv_fb_t* src,  uint32_t sx, uint32_t sy, uint32_t bw, uint32_t bh );
void dv_fb_blit_blend(dv_fb_t *dest, uint32_t dx, uint32_t dy, dv_fb_t *src);

