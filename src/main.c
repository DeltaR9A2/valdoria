
#include "dv_glfw.h"
#include "dv_font.h"

const char test_string[] = "If you want your cache to store only one copy per unique string, you must compare the string contents, not the pointers. Thankfully, stb_ds.h does exactly that - if the key is a char *, it automatically uses strcmp() for comparisons and stbds_hash_string() for hashing.";

void input_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
    (void)mods;
    (void)window;
    (void)scancode;
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS){
        dv_glfw_stop_game();
        printf("%s\n","test");
    }
}

int main(void)
{
    dv_fb_t *screen = dv_glfw_init();

    dv_glfw_add_keyfun(&input_callback);

    dv_fb_t *cursor = dv_get_image("cursor-arrow.png");

    font_t *title_font = font_create("font-alkhemikal-16.png", RGBA(0xFF,0xBF,0x00,0xFF), RGBA(0x3B,0x1F,0x00,0xFF));
    font_t *prose_font = font_create("font-piacevoli-10.png",  RGBA(0xDD,0xDD,0xDD,0xFF), RGBA(0x22,0x22,0x22,0xFF));
    font_t *small_font = font_create("font-microserif-7.png",  RGBA(0xFF,0xFF,0xFF,0xFF), RGBA(0x22,0x22,0x22,0xFF));
    
    font_draw_string(title_font, "Welcome to Larkin Hollow", 20, 40, screen);
    font_draw_string(prose_font, test_string, 20, 60, screen);
    font_wrap_string(prose_font, test_string, 20, 80, 250, screen);
    font_wrap_string(small_font, test_string, 290, 80, 250, screen);

    dv_fb_blit(screen, 20, 20, cursor);

    while (dv_glfw_keep_going())
    {
        dv_glfw_draw_window();
    }

    dv_glfw_exit();
    return 0;
}

