/* nuklear - v1.05 - public domain */
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <climits>

#include <utils.hpp>

#define NK_PRIVATE
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

/* function icon_load () is not used to build this file but might still be useful :) */
/*
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

static struct nk_image
icon_load(const char *filename)
{
    int x,y,n;
    GLuint tex;
    unsigned char *data = stbi_load(filename, &x, &y, &n, 0);
    if (!data) die("[SDL]: failed to load image: %s", filename);

     glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);
    return nk_image_id((int)tex);
}
*/


struct media {
  nk::font *font_14;
  nk::font *font_18;
  nk::font *font_20;
  nk::font *font_22;

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

/* glfw callbacks (I don't know if there is a easier way to access text and scroll )*/
namespace {
  void error_callback(const int e, const char *d) {
    printf("Error %d: %s\n", e, d);
  }

  void text_input(GLFWwindow *win, const unsigned int codepoint) {
    nk::input_unicode(static_cast<nk::context*>(glfwGetWindowUserPointer(win)), codepoint);
  }

  void scroll_input(GLFWwindow *win, double  /*_*/, double yoff) {
    nk::input_scroll(static_cast<nk::context*>(glfwGetWindowUserPointer(win)), nk::vec2_from_floats(0, static_cast<float>(yoff)));
  }

  void pump_input(nk::context *ctx, GLFWwindow *win)
  {
      double x{}, y{};
      nk::input_begin(ctx);
      glfwPollEvents();

      nk::input_key(ctx, nk::NK_KEY_DEL, glfwGetKey(win, GLFW_KEY_DELETE) == GLFW_PRESS);
      nk::input_key(ctx, nk::NK_KEY_ENTER, glfwGetKey(win, GLFW_KEY_ENTER) == GLFW_PRESS);
      nk::input_key(ctx, nk::NK_KEY_TAB, glfwGetKey(win, GLFW_KEY_TAB) == GLFW_PRESS);
      nk::input_key(ctx, nk::NK_KEY_BACKSPACE, glfwGetKey(win, GLFW_KEY_BACKSPACE) == GLFW_PRESS);
      nk::input_key(ctx, nk::NK_KEY_LEFT, glfwGetKey(win, GLFW_KEY_LEFT) == GLFW_PRESS);
      nk::input_key(ctx, nk::NK_KEY_RIGHT, glfwGetKey(win, GLFW_KEY_RIGHT) == GLFW_PRESS);
      nk::input_key(ctx, nk::NK_KEY_UP, glfwGetKey(win, GLFW_KEY_UP) == GLFW_PRESS);
      nk::input_key(ctx, nk::NK_KEY_DOWN, glfwGetKey(win, GLFW_KEY_DOWN) == GLFW_PRESS);

      if (glfwGetKey(win, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS ||
          glfwGetKey(win, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS) {
          nk::input_key(ctx, nk::NK_KEY_COPY, glfwGetKey(win, GLFW_KEY_C) == GLFW_PRESS);
          nk::input_key(ctx, nk::NK_KEY_PASTE, glfwGetKey(win, GLFW_KEY_P) == GLFW_PRESS);
          nk::input_key(ctx, nk::NK_KEY_CUT, glfwGetKey(win, GLFW_KEY_X) == GLFW_PRESS);
          nk::input_key(ctx, nk::NK_KEY_CUT, glfwGetKey(win, GLFW_KEY_E) == GLFW_PRESS);
          nk::input_key(ctx, nk::NK_KEY_SHIFT, 1);
      } else {
          nk::input_key(ctx, nk::NK_KEY_COPY, 0);
          nk::input_key(ctx, nk::NK_KEY_PASTE, 0);
          nk::input_key(ctx, nk::NK_KEY_CUT, 0);
          nk::input_key(ctx, nk::NK_KEY_SHIFT, 0);
      }

      glfwGetCursorPos(win, &x, &y);
      nk::input_motion(ctx, static_cast<int>(x), static_cast<int>(y));
      nk::input_button(ctx, nk::NK_BUTTON_LEFT, static_cast<int>(x), static_cast<int>(y), glfwGetMouseButton(win, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS);
      nk::input_button(ctx, nk::NK_BUTTON_MIDDLE, static_cast<int>(x), static_cast<int>(y), glfwGetMouseButton(win, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS);
      nk::input_button(ctx, nk::NK_BUTTON_RIGHT, static_cast<int>(x), static_cast<int>(y), glfwGetMouseButton(win, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS);
      nk::input_end(ctx);
  }

}

struct nk_canvas {
    nk::command_buffer *painter;
    nk::vec2f item_spacing;
    nk::vec2f panel_padding;
    nk::style_item window_background;
};

static void canvas_begin(nk::context *ctx, nk_canvas *canvas, nk::flag flags,
    float x, float y, float width, float height, const nk::color background_color)
{
    /* save style properties which will be overwritten */
    canvas->panel_padding = ctx->style.window.padding;
    canvas->item_spacing = ctx->style.window.spacing;
    canvas->window_background = ctx->style.window.fixed_background;

    /* use the complete window space and set background */
    ctx->style.window.spacing = nk::vec2f(0,0);
    ctx->style.window.padding = nk::vec2f(0,0);
    ctx->style.window.fixed_background = nk::style_item_color(background_color);

    /* create/update window and set position + size */
    flags = flags & ~nk::window_flags::WINDOW_DYNAMIC;
    nk::window_set_bounds(ctx, "Window", nk::rectf(x, y, width, height));
    nk::begin(ctx, "Window", nk::rectf(x, y, width, height), nk::panel_flags::WINDOW_NO_SCROLLBAR|flags);

    /* allocate the complete window space for drawing */
    {
      nk::rectf total_space{};
      total_space = nk::window_get_content_region(ctx);
      nk::layout_row_dynamic(ctx, total_space.h, 1);
      nk::widget(&total_space, ctx);
      canvas->painter = nk::window_get_canvas(ctx);
    }
}

static void
canvas_end(nk::context *ctx, nk_canvas *canvas)
{
    nk::end(ctx);
    ctx->style.window.spacing = canvas->panel_padding;
    ctx->style.window.padding = canvas->item_spacing;
    ctx->style.window.fixed_background = canvas->window_background;
}

int main([[maybe_unused]] int argc,[[maybe_unused]] char *argv[])
{
    /* Platform */
    static GLFWwindow *win;
    int width = 0, height = 0;

    /* GUI */
    device device;
    nk::font_atlas atlas;
    nk::context ctx;

    /* GLFW */
    glfwSetErrorCallback(error_callback);
    if (glfwInit() == 0) {
        puts("[GFLW] failed to init!\n");
        exit(1);
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    win = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Demo", NULL, NULL);
    glfwMakeContextCurrent(win);
    glfwSetWindowUserPointer(win, &ctx);
    glfwSetCharCallback(win, text_input);
    glfwSetScrollCallback(win, scroll_input);
    glfwGetWindowSize(win, &width, &height);

    /* OpenGL */
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to setup GLEW\n");
        exit(1);
    }

    /* GUI */
    {
      unsigned int w, h;
      nk::font_atlas_init_default(&atlas);
      nk::font_atlas_begin(&atlas);
      nk::font* font = nk::font_atlas_add_default(&atlas, 13, 0);
      const void* image = nk::font_atlas_bake(&atlas, &w, &h, nk::NK_FONT_ATLAS_RGBA32);
      device.upload_atlas(image, w, h);
      nk::font_atlas_end(&atlas, nk::handle_id((int)device.font_tex), &device.tex_null);
      nk::init_default(&ctx, &font->handle);

      glEnable(GL_TEXTURE_2D);
      while (glfwWindowShouldClose(win) == 0)
      {
          /* input */
          pump_input(&ctx, win);

          /* draw */
          {
            nk_canvas canvas;
          canvas_begin(&ctx, &canvas, 0, 0, 0, static_cast<float>(width), static_cast<float>(height), nk::rgb(250,250,250));
          {
              nk::fill_rect(canvas.painter, nk::rect(15,15,210,210), 5, nk::rgb(247, 230, 154));
              nk::fill_rect(canvas.painter, nk::rect(20,20,200,200), 5, nk::rgb(188, 174, 118));
              nk::draw_text(canvas.painter, nk::rect(30, 30, 150, 20), "Text to draw", 12, &font->handle, nk::rgb(188,174,118), nk::rgb(0,0,0));
              nk::fill_rect(canvas.painter, nk::rect(250,20,100,100), 0, nk::rgb(0,0,255));
              nk::fill_circle(canvas.painter, nk::rect(20,250,100,100), nk::rgb(255,0,0));
              nk::fill_triangle(canvas.painter, 250, 250, 350, 250, 300, 350, nk::rgb(0,255,0));
              nk::fill_arc(canvas.painter, 300, 180, 50, 0, 3.141592654f * 3.0f / 4.0f, nk::rgb(255,255,0));

              {
                float points[12];
                points[0] = 200; points[1] = 250;
                points[2] = 250; points[3] = 350;
                points[4] = 225; points[5] = 350;
                points[6] = 200; points[7] = 300;
                points[8] = 175; points[9] = 350;
                points[10] = 150; points[11] = 350;
                nk::fill_polygon(canvas.painter, points, 6, nk::rgb(0,0,0));}

                nk::stroke_line(canvas.painter, 15, 10, 200, 10, 2.0f, nk::rgb(189,45,75));
                nk::stroke_rect(canvas.painter, nk::rect(370, 20, 100, 100), 10, 3, nk::rgb(0,0,255));
                nk::stroke_curve(canvas.painter, 380, 200, 405, 270, 455, 120, 480, 200, 2, nk::rgb(0,150,220));
                nk::stroke_circle(canvas.painter, nk::rect(20, 370, 100, 100), 5, nk::rgb(0,255,120));
                nk::stroke_triangle(canvas.painter, 370, 250, 470, 250, 420, 350, 6, nk::rgb(255,0,143));
          }
          canvas_end(&ctx, &canvas);}

          /* Draw */
          /* Framebuffer size is used instead of window size because the window size is in screen coordinates instead of pixels.
           * See https://www.glfw.org/docs/latest/window_guide.html#window_size for more info
           */
          glfwGetFramebufferSize(win, &width, &height);
          glViewport(0, 0, width, height);
          glClear(GL_COLOR_BUFFER_BIT);
          glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
          device.draw(&ctx, width, height, nk::NK_ANTI_ALIASING_ON);
          glfwSwapBuffers(win);
      }
    }
    nk::font_atlas_clear(&atlas);
    nk::free(&ctx);
    glfwTerminate();
    return 0;
}

