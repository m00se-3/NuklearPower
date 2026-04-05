/* nuklear - v1.05 - public domain */
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <climits>

#include <utils.hpp>

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#include <nk/nuklear.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#ifdef __APPLE__
  #define NK_SHADER_VERSION "#version 150\n"
#else
  #define NK_SHADER_VERSION "#version 300 es\n"
#endif

struct media {
    nk::font *font_14;
    nk::font *font_18;
    nk::font *font_20;
    nk::font *font_22;
::
    nk::image unchecked;
    nk::image checked;
    nk::image rocket;
    nk::image cloud;
    nk::image pen;
    nk::image play;
    nk::image pause;
    nk::image stop;
    nk::image prev;
    nk::image next;
    nk::image tools;
    nk::image dir;
    nk::image copy;
    nk::image convert;
    nk::image del;
    nk::image edit;
    nk::image images[9];
    nk::image menu[6];
};

/* ===============================================================
 *
 *                          CUSTOM WIDGET
 *
 * ===============================================================*/
static int
ui_piemenu(nk::context *ctx, nk::vec2f pos, float radius,
            nk::image *icons, int item_count)
{
    int ret = -1;
    nk::rectf total_space;
    nk::rectf bounds;
    int active_item = 0;

    /* pie menu popup */
    nk::color border = ctx->style.window.border_color;
    nk::style_item background = ctx->style.window.fixed_background;
    ctx->style.window.fixed_background = nk::style_item_hide();
    ctx->style.window.border_color = nk::rgba(0,0,0,0);

    total_space  = nk::window_get_content_region(ctx);
    ctx->style.window.spacing = nk::vec2_from_floats(0,0);
    ctx->style.window.padding = nk::vec2_from_floats(0,0);

    if (nk::popup_begin(ctx, nk::popup_type::POPUP_STATIC, "piemenu", nk::panel_flags::WINDOW_NO_SCROLLBAR,
        nk::rect(pos.x - total_space.x - radius, pos.y - radius - total_space.y,
        2*radius,2*radius)))
    {
        int i = 0;
        nk::command_buffer* out = nk::window_get_canvas(ctx);
        const struct nk::input *in = &ctx->input;

        total_space = nk::window_get_content_region(ctx);
        ctx->style.window.spacing = nk::vec2_from_floats(4,4);
        ctx->style.window.padding = nk::vec2_from_floats(8,8);
        nk::layout_row_dynamic(ctx, total_space.h, 1);
        nk::widget(&bounds, ctx);

        /* outer circle */
        nk::fill_circle(out, bounds, nk::rgb(50,50,50));
        {
            /* circle buttons */
            float step = (2 * 3.141592654f) / (float)(std::max(1,item_count));
            float a_min = 0; float a_max = step;

            nk::vec2f center = nk::vec2_from_floats(bounds.x + bounds.w / 2.0f, bounds.y + bounds.h / 2.0f);
            nk::vec2f drag = nk::vec2_from_floats(in->mouse.pos.x - center.x, in->mouse.pos.y - center.y);
            float angle = (float)atan2(drag.y, drag.x);
            if (angle < -0.0f) angle += 2.0f * 3.141592654f;
            active_item = (int)(angle/step);

            for (i = 0; i < item_count; ++i) {
                nk::rectf content;
                float rx, ry, dx, dy, a;
                nk::fill_arc(out, center.x, center.y, (bounds.w/2.0f),
                    a_min, a_max, (active_item == i) ? nk::rgb(45,100,255): nk::rgb(60,60,60));

                /* separator line */
                rx = bounds.w/2.0f; ry = 0;
                dx = rx * (float)cos(a_min) - ry * (float)sin(a_min);
                dy = rx * (float)sin(a_min) + ry * (float)cos(a_min);
                nk::stroke_line(out, center.x, center.y,
                    center.x + dx, center.y + dy, 1.0f, nk::rgb(50,50,50));

                /* button content */
                a = a_min + (a_max - a_min)/2.0f;
                rx = bounds.w/2.5f; ry = 0;
                content.w = 30; content.h = 30;
                content.x = center.x + ((rx * (float)cos(a) - ry * (float)sin(a)) - content.w/2.0f);
                content.y = center.y + (rx * (float)sin(a) + ry * (float)cos(a) - content.h/2.0f);
                nk::draw_image(out, content, &icons[i], nk::rgb(255,255,255));
                a_min = a_max; a_max += step;
            }
        }
        {
            /* inner circle */
            nk::rectf inner;
            inner.x = bounds.x + bounds.w/2 - bounds.w/4;
            inner.y = bounds.y + bounds.h/2 - bounds.h/4;
            inner.w = bounds.w/2; inner.h = bounds.h/2;
            nk::fill_circle(out, inner, nk::rgb(45,45,45));

            /* active icon content */
            bounds.w = inner.w / 2.0f;
            bounds.h = inner.h / 2.0f;
            bounds.x = inner.x + inner.w/2 - bounds.w/2;
            bounds.y = inner.y + inner.h/2 - bounds.h/2;
            nk::draw_image(out, bounds, &icons[active_item], nk::rgb(255,255,255));
        }
        nk::layout_space_end(ctx);
        if (!nk::input_is_mouse_down(&ctx->input, nk::NK_BUTTON_RIGHT)) {
            nk::popup_close(ctx);
            ret = active_item;
        }
    } else ret = -2;
    ctx->style.window.spacing = nk::vec2_from_floats(4,4);
    ctx->style.window.padding = nk::vec2_from_floats(8,8);
    nk::popup_end(ctx);

    ctx->style.window.fixed_background = background;
    ctx->style.window.border_color = border;
    return ret;
}

