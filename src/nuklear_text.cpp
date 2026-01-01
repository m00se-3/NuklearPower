#include "nuklear.h"
#include "nuklear_internal.h"

namespace nk {
  /* ===============================================================
   *
   *                              TEXT
   *
   * ===============================================================*/
  NK_LIB void
  widget_text(command_buffer* o, rectf b,
              const char* string, const int len, const text* t,
              const flag a, const user_font* f) {
    rectf label;

    NK_ASSERT(o);
    NK_ASSERT(t);
    if (!o || !t)
      return;

    b.h = NK_MAX(b.h, 2 * t->padding.y);
    label.x = 0;
    label.w = 0;
    label.y = b.y + t->padding.y;
    label.h = NK_MIN(f->height, b.h - 2 * t->padding.y);

    float text_width = f->width(f->userdata, f->height, (const char*) string, len);
    text_width += (2.0f * t->padding.x);

    /* align in x-axis */
    if (a & NK_TEXT_ALIGN_LEFT) {
      label.x = b.x + t->padding.x;
      label.w = NK_MAX(0, b.w - 2 * t->padding.x);
    } else if (a & NK_TEXT_ALIGN_CENTERED) {
      label.w = NK_MAX(1, 2 * t->padding.x + (float) text_width);
      label.x = (b.x + t->padding.x + ((b.w - 2 * t->padding.x) - label.w) / 2);
      label.x = NK_MAX(b.x + t->padding.x, label.x);
      label.w = NK_MIN(b.x + b.w, label.x + label.w);
      if (label.w >= label.x)
        label.w -= label.x;
    } else if (a & NK_TEXT_ALIGN_RIGHT) {
      label.x = NK_MAX(b.x + t->padding.x, (b.x + b.w) - (2 * t->padding.x + (float) text_width));
      label.w = (float) text_width + 2 * t->padding.x;
    } else
      return;

    /* align in y-axis */
    if (a & NK_TEXT_ALIGN_MIDDLE) {
      label.y = b.y + b.h / 2.0f - (float) f->height / 2.0f;
      label.h = NK_MAX(b.h / 2.0f, b.h - (b.h / 2.0f + f->height / 2.0f));
    } else if (a & NK_TEXT_ALIGN_BOTTOM) {
      label.y = b.y + b.h - f->height;
      label.h = f->height;
    }
    draw_text(o, label, (const char*) string, len, f, t->background, t->txt);
  }
  NK_LIB void
  widget_text_wrap(command_buffer* o, rectf b,
                   const char* string, const int len, const text* t,
                   const user_font* f) {
    float width;
    int glyphs = 0;
    int fitting = 0;
    int done = 0;
    rectf line;
    text text;
    INTERN rune seperator[] = {' '};

    NK_ASSERT(o);
    NK_ASSERT(t);
    if (!o || !t)
      return;

    text.padding = vec2_from_floats(0.0f, 0.0f);
    text.background = t->background;
    text.txt = t->txt;

    b.w = NK_MAX(b.w, 2 * t->padding.x);
    b.h = NK_MAX(b.h, 2 * t->padding.y);
    b.h = b.h - 2 * t->padding.y;

    line.x = b.x + t->padding.x;
    line.y = b.y + t->padding.y;
    line.w = b.w - 2 * t->padding.x;
    line.h = 2 * t->padding.y + f->height;

    fitting = text_clamp(f, string, len, line.w, &glyphs, &width, seperator, NK_LEN(seperator));
    while (done < len) {
      if (!fitting || line.y + line.h >= (b.y + b.h))
        break;
      widget_text(o, line, &string[done], fitting, &text, NK_TEXT_LEFT, f);
      done += fitting;
      line.y += f->height + 2 * t->padding.y;
      fitting = text_clamp(f, &string[done], len - done, line.w, &glyphs, &width, seperator, NK_LEN(seperator));
    }
  }
  NK_API void
  text_colored(context* ctx, const char* str, const int len,
               const flag alignment, const color color) {

    rectf bounds;
    text text;

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout)
      return;

    window* win = ctx->current;
    const style* style = &ctx->style;
    panel_alloc_space(&bounds, ctx);
    const vec2f item_padding = style->text.padding;

    text.padding.x = item_padding.x;
    text.padding.y = item_padding.y;
    text.background = style->window.background;
    text.txt = rgb_factor(color, style->text.color_factor);
    widget_text(&win->buffer, bounds, str, len, &text, alignment, style->font);
  }
  NK_API void
  text_wrap_colored(context* ctx, const char* str,
                    const int len, const color color) {

    rectf bounds;
    text text;

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout)
      return;

    window* win = ctx->current;
    const style* style = &ctx->style;
    panel_alloc_space(&bounds, ctx);
    const vec2f item_padding = style->text.padding;

    text.padding.x = item_padding.x;
    text.padding.y = item_padding.y;
    text.background = style->window.background;
    text.txt = rgb_factor(color, style->text.color_factor);
    widget_text_wrap(&win->buffer, bounds, str, len, &text, style->font);
  }
