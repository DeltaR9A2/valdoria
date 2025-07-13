#include "dv_fb.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#include "stb_image.h"

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

dv_fb_t *dv_fb_create(uint32_t w, uint32_t h){
    dv_fb_t *fb = calloc( 1, sizeof( dv_fb_t ) );
    fb->w = w; fb->h = h;
    fb->pixels = calloc( w * h, sizeof(uint32_t) );
    return fb;
}

dv_fb_t *dv_get_image(const char *fn){
  static struct { char *key; dv_fb_t *value; } *image_cache = NULL;
  dv_fb_t *image = shget(image_cache, fn);
  if(image == NULL){
    int w, h, n;
    unsigned char *data = stbi_load(fn, &w, &h, &n, 4);
    if(data == NULL){ return NULL; }
    image = dv_fb_create(w,h);
    memcpy(image->pixels,data,w*h*sizeof(uint32_t));
    stbi_image_free(data);
    shput(image_cache, fn, image);
  }
  return image;
}

void dv_fb_blit(dv_fb_t *dest, uint32_t dx, uint32_t dy, dv_fb_t *src){
    for (uint32_t y = 0; y < src->h; ++y) {
        if (y >= src->h || y + dy >= dest->h){ continue; }
        for (uint32_t x = 0; x < src->w; ++x) {
            if (x >= src->w || x + dx >= dest->w){ continue; }
            dest->pixels[((y+dy)*dest->w)+(x+dx)] = src->pixels[(y*src->w)+x];
        }
    }
}

static inline uint32_t blend(uint32_t src, uint32_t dest) {
    uint8_t src_a = (src >> 24) & 0xFF;
    if (src_a == 255) return src;
    if (src_a == 0)   return dest;

    uint8_t src_r = (src >> 16) & 0xFF;
    uint8_t src_g = (src >> 8)  & 0xFF;
    uint8_t src_b =  src        & 0xFF;

    uint8_t dest_r = (dest >> 16) & 0xFF;
    uint8_t dest_g = (dest >> 8)  & 0xFF;
    uint8_t dest_b =  dest        & 0xFF;

    float alpha = src_a / 255.0f;

    uint8_t out_r = (uint8_t)(src_r * alpha + dest_r * (1.0f - alpha));
    uint8_t out_g = (uint8_t)(src_g * alpha + dest_g * (1.0f - alpha));
    uint8_t out_b = (uint8_t)(src_b * alpha + dest_b * (1.0f - alpha));

    return (0xFF << 24) | (out_r << 16) | (out_g << 8) | out_b;
}

void dv_fb_blit_part(
    dv_fb_t* dest, uint32_t dx, uint32_t dy,
    dv_fb_t* src,  uint32_t sx, uint32_t sy,
                   uint32_t bw, uint32_t bh ) {
    for (uint32_t y = 0; y < bh; ++y) {
        uint32_t src_row = sy + y;
        uint32_t dest_row = dy + y;
        if (src_row >= src->h || dest_row >= dest->h){ continue; }

        for (uint32_t x = 0; x < bw; ++x) {
            uint32_t src_col = sx + x;
            uint32_t dest_col = dx + x;
            if (src_col >= src->w || dest_col >= dest->w){ continue; }

            uint32_t s = src->pixels[src_row * src->w + src_col];
            uint32_t d = dest->pixels[dest_row * dest->w + dest_col];
            dest->pixels[dest_row * dest->w + dest_col] = blend(s, d);
        }
    }
}


void dv_fb_blit_blend(dv_fb_t *dest, uint32_t dx, uint32_t dy, dv_fb_t *src){
    for (uint32_t y = 0; y < src->h; ++y) {
        if (y >= src->h || y + dy >= dest->h){ continue; }
        for (uint32_t x = 0; x < src->w; ++x) {
            if (x >= src->w || x + dx >= dest->w){ continue; }
            dest->pixels[((y+dy)*dest->w)+(x+dx)] = blend(src->pixels[(y*src->w)+x],dest->pixels[((y+dy)*dest->w)+(x+dx)]);
        }
    }
}