/* ===============================================================
 *
 *                          GRID
 *
 * ===============================================================*/
static void
grid_demo(nk::context *ctx, const media *media)
{
    static char text[3][64];
    static int text_len[3];
    static const char *items[] = {"Item 0","item 1","item 2"};
    static int selected_item = 0;
    bool check = true;

    int i;
    nk::style_set_font(ctx, &media->font_20->handle);
    if (nk::begin(ctx, "Grid Demo", nk::rect(600, 350, 275, 250),
        nk::panel_flags::WINDOW_TITLE | nk::panel_flags::WINDOW_BORDER
        | nk::panel_flags::WINDOW_MOVABLE | nk::panel_flags::WINDOW_NO_SCROLLBAR))
    {
        nk::style_set_font(ctx, &media->font_18->handle);
        nk::layout_row_dynamic(ctx, 30, 2);
        nk::label(ctx, "Floating point:", nk::NK_TEXT_RIGHT);
        nk::edit_string(ctx, static_cast<nk::flag>(nk::edit_types::EDIT_FIELD), text[0], &text_len[0], 64, nk::filter_float);
        nk::label(ctx, "Hexadecimal:", nk::NK_TEXT_RIGHT);
        nk::edit_string(ctx, static_cast<nk::flag>(nk::edit_types::EDIT_FIELD), text[1], &text_len[1], 64, nk::filter_hex);
        nk::label(ctx, "Binary:", nk::NK_TEXT_RIGHT);
        nk::edit_string(ctx, static_cast<nk::flag>(nk::edit_types::EDIT_FIELD), text[2], &text_len[2], 64, nk::filter_binary);
        nk::label(ctx, "Checkbox:", nk::NK_TEXT_RIGHT);
        nk::checkbox_label(ctx, "Check me", &check);
        nk::label(ctx, "Combobox:", nk::NK_TEXT_RIGHT);
        if (nk::combo_begin_label(ctx, items[selected_item], nk::vec2_from_floats(nk::widget_width(ctx), 200))) {
            nk::layout_row_dynamic(ctx, 25, 1);
            for (i = 0; i < 3; ++i) {
              if (nk::combo_item_label(ctx, items[i], nk::NK_TEXT_LEFT)) {
                selected_item = i;
              }
            }

            nk::combo_end(ctx);
        }
    }
    nk::end(ctx);
    nk::style_set_font(ctx, &media->font_14->handle);
}

/* ===============================================================
 *
 *                          BUTTON DEMO
 *
 * ===============================================================*/
