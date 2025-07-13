#pragma once

#include "dv_font.h"
#include "dv_fb.h"

#define GLYPH_ARRAY_SIZE 256

typedef struct {
  dv_fb_t *glyphs[GLYPH_ARRAY_SIZE];
  uint32_t head_kerns[GLYPH_ARRAY_SIZE];
  uint32_t tail_kerns[GLYPH_ARRAY_SIZE];
} font_t;

font_t *font_create(const char *image_fn, uint32_t fg_color, uint32_t bg_color);

void font_delete(font_t *font);

void font_draw_string(font_t *font, const char *string, uint32_t x, uint32_t y, dv_fb_t *target);

void font_draw_partial_string(font_t *font, const char *string, uint32_t len, uint32_t x, uint32_t y, dv_fb_t *target);

uint32_t font_get_width(font_t *font, const char *string);

uint32_t font_get_height(font_t *font);

uint32_t font_wrap_string(font_t *font, const char *string, uint32_t x, uint32_t y, uint32_t w, dv_fb_t *target);

int32_t font_wrap_partial_string(font_t *font, const char *string, uint32_t len, uint32_t x, uint32_t y, uint32_t w, dv_fb_t *target);
void font_draw_all_glyphs(font_t *font, uint32_t x, uint32_t y, dv_fb_t *target);