#ifdef NK_INCLUDE_STANDARD_VARARGS
  NK_API void
  labelf_colored(struct context* ctx, flag flags,
                 struct color color, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    labelfv_colored(ctx, flags, color, fmt, args);
    va_end(args);
  }
  NK_API void
  labelf_colored_wrap(struct context* ctx, struct color color,
                      const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    labelfv_colored_wrap(ctx, color, fmt, args);
    va_end(args);
  }
  NK_API void
  labelf(struct context* ctx, flag flags, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    labelfv(ctx, flags, fmt, args);
    va_end(args);
  }
  NK_API void
  labelf_wrap(struct context* ctx, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    labelfv_wrap(ctx, fmt, args);
    va_end(args);
  }
  NK_API void
  labelfv_colored(struct context* ctx, flag flags,
                  struct color color, const char* fmt, va_list args) {
    char buf[256];
    strfmt(buf, NK_LEN(buf), fmt, args);
    label_colored(ctx, buf, flags, color);
  }

  NK_API void
  labelfv_colored_wrap(struct context* ctx, struct color color,
                       const char* fmt, va_list args) {
    char buf[256];
    strfmt(buf, NK_LEN(buf), fmt, args);
    label_colored_wrap(ctx, buf, color);
  }

  NK_API void
  labelfv(struct context* ctx, flag flags, const char* fmt, va_list args) {
    char buf[256];
    strfmt(buf, NK_LEN(buf), fmt, args);
    label(ctx, buf, flags);
  }

  NK_API void
  labelfv_wrap(struct context* ctx, const char* fmt, va_list args) {
    char buf[256];
    strfmt(buf, NK_LEN(buf), fmt, args);
    label_wrap(ctx, buf);
  }

  NK_API void
  value_bool(struct context* ctx, const char* prefix, int value) {
    labelf(ctx, NK_TEXT_LEFT, "%s: %s", prefix, ((value) ? "true" : "false"));
  }
  NK_API void
  value_int(struct context* ctx, const char* prefix, int value) {
    labelf(ctx, NK_TEXT_LEFT, "%s: %d", prefix, value);
  }
  NK_API void
  value_uint(struct context* ctx, const char* prefix, unsigned int value) {
    labelf(ctx, NK_TEXT_LEFT, "%s: %u", prefix, value);
  }
  NK_API void
  value_float(struct context* ctx, const char* prefix, float value) {
    double double_value = (double) value;
    labelf(ctx, NK_TEXT_LEFT, "%s: %.3f", prefix, double_value);
  }
  NK_API void
  value_color_byte(struct context* ctx, const char* p, struct color c) {
    labelf(ctx, NK_TEXT_LEFT, "%s: (%d, %d, %d, %d)", p, c.r, c.g, c.b, c.a);
  }
  NK_API void
  value_color_float(struct context* ctx, const char* p, struct color color) {
    double c[4];
    color_dv(c, color);
    labelf(ctx, NK_TEXT_LEFT, "%s: (%.2f, %.2f, %.2f, %.2f)",
           p, c[0], c[1], c[2], c[3]);
  }
  NK_API void
  value_color_hex(struct context* ctx, const char* prefix, struct color color) {
    char hex[16];
    color_hex_rgba(hex, color);
    labelf(ctx, NK_TEXT_LEFT, "%s: %s", prefix, hex);
  }
#endif
  NK_API void
  text_string(context* ctx, const char* str, const int len, const flag alignment) {
    NK_ASSERT(ctx);
    if (!ctx)
      return;
    text_colored(ctx, str, len, alignment, ctx->style.text.color);
  }
  NK_API void
  text_wrap(context* ctx, const char* str, const int len) {
    NK_ASSERT(ctx);
    if (!ctx)
      return;
    text_wrap_colored(ctx, str, len, ctx->style.text.color);
  }
  NK_API void
  label(context* ctx, const char* str, const flag alignment) {
    text_string(ctx, str, strlen(str), alignment);
  }
  NK_API void
  label_colored(context* ctx, const char* str, const flag align,
                const color color) {
    text_colored(ctx, str, strlen(str), align, color);
  }
  NK_API void
  label_wrap(context* ctx, const char* str) {
    text_wrap(ctx, str, strlen(str));
  }
  NK_API void
  label_colored_wrap(context* ctx, const char* str, const color color) {
    text_wrap_colored(ctx, str, strlen(str), color);
  }
} // namespace nk