static void
ui_header(struct nk::context *ctx, struct media *media, const char *title)
{
    nk::style_set_font(ctx, &media->font_18->handle);
    nk::layout_row_dynamic(ctx, 20, 1);
    nk::label(ctx, title, nk::NK_TEXT_LEFT);
}

static void
ui_widget(struct nk::context *ctx, struct media *media, float height)
{
    static const float ratio[] = {0.15f, 0.85f};
    nk::style_set_font(ctx, &media->font_22->handle);
    nk::layout_row(ctx, nk::layout_format::DYNAMIC, height, 2, ratio);
    nk::spacing(ctx, 1);
}

static void
ui_widget_centered(struct nk::context *ctx, struct media *media, float height)
{
    static const float ratio[] = {0.15f, 0.50f, 0.35f};
    nk::style_set_font(ctx, &media->font_22->handle);
    nk::layout_row(ctx, nk::layout_format::DYNAMIC, height, 3, ratio);
    nk::spacing(ctx, 1);
}

static void
button_demo(struct nk::context *ctx, struct media *media)
{
    static int option = 1;
    static int toggle0 = 1;
    static int toggle1 = 0;
    static int toggle2 = 1;

    nk::style_set_font(ctx, &media->font_20->handle);
    nk::begin(ctx, "Button Demo", nk::rect(50,50,255,610),
        nk::panel_flags::WINDOW_BORDER|nk::panel_flags::WINDOW_MOVABLE|
        nk::panel_flags::WINDOW_TITLE);

    /*------------------------------------------------
     *                  MENU
     *------------------------------------------------*/
    nk::menubar_begin(ctx);
    {
        /* toolbar */
        nk::layout_row_static(ctx, 40, 40, 4);
        if (nk::menu_begin_image(ctx, "Music", media->play, nk::vec2_from_floats(110,120)))
        {
            /* settings */
            nk::layout_row_dynamic(ctx, 25, 1);
            nk::menu_item_image_label(ctx, media->play, "Play", nk::NK_TEXT_RIGHT);
            nk::menu_item_image_label(ctx, media->stop, "Stop", nk::NK_TEXT_RIGHT);
            nk::menu_item_image_label(ctx, media->pause, "Pause", nk::NK_TEXT_RIGHT);
            nk::menu_item_image_label(ctx, media->next, "Next", nk::NK_TEXT_RIGHT);
            nk::menu_item_image_label(ctx, media->prev, "Prev", nk::NK_TEXT_RIGHT);
            nk::menu_end(ctx);
        }
        nk::button_image(ctx, media->tools);
        nk::button_image(ctx, media->cloud);
        nk::button_image(ctx, media->pen);
    }
    nk::menubar_end(ctx);

    /*------------------------------------------------
     *                  BUTTON
     *------------------------------------------------*/
    ui_header(ctx, media, "Push buttons");
    ui_widget(ctx, media, 35);
    if (nk::button_label(ctx, "Push me"))
        puts("pushed!\n");
    ui_widget(ctx, media, 35);
    if (nk::button_image_label(ctx, media->rocket, "Styled", nk::NK_TEXT_CENTERED))
        puts("rocket!\n");

    /*------------------------------------------------
     *                  REPEATER
     *------------------------------------------------*/
    ui_header(ctx, media, "Repeater");
    ui_widget(ctx, media, 35);
    if (nk::button_label(ctx, "Press me"))
        puts("pressed!\n");

    /*------------------------------------------------
     *                  TOGGLE
     *------------------------------------------------*/
    ui_header(ctx, media, "Toggle buttons");
    ui_widget(ctx, media, 35);
    if (nk::button_image_label(ctx, (toggle0) ? media->checked: media->unchecked, "Toggle", nk::NK_TEXT_LEFT))
        toggle0 = !toggle0;

    ui_widget(ctx, media, 35);
    if (nk::button_image_label(ctx, (toggle1) ? media->checked: media->unchecked, "Toggle", nk::NK_TEXT_LEFT))
        toggle1 = !toggle1;

    ui_widget(ctx, media, 35);
    if (nk::button_image_label(ctx, (toggle2) ? media->checked: media->unchecked, "Toggle", nk::NK_TEXT_LEFT))
        toggle2 = !toggle2;

    /*------------------------------------------------
     *                  RADIO
     *------------------------------------------------*/
    ui_header(ctx, media, "Radio buttons");
    ui_widget(ctx, media, 35);
    if (nk::button_symbol_label(ctx, (option == 0)?nk::symbol_type::SYMBOL_CIRCLE_OUTLINE :
      nk::symbol_type::SYMBOL_CIRCLE_SOLID, "Select", nk::NK_TEXT_LEFT))
        option = 0;
    ui_widget(ctx, media, 35);
    if (nk::button_symbol_label(ctx, (option == 1)?nk::symbol_type::SYMBOL_CIRCLE_OUTLINE :
      nk::symbol_type::SYMBOL_CIRCLE_SOLID, "Select", nk::NK_TEXT_LEFT))
        option = 1;
    ui_widget(ctx, media, 35);
    if (nk::button_symbol_label(ctx, (option == 2)?nk::symbol_type::SYMBOL_CIRCLE_OUTLINE :
      nk::symbol_type::SYMBOL_CIRCLE_SOLID, "Select", nk::NK_TEXT_LEFT))
        option = 2;

    /*------------------------------------------------
     *                  CONTEXTUAL
     *------------------------------------------------*/
    nk::style_set_font(ctx, &media->font_18->handle);
    if (nk::contextual_begin(ctx, nk::panel_flags::WINDOW_NO_SCROLLBAR, nk::vec2_from_floats(150, 300), nk::window_get_bounds(ctx))) {
        nk::layout_row_dynamic(ctx, 30, 1);
        if (nk::contextual_item_image_label(ctx, media->copy, "Clone", nk::NK_TEXT_RIGHT))
            fprintf(stdout, "pressed clone!\n");
        if (nk::contextual_item_image_label(ctx, media->del, "Delete", nk::NK_TEXT_RIGHT))
            fprintf(stdout, "pressed delete!\n");
        if (nk::contextual_item_image_label(ctx, media->convert, "Convert", nk::NK_TEXT_RIGHT))
            fprintf(stdout, "pressed convert!\n");
        if (nk::contextual_item_image_label(ctx, media->edit, "Edit", nk::NK_TEXT_RIGHT))
            fprintf(stdout, "pressed edit!\n");
        nk::contextual_end(ctx);
    }
    nk::style_set_font(ctx, &media->font_14->handle);
    nk::end(ctx);
}

