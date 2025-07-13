#include "dv_font.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define GLYPH_ARRAY_SIZE 256
#define STRING_BUFFER_SIZE 2048

//const char *glyph_order = " 1234567890-=`!@#$%^&*()_+~abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ[]\\;',./{}|:\"<>?";

const char glyph_order[] = {
  ' ', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', 
  'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 
  'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 
  '-', '=', '`', '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '~', 
  '[', ']','\\', ';','\'', ',', '.', '/', '{', '}', '|', ':', '"', '<', '>', '?','\0',
};


font_t *font_create(const char *image_fn, uint32_t fg_color, uint32_t bg_color){
  font_t *font = calloc(1,sizeof(font_t));
  memset(font, 0, sizeof(font_t));

  dv_fb_t *font_img = dv_get_image(image_fn);
  uint32_t *pixels = font_img->pixels;

  for(size_t i=0; i<(font_img->w*font_img->h); i+=1) { 
    if(pixels[i] == RGBA(255,255,255,255)){ pixels[i] = fg_color; } 
    if(pixels[i] == RGBA(  0,  0,  0,255)){ pixels[i] = bg_color; } 
  }

  uint32_t this_mark = 0;
  uint32_t prev_mark = 0;

  uint32_t mark_color = pixels[0];
  uint32_t kern_color = pixels[1];

  uint32_t glyph_index = 0;
  dv_fb_t *glyph_surface;
  uint32_t grx, gry, grw, grh;
  
  uint32_t glyph_count = strlen(glyph_order);
  uint32_t kern_mark = 0;
  uint32_t kern_counter = 0;

  while(glyph_index < glyph_count){
    uint8_t ascii_code = (int)glyph_order[glyph_index];

    // Seek to next glyph
    while(pixels[this_mark] == mark_color){
      if(this_mark > font_img->w){ break; }
      this_mark += 1;
    }

    // Measure Head Kern
    kern_mark = this_mark;
    kern_counter = 0;
    while(pixels[kern_mark] == kern_color){
      kern_mark += 1;
      kern_counter += 1;
    }
    font->head_kerns[ascii_code] = kern_counter;

    // Measure Glyph
    prev_mark = this_mark;
    while(pixels[this_mark] != mark_color){
      if(this_mark > font_img->w){ break; }
      this_mark += 1;
    }

    // Measure Tail Kern
    kern_mark = this_mark - 1;
    kern_counter = 0;
    while(pixels[kern_mark] == kern_color){
      kern_mark -= 1;
      kern_counter += 1;
    }
    font->tail_kerns[ascii_code] = kern_counter;

    // Check if we have run out of glyphs early.
    if(this_mark > font_img->w){
      fprintf(stderr, "Warning: font_init: Font source shorter than glyph list.\n");
      break;
    }

    // Record glyph to its own surface
    grx = prev_mark;
    grw = this_mark - prev_mark;
    gry = 1;
    grh = font_img->h-1;

    glyph_surface = dv_fb_create(grw, grh);
    dv_fb_blit_part(glyph_surface, 0, 0, font_img, grx, gry, grw, grh);

    font->glyphs[(int)glyph_order[glyph_index]] = glyph_surface;

    glyph_index += 1;
  }

  return font;
}

void font_draw_string(font_t *font, const char *string, uint32_t x, uint32_t y, dv_fb_t *target){
  if(string == NULL){ return; }

  uint32_t tx = x;
  uint32_t ty = y;

  for(uint32_t i=0; i<strlen(string); i++){
    uint8_t ascii_code = (int)string[i];

    if(font->glyphs[ascii_code] != NULL){
      tx -= font->head_kerns[ascii_code];
      dv_fb_blit_blend(target, tx, ty, font->glyphs[ascii_code]);
      tx += font->glyphs[ascii_code]->w;
      tx -= font->tail_kerns[ascii_code];
      tx += 1;
    }
  }
}

void font_draw_all_glyphs(font_t *font, uint32_t x, uint32_t y, dv_fb_t *target){
  font_draw_string(font, glyph_order, x, y, target);
}


void font_draw_partial_string(font_t *font, const char *string, uint32_t len, uint32_t x, uint32_t y, dv_fb_t *target){
  if(string == NULL){ return; }
  char temp[STRING_BUFFER_SIZE];

  if(len >= strlen(string)){
    font_draw_string(font, string, x, y, target);
  }else{
    strncpy(temp, string, len);
    temp[len] = '\0';
    font_draw_string(font, temp, x, y, target);
  }
}

uint32_t font_get_width(font_t *font, const char *string){
  if(string == NULL){ return 0; }
  int32_t w = 0;
  for(uint32_t i=0; i<strlen(string); i++){
    uint8_t ascii_code = (int)string[i];
    if(font->glyphs[ascii_code] != NULL){
      w -= font->head_kerns[ascii_code];
      w += font->glyphs[ascii_code]->w;
      w -= font->tail_kerns[ascii_code];
      w += 1;
    }
  }
  return w;
}

uint32_t font_get_height(font_t *font){
  return font->glyphs[(int)glyph_order[0]]->h;
}

uint32_t font_wrap_string(font_t *font, const char *string, uint32_t x, uint32_t y, uint32_t w, dv_fb_t *target){
  if(string == NULL){ return 0; }
  char temp[STRING_BUFFER_SIZE];
  uint32_t h = font_get_height(font);
  uint32_t line_start = 0;
  uint32_t line_end = 0;
  bool wrap_now = false;

  uint32_t total_height = 0;

  while(line_end <= strlen(string)){
    wrap_now = false;
    strncpy(temp, &(string[line_start]), line_end-line_start);
    temp[line_end-line_start] = '\0';

    if(string[line_end] == '\n'){
      wrap_now = true;
    }else if(font_get_width(font, temp) > w){
      int32_t temp_end = line_end;
      while(string[line_end] != ' '){
        line_end -= 1;
        if(line_end == 0){
          line_end = temp_end;
          break;
        }
      }
      wrap_now = true;
    }else if(line_end == strlen(string)){
      wrap_now = true;
    }

    if(wrap_now){
      font_draw_partial_string(font, &(string[line_start]), line_end-line_start, x, y, target);
      line_start = line_end;
      if(string[line_start] == ' '){ line_start += 1; }
      line_end = line_start + 1;
      y += h;
      total_height += h;
    }else{
      line_end += 1;
    }
  }
  return total_height;
}

int32_t font_wrap_partial_string(font_t *font, const char *string, uint32_t len, uint32_t x, uint32_t y, uint32_t w, dv_fb_t *target){
  if(string == NULL){ return 0; }
  char temp[STRING_BUFFER_SIZE];

  if(len >= strlen(string)){
    return font_wrap_string(font, string, x, y, w, target);
  }else{
    strncpy(temp, string, len);
    temp[len] = '\0';
    return font_wrap_string(font, temp, x, y, w, target);
  }
}

