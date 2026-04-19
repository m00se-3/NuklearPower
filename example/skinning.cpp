/* nuklear - v1.05 - public domain */
#include <utils.hpp>

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#include "nk/nuklear.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

struct media {
    GLint skin;
    nk::image menu;
    nk::image check;
    nk::image check_cursor;
    nk::image option;
    nk::image option_cursor;
    nk::image header;
    nk::image window;
    nk::image scrollbar_inc_button;
    nk::image scrollbar_inc_button_hover;
    nk::image scrollbar_dec_button;
    nk::image scrollbar_dec_button_hover;
    nk::image button;
    nk::image button_hover;
    nk::image button_active;
    nk::image tab_minimize;
    nk::image tab_maximize;
    nk::image slider;
    nk::image slider_hover;
    nk::image slider_active;
};

static void
die(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    fputs("\n", stderr);
    exit(EXIT_FAILURE);
}

static GLuint
image_load(const char *filename)
{
  const auto high_bit_set = [](const unsigned int val) { return (val & 0x80000000) > 0; };

  unsigned int x,y,n;
  GLuint tex;
  unsigned char *data = stbi_load(filename, &x, &y, &n, 0);
  if (!data || high_bit_set(x) || high_bit_set(y)) {
    die("failed to load image: %s", filename);
  }

  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_2D, tex);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, static_cast<GLint>(x), static_cast<GLint>(y), 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);
  stbi_image_free(data);
  return tex;
}

/* glfw callbacks (I don't know if there is a easier way to access text and scroll )*/
static void error_callback(int e, const char *d){printf("Error %d: %s\n", e, d);}
static void text_input(GLFWwindow *win, unsigned int codepoint)
{nk::input_unicode((struct nk::context*)glfwGetWindowUserPointer(win), codepoint);}
static void scroll_input(GLFWwindow *win, double _, double yoff)
{nk::input_scroll((struct nk::context*)glfwGetWindowUserPointer(win), nk::vec2_from_floats(0, (float)yoff));}