/* ===============================================================
 *
 *                          BASIC DEMO
 *
 * ===============================================================*/
static void
basic_demo(struct nk::context *ctx, struct media *media)
{
    static int image_active;
    static bool check0 = true;
    static bool check1 = false;
    static size_t prog = 80;
    static int selected_item = 0;
    static int selected_image = 3;
    static int selected_icon = 0;
    static const char *items[] = {"Item 0","item 1","item 2"};
    static int piemenu_active = 0;
    static nk::vec2f piemenu_pos;

    int i = 0;
    nk::style_set_font(ctx, &media->font_20->handle);
    nk::begin(ctx, "Basic Demo", nk::rect(320, 50, 275, 610),
        nk::panel_flags::WINDOW_BORDER|nk::panel_flags::WINDOW_MOVABLE|
        nk::panel_flags::WINDOW_TITLE);

    /*------------------------------------------------
     *                  POPUP BUTTON
     *------------------------------------------------*/
    ui_header(ctx, media, "Popup & Scrollbar & Images");
    ui_widget(ctx, media, 35);
    if (nk::button_image_label(ctx, media->dir, "Images", nk::NK_TEXT_CENTERED))
        image_active = !image_active;

    /*------------------------------------------------
     *                  SELECTED IMAGE
     *------------------------------------------------*/
    ui_header(ctx, media, "Selected Image");
    ui_widget_centered(ctx, media, 100);
    nk::get_image(ctx, media->images[selected_image]);

    /*------------------------------------------------
     *                  IMAGE POPUP
     *------------------------------------------------*/
    if (image_active) {
        if (nk::popup_begin(ctx, nk::popup_type::POPUP_STATIC, "Image Popup", 0, nk::rect(265, 0, 320, 220))) {
            nk::layout_row_static(ctx, 82, 82, 3);
            for (i = 0; i < 9; ++i) {
                if (nk::button_image(ctx, media->images[i])) {
                    selected_image = i;
                    image_active = 0;
                    nk::popup_close(ctx);
                }
            }
            nk::popup_end(ctx);
        }
    }
    /*------------------------------------------------
     *                  COMBOBOX
     *------------------------------------------------*/
    ui_header(ctx, media, "Combo box");
    ui_widget(ctx, media, 40);
    if (nk::combo_begin_label(ctx, items[selected_item], nk::vec2_from_floats(nk::widget_width(ctx), 200))) {
        nk::layout_row_dynamic(ctx, 35, 1);
        for (i = 0; i < 3; ++i)
            if (nk::combo_item_label(ctx, items[i], nk::NK_TEXT_LEFT))
                selected_item = i;
        nk::combo_end(ctx);
    }

    ui_widget(ctx, media, 40);
    if (nk::combo_begin_image_label(ctx, items[selected_icon], media->images[selected_icon], nk::vec2_from_floats(nk::widget_width(ctx), 200))) {
        nk::layout_row_dynamic(ctx, 35, 1);
        for (i = 0; i < 3; ++i)
            if (nk::combo_item_image_label(ctx, media->images[i], items[i], nk::NK_TEXT_RIGHT))
                selected_icon = i;
        nk::combo_end(ctx);
    }

    /*------------------------------------------------
     *                  CHECKBOX
     *------------------------------------------------*/
    ui_header(ctx, media, "Checkbox");
    ui_widget(ctx, media, 30);
    nk::checkbox_label(ctx, "Flag 1", &check0);
    ui_widget(ctx, media, 30);
    nk::checkbox_label(ctx, "Flag 2", &check1);

    /*------------------------------------------------
     *                  PROGRESSBAR
     *------------------------------------------------*/
    ui_header(ctx, media, "Progressbar");
    ui_widget(ctx, media, 35);
    nk::progress(ctx, &prog, 100, true);

    /*------------------------------------------------
     *                  PIEMENU
     *------------------------------------------------*/
    if (nk::input_is_mouse_click_down_in_rect(&ctx->input, nk::NK_BUTTON_RIGHT,
        nk::window_get_bounds(ctx),true)){
        piemenu_pos = ctx->input.mouse.pos;
        piemenu_active = 1;
    }

    if (piemenu_active) {
        int ret = ui_piemenu(ctx, piemenu_pos, 140, &media->menu[0], 6);
        if (ret == -2) piemenu_active = 0;
        if (ret != -1) {
            fprintf(stdout, "piemenu selected: %d\n", ret);
            piemenu_active = 0;
        }
    }
    nk::style_set_font(ctx, &media->font_14->handle);
    nk::end(ctx);
}

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

