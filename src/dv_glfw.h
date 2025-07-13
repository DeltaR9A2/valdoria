#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "glad/gl.h"
#include <GLFW/glfw3.h>
#include "dv_fb.h"

#define VIRTUAL_SCREEN_W 640
#define VIRTUAL_SCREEN_H 360

dv_fb_t *dv_glfw_init(void);
void     dv_glfw_exit(void);

void dv_glfw_draw_window();
bool dv_glfw_keep_going();

void dv_glfw_stop_game();

void dv_glfw_add_keyfun(GLFWkeyfun cb);