int main(int argc, char *argv[])
{
    /* Platform */
    static GLFWwindow *win;
    int width = 0, height = 0;
    int display_width=0, display_height=0;

    /* GUI */
    nk::font_atlas atlas{};
    media media{};
    nk::context ctx{};
    nk::font *font{};

    /* GLFW */
    glfwSetErrorCallback(error_callback);
    if (!glfwInit()) {
        fprintf(stdout, "[GFLW] failed to init!\n");
        exit(1);
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    win = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Demo", NULL, NULL);
    glfwMakeContextCurrent(win);
    glfwSetWindowUserPointer(win, &ctx);
    glfwSetCharCallback(win, text_input);
    glfwSetScrollCallback(win, scroll_input);
    glfwGetWindowSize(win, &width, &height);
    glfwGetFramebufferSize(win, &display_width, &display_height);

    /* OpenGL */
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glewExperimental = 1;
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to setup GLEW\n");
        exit(1);
    }

    device device;

    /* GUI */
    {
    {const void *image; unsigned int w, h;
    const char *font_path = (argc > 1) ? argv[1]: 0;
    nk::font_atlas_init_default(&atlas);
    nk::font_atlas_begin(&atlas);
    if (font_path) font = nk::font_atlas_add_from_file(&atlas, font_path, 13.0f, NULL);
    else font = nk::font_atlas_add_default(&atlas, 13.0f, NULL);
    image = nk::font_atlas_bake(&atlas, &w, &h, nk::NK_FONT_ATLAS_RGBA32);
    device.upload_atlas(image, w, h);
    nk::font_atlas_end(&atlas, nk::handle_id((int)device.font_tex), &device.tex_null);}
    nk::init_default(&ctx, &font->handle);}

    {   /* skin */
        glEnable(GL_TEXTURE_2D);
        media.skin = static_cast<int>(image_load("../skins/gwen.png"));
        media.check = nk::subimage_id(media.skin, 512,512, nk::rect(464,32,15,15));
        media.check_cursor = nk::subimage_id(media.skin, 512,512, nk::rect(450,34,11,11));
        media.option = nk::subimage_id(media.skin, 512,512, nk::rect(464,64,15,15));
        media.option_cursor = nk::subimage_id(media.skin, 512,512, nk::rect(451,67,9,9));
        media.header = nk::subimage_id(media.skin, 512,512, nk::rect(128,0,127,24));
        media.window = nk::subimage_id(media.skin, 512,512, nk::rect(128,23,127,104));
        media.scrollbar_inc_button = nk::subimage_id(media.skin, 512,512, nk::rect(464,256,15,15));
        media.scrollbar_inc_button_hover = nk::subimage_id(media.skin, 512,512, nk::rect(464,320,15,15));
        media.scrollbar_dec_button = nk::subimage_id(media.skin, 512,512, nk::rect(464,224,15,15));
        media.scrollbar_dec_button_hover = nk::subimage_id(media.skin, 512,512, nk::rect(464,288,15,15));
        media.button = nk::subimage_id(media.skin, 512,512, nk::rect(384,336,127,31));
        media.button_hover = nk::subimage_id(media.skin, 512,512, nk::rect(384,368,127,31));
        media.button_active = nk::subimage_id(media.skin, 512,512, nk::rect(384,400,127,31));
        media.tab_minimize = nk::subimage_id(media.skin, 512,512, nk::rect(451, 99, 9, 9));
        media.tab_maximize = nk::subimage_id(media.skin, 512,512, nk::rect(467,99,9,9));
        media.slider = nk::subimage_id(media.skin, 512,512, nk::rect(418,33,11,14));
        media.slider_hover = nk::subimage_id(media.skin, 512,512, nk::rect(418,49,11,14));
        media.slider_active = nk::subimage_id(media.skin, 512,512, nk::rect(418,64,11,14));

        /* window */
        ctx.style.window.background = nk::rgb(204,204,204);
        ctx.style.window.fixed_background = nk::style_item_image(media.window);
        ctx.style.window.border_color = nk::rgb(67,67,67);
        ctx.style.window.combo_border_color = nk::rgb(67,67,67);
        ctx.style.window.contextual_border_color = nk::rgb(67,67,67);
        ctx.style.window.menu_border_color = nk::rgb(67,67,67);
        ctx.style.window.group_border_color = nk::rgb(67,67,67);
        ctx.style.window.tooltip_border_color = nk::rgb(67,67,67);
        ctx.style.window.scrollbar_size = nk::vec2_from_floats(16,16);
        ctx.style.window.border_color = nk::rgba(0,0,0,0);
        ctx.style.window.padding = nk::vec2_from_floats(8,4);
        ctx.style.window.border = 3;

        /* window header */
        ctx.style.window.header.normal = nk::style_item_image(media.header);
        ctx.style.window.header.hover = nk::style_item_image(media.header);
        ctx.style.window.header.active = nk::style_item_image(media.header);
        ctx.style.window.header.label_normal = nk::rgb(95,95,95);
        ctx.style.window.header.label_hover = nk::rgb(95,95,95);
        ctx.style.window.header.label_active = nk::rgb(95,95,95);

        /* scrollbar */
        ctx.style.scrollv.normal          = nk::style_item_color(nk::rgb(184,184,184));
        ctx.style.scrollv.hover           = nk::style_item_color(nk::rgb(184,184,184));
        ctx.style.scrollv.active          = nk::style_item_color(nk::rgb(184,184,184));
        ctx.style.scrollv.cursor_normal   = nk::style_item_color(nk::rgb(220,220,220));
        ctx.style.scrollv.cursor_hover    = nk::style_item_color(nk::rgb(235,235,235));
        ctx.style.scrollv.cursor_active   = nk::style_item_color(nk::rgb(99,202,255));
        ctx.style.scrollv.dec_symbol      = nk::symbol_type::SYMBOL_NONE;
        ctx.style.scrollv.inc_symbol      = nk::symbol_type::SYMBOL_NONE;
        ctx.style.scrollv.show_buttons    = 1;
        ctx.style.scrollv.border_color    = nk::rgb(81,81,81);
        ctx.style.scrollv.cursor_border_color = nk::rgb(81,81,81);
        ctx.style.scrollv.border          = 1;
        ctx.style.scrollv.rounding        = 0;
        ctx.style.scrollv.border_cursor   = 1;
        ctx.style.scrollv.rounding_cursor = 2;

        /* scrollbar buttons */
        ctx.style.scrollv.inc_button.normal          = nk::style_item_image(media.scrollbar_inc_button);
        ctx.style.scrollv.inc_button.hover           = nk::style_item_image(media.scrollbar_inc_button_hover);
        ctx.style.scrollv.inc_button.active          = nk::style_item_image(media.scrollbar_inc_button_hover);
        ctx.style.scrollv.inc_button.border_color    = nk::rgba(0,0,0,0);
        ctx.style.scrollv.inc_button.text_background = nk::rgba(0,0,0,0);
        ctx.style.scrollv.inc_button.text_normal     = nk::rgba(0,0,0,0);
        ctx.style.scrollv.inc_button.text_hover      = nk::rgba(0,0,0,0);
        ctx.style.scrollv.inc_button.text_active     = nk::rgba(0,0,0,0);
        ctx.style.scrollv.inc_button.border          = 0.0f;

        ctx.style.scrollv.dec_button.normal          = nk::style_item_image(media.scrollbar_dec_button);
        ctx.style.scrollv.dec_button.hover           = nk::style_item_image(media.scrollbar_dec_button_hover);
        ctx.style.scrollv.dec_button.active          = nk::style_item_image(media.scrollbar_dec_button_hover);
        ctx.style.scrollv.dec_button.border_color    = nk::rgba(0,0,0,0);
        ctx.style.scrollv.dec_button.text_background = nk::rgba(0,0,0,0);
        ctx.style.scrollv.dec_button.text_normal     = nk::rgba(0,0,0,0);
        ctx.style.scrollv.dec_button.text_hover      = nk::rgba(0,0,0,0);
        ctx.style.scrollv.dec_button.text_active     = nk::rgba(0,0,0,0);
        ctx.style.scrollv.dec_button.border          = 0.0f;

        /* checkbox toggle */
        {struct nk::style_toggle *toggle;
        toggle = &ctx.style.checkbox;
        toggle->normal          = nk::style_item_image(media.check);
        toggle->hover           = nk::style_item_image(media.check);
        toggle->active          = nk::style_item_image(media.check);
        toggle->cursor_normal   = nk::style_item_image(media.check_cursor);
        toggle->cursor_hover    = nk::style_item_image(media.check_cursor);
        toggle->text_normal     = nk::rgb(95,95,95);
        toggle->text_hover      = nk::rgb(95,95,95);
        toggle->text_active     = nk::rgb(95,95,95);}

        /* option toggle */
        {struct nk::style_toggle *toggle;
        toggle = &ctx.style.option;
        toggle->normal          = nk::style_item_image(media.option);
        toggle->hover           = nk::style_item_image(media.option);
        toggle->active          = nk::style_item_image(media.option);
        toggle->cursor_normal   = nk::style_item_image(media.option_cursor);
        toggle->cursor_hover    = nk::style_item_image(media.option_cursor);
        toggle->text_normal     = nk::rgb(95,95,95);
        toggle->text_hover      = nk::rgb(95,95,95);
        toggle->text_active     = nk::rgb(95,95,95);}

        /* default button */
        ctx.style.button.normal = nk::style_item_image(media.button);
        ctx.style.button.hover = nk::style_item_image(media.button_hover);
        ctx.style.button.active = nk::style_item_image(media.button_active);
        ctx.style.button.border_color = nk::rgba(0,0,0,0);
        ctx.style.button.text_background = nk::rgba(0,0,0,0);
        ctx.style.button.text_normal = nk::rgb(95,95,95);
        ctx.style.button.text_hover = nk::rgb(95,95,95);
        ctx.style.button.text_active = nk::rgb(95,95,95);

        /* default text */
        ctx.style.text.color = nk::rgb(95,95,95);

        /* contextual button */
        ctx.style.contextual_button.normal = nk::style_item_color(nk::rgb(206,206,206));
        ctx.style.contextual_button.hover = nk::style_item_color(nk::rgb(229,229,229));
        ctx.style.contextual_button.active = nk::style_item_color(nk::rgb(99,202,255));
        ctx.style.contextual_button.border_color = nk::rgba(0,0,0,0);
        ctx.style.contextual_button.text_background = nk::rgba(0,0,0,0);
        ctx.style.contextual_button.text_normal = nk::rgb(95,95,95);
        ctx.style.contextual_button.text_hover = nk::rgb(95,95,95);
        ctx.style.contextual_button.text_active = nk::rgb(95,95,95);

        /* menu button */
        ctx.style.menu_button.normal = nk::style_item_color(nk::rgb(206,206,206));
        ctx.style.menu_button.hover = nk::style_item_color(nk::rgb(229,229,229));
        ctx.style.menu_button.active = nk::style_item_color(nk::rgb(99,202,255));
        ctx.style.menu_button.border_color = nk::rgba(0,0,0,0);
        ctx.style.menu_button.text_background = nk::rgba(0,0,0,0);
        ctx.style.menu_button.text_normal = nk::rgb(95,95,95);
        ctx.style.menu_button.text_hover = nk::rgb(95,95,95);
        ctx.style.menu_button.text_active = nk::rgb(95,95,95);

        /* tree */
        ctx.style.tab.text = nk::rgb(95,95,95);
        ctx.style.tab.tab_minimize_button.normal = nk::style_item_image(media.tab_minimize);
        ctx.style.tab.tab_minimize_button.hover = nk::style_item_image(media.tab_minimize);
        ctx.style.tab.tab_minimize_button.active = nk::style_item_image(media.tab_minimize);
        ctx.style.tab.tab_minimize_button.text_background = nk::rgba(0,0,0,0);
        ctx.style.tab.tab_minimize_button.text_normal = nk::rgba(0,0,0,0);
        ctx.style.tab.tab_minimize_button.text_hover = nk::rgba(0,0,0,0);
        ctx.style.tab.tab_minimize_button.text_active = nk::rgba(0,0,0,0);

        ctx.style.tab.tab_maximize_button.normal = nk::style_item_image(media.tab_maximize);
        ctx.style.tab.tab_maximize_button.hover = nk::style_item_image(media.tab_maximize);
        ctx.style.tab.tab_maximize_button.active = nk::style_item_image(media.tab_maximize);
        ctx.style.tab.tab_maximize_button.text_background = nk::rgba(0,0,0,0);
        ctx.style.tab.tab_maximize_button.text_normal = nk::rgba(0,0,0,0);
        ctx.style.tab.tab_maximize_button.text_hover = nk::rgba(0,0,0,0);
        ctx.style.tab.tab_maximize_button.text_active = nk::rgba(0,0,0,0);

        ctx.style.tab.node_minimize_button.normal = nk::style_item_image(media.tab_minimize);
        ctx.style.tab.node_minimize_button.hover = nk::style_item_image(media.tab_minimize);
        ctx.style.tab.node_minimize_button.active = nk::style_item_image(media.tab_minimize);
        ctx.style.tab.node_minimize_button.text_background = nk::rgba(0,0,0,0);
        ctx.style.tab.node_minimize_button.text_normal = nk::rgba(0,0,0,0);
        ctx.style.tab.node_minimize_button.text_hover = nk::rgba(0,0,0,0);
        ctx.style.tab.node_minimize_button.text_active = nk::rgba(0,0,0,0);

        ctx.style.tab.node_maximize_button.normal = nk::style_item_image(media.tab_maximize);
        ctx.style.tab.node_maximize_button.hover = nk::style_item_image(media.tab_maximize);
        ctx.style.tab.node_maximize_button.active = nk::style_item_image(media.tab_maximize);
        ctx.style.tab.node_maximize_button.text_background = nk::rgba(0,0,0,0);
        ctx.style.tab.node_maximize_button.text_normal = nk::rgba(0,0,0,0);
        ctx.style.tab.node_maximize_button.text_hover = nk::rgba(0,0,0,0);
        ctx.style.tab.node_maximize_button.text_active = nk::rgba(0,0,0,0);

        /* selectable */
        ctx.style.selectable.normal = nk::style_item_color(nk::rgb(206,206,206));
        ctx.style.selectable.hover = nk::style_item_color(nk::rgb(206,206,206));
        ctx.style.selectable.pressed = nk::style_item_color(nk::rgb(206,206,206));
        ctx.style.selectable.normal_active = nk::style_item_color(nk::rgb(185,205,248));
        ctx.style.selectable.hover_active = nk::style_item_color(nk::rgb(185,205,248));
        ctx.style.selectable.pressed_active = nk::style_item_color(nk::rgb(185,205,248));
        ctx.style.selectable.text_normal = nk::rgb(95,95,95);
        ctx.style.selectable.text_hover = nk::rgb(95,95,95);
        ctx.style.selectable.text_pressed = nk::rgb(95,95,95);
        ctx.style.selectable.text_normal_active = nk::rgb(95,95,95);
        ctx.style.selectable.text_hover_active = nk::rgb(95,95,95);
        ctx.style.selectable.text_pressed_active = nk::rgb(95,95,95);

        /* slider */
        ctx.style.slider.normal          = nk::style_item_hide();
        ctx.style.slider.hover           = nk::style_item_hide();
        ctx.style.slider.active          = nk::style_item_hide();
        ctx.style.slider.bar_normal      = nk::rgb(156,156,156);
        ctx.style.slider.bar_hover       = nk::rgb(156,156,156);
        ctx.style.slider.bar_active      = nk::rgb(156,156,156);
        ctx.style.slider.bar_filled      = nk::rgb(156,156,156);
        ctx.style.slider.cursor_normal   = nk::style_item_image(media.slider);
        ctx.style.slider.cursor_hover    = nk::style_item_image(media.slider_hover);
        ctx.style.slider.cursor_active   = nk::style_item_image(media.slider_active);
        ctx.style.slider.cursor_size     = nk::vec2_from_floats(16.5f,21);
        ctx.style.slider.bar_height      = 1;

        /* progressbar */
        ctx.style.progress.normal = nk::style_item_color(nk::rgb(231,231,231));
        ctx.style.progress.hover = nk::style_item_color(nk::rgb(231,231,231));
        ctx.style.progress.active = nk::style_item_color(nk::rgb(231,231,231));
        ctx.style.progress.cursor_normal = nk::style_item_color(nk::rgb(63,242,93));
        ctx.style.progress.cursor_hover = nk::style_item_color(nk::rgb(63,242,93));
        ctx.style.progress.cursor_active = nk::style_item_color(nk::rgb(63,242,93));
        ctx.style.progress.border_color = nk::rgb(114,116,115);
        ctx.style.progress.padding = nk::vec2_from_floats(0,0);
        ctx.style.progress.border = 2;
        ctx.style.progress.rounding = 1;

        /* combo */
        ctx.style.combo.normal = nk::style_item_color(nk::rgb(216,216,216));
        ctx.style.combo.hover = nk::style_item_color(nk::rgb(216,216,216));
        ctx.style.combo.active = nk::style_item_color(nk::rgb(216,216,216));
        ctx.style.combo.border_color = nk::rgb(95,95,95);
        ctx.style.combo.label_normal = nk::rgb(95,95,95);
        ctx.style.combo.label_hover = nk::rgb(95,95,95);
        ctx.style.combo.label_active = nk::rgb(95,95,95);
        ctx.style.combo.border = 1;
        ctx.style.combo.rounding = 1;

        /* combo button */
        ctx.style.combo.button.normal = nk::style_item_color(nk::rgb(216,216,216));
        ctx.style.combo.button.hover = nk::style_item_color(nk::rgb(216,216,216));
        ctx.style.combo.button.active = nk::style_item_color(nk::rgb(216,216,216));
        ctx.style.combo.button.text_background = nk::rgb(216,216,216);
        ctx.style.combo.button.text_normal = nk::rgb(95,95,95);
        ctx.style.combo.button.text_hover = nk::rgb(95,95,95);
        ctx.style.combo.button.text_active = nk::rgb(95,95,95);

        /* property */
        ctx.style.property.normal = nk::style_item_color(nk::rgb(216,216,216));
        ctx.style.property.hover = nk::style_item_color(nk::rgb(216,216,216));
        ctx.style.property.active = nk::style_item_color(nk::rgb(216,216,216));
        ctx.style.property.border_color = nk::rgb(81,81,81);
        ctx.style.property.label_normal = nk::rgb(95,95,95);
        ctx.style.property.label_hover = nk::rgb(95,95,95);
        ctx.style.property.label_active = nk::rgb(95,95,95);
        ctx.style.property.sym_left = nk::symbol_type::SYMBOL_TRIANGLE_LEFT;
        ctx.style.property.sym_right = nk::symbol_type::SYMBOL_TRIANGLE_RIGHT;
        ctx.style.property.rounding = 10;
        ctx.style.property.border = 1;

        /* edit */
        ctx.style.edit.normal = nk::style_item_color(nk::rgb(240,240,240));
        ctx.style.edit.hover = nk::style_item_color(nk::rgb(240,240,240));
        ctx.style.edit.active = nk::style_item_color(nk::rgb(240,240,240));
        ctx.style.edit.border_color = nk::rgb(62,62,62);
        ctx.style.edit.cursor_normal = nk::rgb(99,202,255);
        ctx.style.edit.cursor_hover = nk::rgb(99,202,255);
        ctx.style.edit.cursor_text_normal = nk::rgb(95,95,95);
        ctx.style.edit.cursor_text_hover = nk::rgb(95,95,95);
        ctx.style.edit.text_normal = nk::rgb(95,95,95);
        ctx.style.edit.text_hover = nk::rgb(95,95,95);
        ctx.style.edit.text_active = nk::rgb(95,95,95);
        ctx.style.edit.selected_normal = nk::rgb(99,202,255);
        ctx.style.edit.selected_hover = nk::rgb(99,202,255);
        ctx.style.edit.selected_text_normal = nk::rgb(95,95,95);
        ctx.style.edit.selected_text_hover = nk::rgb(95,95,95);
        ctx.style.edit.border = 1;
        ctx.style.edit.rounding = 2;

        /* property buttons */
        ctx.style.property.dec_button.normal = nk::style_item_color(nk::rgb(216,216,216));
        ctx.style.property.dec_button.hover = nk::style_item_color(nk::rgb(216,216,216));
        ctx.style.property.dec_button.active = nk::style_item_color(nk::rgb(216,216,216));
        ctx.style.property.dec_button.text_background = nk::rgba(0,0,0,0);
        ctx.style.property.dec_button.text_normal = nk::rgb(95,95,95);
        ctx.style.property.dec_button.text_hover = nk::rgb(95,95,95);
        ctx.style.property.dec_button.text_active = nk::rgb(95,95,95);
        ctx.style.property.inc_button = ctx.style.property.dec_button;

        /* property edit */
        ctx.style.property.edit.normal = nk::style_item_color(nk::rgb(216,216,216));
        ctx.style.property.edit.hover = nk::style_item_color(nk::rgb(216,216,216));
        ctx.style.property.edit.active = nk::style_item_color(nk::rgb(216,216,216));
        ctx.style.property.edit.border_color = nk::rgba(0,0,0,0);
        ctx.style.property.edit.cursor_normal = nk::rgb(95,95,95);
        ctx.style.property.edit.cursor_hover = nk::rgb(95,95,95);
        ctx.style.property.edit.cursor_text_normal = nk::rgb(216,216,216);
        ctx.style.property.edit.cursor_text_hover = nk::rgb(216,216,216);
        ctx.style.property.edit.text_normal = nk::rgb(95,95,95);
        ctx.style.property.edit.text_hover = nk::rgb(95,95,95);
        ctx.style.property.edit.text_active = nk::rgb(95,95,95);
        ctx.style.property.edit.selected_normal = nk::rgb(95,95,95);
        ctx.style.property.edit.selected_hover = nk::rgb(95,95,95);
        ctx.style.property.edit.selected_text_normal = nk::rgb(216,216,216);
        ctx.style.property.edit.selected_text_hover = nk::rgb(216,216,216);

        /* chart */
        ctx.style.chart.background = nk::style_item_color(nk::rgb(216,216,216));
        ctx.style.chart.border_color = nk::rgb(81,81,81);
        ctx.style.chart.color = nk::rgb(95,95,95);
        ctx.style.chart.selected_color = nk::rgb(255,0,0);
        ctx.style.chart.border = 1;
    }

    while (!glfwWindowShouldClose(win))
    {
        /* High DPI displays */
        nk::vec2f scale;
        glfwGetWindowSize(win, &width, &height);
        glfwGetFramebufferSize(win, &display_width, &display_height);
        scale.x = (float)display_width/(float)width;
        scale.y = (float)display_height/(float)height;

        /* Input */
        {double x, y;
        nk::input_begin(&ctx);
        glfwPollEvents();
        nk::input_key(&ctx, nk::NK_KEY_DEL, glfwGetKey(win, GLFW_KEY_DELETE) == GLFW_PRESS);
        nk::input_key(&ctx, nk::NK_KEY_ENTER, glfwGetKey(win, GLFW_KEY_ENTER) == GLFW_PRESS);
        nk::input_key(&ctx, nk::NK_KEY_TAB, glfwGetKey(win, GLFW_KEY_TAB) == GLFW_PRESS);
        nk::input_key(&ctx, nk::NK_KEY_BACKSPACE, glfwGetKey(win, GLFW_KEY_BACKSPACE) == GLFW_PRESS);
        nk::input_key(&ctx, nk::NK_KEY_LEFT, glfwGetKey(win, GLFW_KEY_LEFT) == GLFW_PRESS);
        nk::input_key(&ctx, nk::NK_KEY_RIGHT, glfwGetKey(win, GLFW_KEY_RIGHT) == GLFW_PRESS);
        nk::input_key(&ctx, nk::NK_KEY_UP, glfwGetKey(win, GLFW_KEY_UP) == GLFW_PRESS);
        nk::input_key(&ctx, nk::NK_KEY_DOWN, glfwGetKey(win, GLFW_KEY_DOWN) == GLFW_PRESS);
        if (glfwGetKey(win, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS ||
            glfwGetKey(win, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS) {
            nk::input_key(&ctx, nk::NK_KEY_COPY, glfwGetKey(win, GLFW_KEY_C) == GLFW_PRESS);
            nk::input_key(&ctx, nk::NK_KEY_PASTE, glfwGetKey(win, GLFW_KEY_P) == GLFW_PRESS);
            nk::input_key(&ctx, nk::NK_KEY_CUT, glfwGetKey(win, GLFW_KEY_X) == GLFW_PRESS);
            nk::input_key(&ctx, nk::NK_KEY_CUT, glfwGetKey(win, GLFW_KEY_E) == GLFW_PRESS);
            nk::input_key(&ctx, nk::NK_KEY_SHIFT, 1);
        } else {
            nk::input_key(&ctx, nk::NK_KEY_COPY, 0);
            nk::input_key(&ctx, nk::NK_KEY_PASTE, 0);
            nk::input_key(&ctx, nk::NK_KEY_CUT, 0);
            nk::input_key(&ctx, nk::NK_KEY_SHIFT, 0);
        }
        glfwGetCursorPos(win, &x, &y);
        nk::input_motion(&ctx, (int)x, (int)y);
        nk::input_button(&ctx, nk::NK_BUTTON_LEFT, (int)x, (int)y, glfwGetMouseButton(win, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS);
        nk::input_button(&ctx, nk::NK_BUTTON_MIDDLE, (int)x, (int)y, glfwGetMouseButton(win, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS);
        nk::input_button(&ctx, nk::NK_BUTTON_RIGHT, (int)x, (int)y, glfwGetMouseButton(win, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS);
        nk::input_end(&ctx);}

        /* GUI */
        if (nk::begin(&ctx, "Demo", nk::rect(50, 50, 300, 400),
        nk::panel_flags::WINDOW_BORDER|nk::panel_flags::WINDOW_MOVABLE|
        nk::panel_flags::WINDOW_TITLE))
        {
            int i;
            float id;
            static int slider = 10;
            static int field_len;
            static std::size_t prog_value = 60;
            static int current_weapon = 0;
            static char field_buffer[64];
            static float pos;
            static constexpr auto weapons = std::array{"Fist","Pistol","Shotgun","Plasma","BFG"};
            const float step = (2*3.141592654f) / 32;

            nk::layout_row_static(&ctx, 30, 120, 1);
            if (nk::button_label(&ctx, "button"))
                fprintf(stdout, "button pressed\n");

            nk::layout_row_dynamic(&ctx, 20, 1);
            nk::label(&ctx, "Label", nk::NK_TEXT_LEFT);
            nk::layout_row_dynamic(&ctx, 30, 2);
            nk::check_label(&ctx, "inactive", 0);
            nk::check_label(&ctx, "active", 1);
            nk::option_label(&ctx, "active", 1);
            nk::option_label(&ctx, "inactive", 0);

            nk::layout_row_dynamic(&ctx, 30, 1);
            nk::slider_int(&ctx, 0, &slider, 16, 1);
            nk::layout_row_dynamic(&ctx, 20, 1);
            nk::progress(&ctx, &prog_value, 100, true);

            nk::layout_row_dynamic(&ctx, 25, 1);
            nk::edit_string(&ctx, static_cast<nk::flag>(nk::edit_types::EDIT_FIELD), field_buffer, &field_len, 64, nk::filter_default);
            nk::property_float(&ctx, "#X:", -1024.0f, &pos, 1024.0f, 1, 1);
            current_weapon = nk::combo(&ctx, weapons.data(), weapons.size(), current_weapon, 25, nk::vec2_from_floats(nk::widget_width(&ctx),200));

            nk::layout_row_dynamic(&ctx, 100, 1);
            if (nk::chart_begin_colored(&ctx, nk::chart_type::CHART_LINES, nk::rgb(255,0,0), nk::rgb(150,0,0), 32, 0.0f, 1.0f)) {
                nk::chart_add_slot_colored(&ctx, nk::chart_type::CHART_LINES, nk::rgb(0,0,255), nk::rgb(0,0,150),32, -1.0f, 1.0f);
                nk::chart_add_slot_colored(&ctx, nk::chart_type::CHART_LINES, nk::rgb(0,255,0), nk::rgb(0,150,0), 32, -1.0f, 1.0f);
                for (id = 0, i = 0; i < 32; ++i) {
                    nk::chart_push_slot(&ctx, (float)fabs(sin(id)), 0);
                    nk::chart_push_slot(&ctx, (float)cos(id), 1);
                    nk::chart_push_slot(&ctx, (float)sin(id), 2);
                    id += step;
                }
            }
            nk::chart_end(&ctx);

            nk::layout_row_dynamic(&ctx, 250, 1);
            if (nk::group_begin(&ctx, "Standard", nk::panel_flags::WINDOW_BORDER|
              nk::panel_flags::WINDOW_BORDER))
            {
                if (nk::tree_push(&ctx, nk::tree_type::TREE_NODE, "Window", nk::collapse_states::MAXIMIZED)) {
                    static bool selected[8];
                    if (nk::tree_push(&ctx, nk::tree_type::TREE_NODE, "Next", nk::collapse_states::MAXIMIZED)) {
                        nk::layout_row_dynamic(&ctx, 20, 1);
                        for (i = 0; i < 4; ++i)
                            nk::selectable_label(&ctx, (selected[i]) ? "Selected": "Unselected", nk::NK_TEXT_LEFT, &selected[i]);
                        nk::tree_pop(&ctx);
                    }
                    if (nk::tree_push(&ctx, nk::tree_type::TREE_NODE, "Previous", nk::collapse_states::MAXIMIZED)) {
                        nk::layout_row_dynamic(&ctx, 20, 1);
                        for (i = 4; i < 8; ++i)
                            nk::selectable_label(&ctx, (selected[i]) ? "Selected": "Unselected", nk::NK_TEXT_LEFT, &selected[i]);
                        nk::tree_pop(&ctx);
                    }
                    nk::tree_pop(&ctx);
                }
                nk::group_end(&ctx);
            }
        }
        nk::end(&ctx);

        /* Draw */
        glViewport(0, 0, display_width, display_height);
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.5882f, 0.6666f, 0.6666f, 1.0f);
        device.draw(&ctx, width, height, nk::anti_aliasing::NK_ANTI_ALIASING_ON);
        glfwSwapBuffers(win);
    }
    glDeleteTextures(1,(const GLuint*)&media.skin);
    nk::font_atlas_clear(&atlas);
    nk::free(&ctx);
    glfwTerminate();
    return 0;
}