static struct nk::image
icon_load(const char *filename)
{
  const auto high_bit_set = [](const unsigned int val) { return (val & 0x80000000) > 0; };

  unsigned int x,y,n;
  GLuint tex;
  unsigned char *data = stbi_load(filename, &x, &y, &n, 0);
  if (!data || high_bit_set(x) || high_bit_set(y)) {
    die("[SDL]: failed to load image: %s", filename);
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
  return nk::image_id((int)tex);
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
    device device;
    nk::font_atlas atlas;
    media media;
    nk::context ctx;

    NK_UNUSED(argc);
    NK_UNUSED(argv);

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

    {/* GUI */
    {const void *image; unsigned int w, h;
    struct nk::font_config cfg = nk::font_config(0);
    cfg.oversample_h = 3; cfg.oversample_v = 2;
    /* Loading one font with different heights is only required if you want higher
     * quality text otherwise you can just set the font height directly
     * e.g.: ctx->style.font.height = 20. */
    nk::font_atlas_init_default(&atlas);
    nk::font_atlas_begin(&atlas);
    media.font_14 = nk::font_atlas_add_from_file(&atlas, "../../extra_font/Roboto-Regular.ttf", 14.0f, &cfg);
    media.font_18 = nk::font_atlas_add_from_file(&atlas, "../../extra_font/Roboto-Regular.ttf", 18.0f, &cfg);
    media.font_20 = nk::font_atlas_add_from_file(&atlas, "../../extra_font/Roboto-Regular.ttf", 20.0f, &cfg);
    media.font_22 = nk::font_atlas_add_from_file(&atlas, "../../extra_font/Roboto-Regular.ttf", 22.0f, &cfg);
    image = nk::font_atlas_bake(&atlas, &w, &h, nk::NK_FONT_ATLAS_RGBA32);
    device.upload_atlas(image, w, h);
    nk::font_atlas_end(&atlas, nk::handle_id((int)device.font_tex), &device.tex_null);}
    nk::init_default(&ctx, &media.font_14->handle);}

    /* icons */
    glEnable(GL_TEXTURE_2D);
    media.unchecked = icon_load("../icon/unchecked.png");
    media.checked = icon_load("../icon/checked.png");
    media.rocket = icon_load("../icon/rocket.png");
    media.cloud = icon_load("../icon/cloud.png");
    media.pen = icon_load("../icon/pen.png");
    media.play = icon_load("../icon/play.png");
    media.pause = icon_load("../icon/pause.png");
    media.stop = icon_load("../icon/stop.png");
    media.next =  icon_load("../icon/next.png");
    media.prev =  icon_load("../icon/prev.png");
    media.tools = icon_load("../icon/tools.png");
    media.dir = icon_load("../icon/directory.png");
    media.copy = icon_load("../icon/copy.png");
    media.convert = icon_load("../icon/export.png");
    media.del = icon_load("../icon/delete.png");
    media.edit = icon_load("../icon/edit.png");
    media.menu[0] = icon_load("../icon/home.png");
    media.menu[1] = icon_load("../icon/phone.png");
    media.menu[2] = icon_load("../icon/plane.png");
    media.menu[3] = icon_load("../icon/wifi.png");
    media.menu[4] = icon_load("../icon/settings.png");
    media.menu[5] = icon_load("../icon/volume.png");

    {int i;
    for (i = 0; i < 9; ++i) {
        char buffer[256];
        sprintf(buffer, "../images/image%d.png", (i+1));
        media.images[i] = icon_load(buffer);
    }}

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
        basic_demo(&ctx, &media);
        button_demo(&ctx, &media);
        grid_demo(&ctx, &media);

        /* Draw */
        glViewport(0, 0, display_width, display_height);
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
        // device.draw(&ctx, width, height, scale, nk::NK_ANTI_ALIASING_ON);
        device.draw(&ctx, width, height, nk::NK_ANTI_ALIASING_ON);
        glfwSwapBuffers(win);
    }

    glDeleteTextures(1,(const GLuint*)&media.unchecked.handle.id);
    glDeleteTextures(1,(const GLuint*)&media.checked.handle.id);
    glDeleteTextures(1,(const GLuint*)&media.rocket.handle.id);
    glDeleteTextures(1,(const GLuint*)&media.cloud.handle.id);
    glDeleteTextures(1,(const GLuint*)&media.pen.handle.id);
    glDeleteTextures(1,(const GLuint*)&media.play.handle.id);
    glDeleteTextures(1,(const GLuint*)&media.pause.handle.id);
    glDeleteTextures(1,(const GLuint*)&media.stop.handle.id);
    glDeleteTextures(1,(const GLuint*)&media.next.handle.id);
    glDeleteTextures(1,(const GLuint*)&media.prev.handle.id);
    glDeleteTextures(1,(const GLuint*)&media.tools.handle.id);
    glDeleteTextures(1,(const GLuint*)&media.dir.handle.id);
    glDeleteTextures(1,(const GLuint*)&media.del.handle.id);

    nk::font_atlas_clear(&atlas);
    nk::free(&ctx);

    glfwTerminate();
    return 0;
}

