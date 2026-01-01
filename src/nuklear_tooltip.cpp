#include "nuklear.h"
#include "nuklear_internal.h"

namespace nk {
  /* ===============================================================
   *
   *                              TOOLTIP
   *
   * ===============================================================*/
  NK_API bool
  tooltip_begin(struct context* ctx, float width) {
    int x, y, w, h;
    struct window* win;
    const struct input* in;
    rectf bounds;
    int ret;

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout)
      return 0;

    /* make sure that no nonblocking popup is currently active */
    win = ctx->current;
    in = &ctx->input;
    if (win->popup.win && ((int) win->popup.type & static_cast<int>(panel_set::PANEL_SET_NONBLOCK)))
      return 0;

    w = iceilf(width);
    h = iceilf(null_rect.h);
    x = ifloorf(in->mouse.pos.x + 1) - (int) win->layout->clip.x;
    y = ifloorf(in->mouse.pos.y + 1) - (int) win->layout->clip.y;

    bounds.x = (float) x;
    bounds.y = (float) y;
    bounds.w = (float) w;
    bounds.h = (float) h;

    ret = popup_begin(ctx, popup_type::POPUP_DYNAMIC,
                      "__##Tooltip##__", static_cast<flag>(panel_flags::WINDOW_NO_SCROLLBAR | panel_flags::WINDOW_BORDER), bounds);
    if (ret)
      win->layout->flags &= ~static_cast<flag>(window_flags::WINDOW_ROM);
    win->popup.type = panel_type::PANEL_TOOLTIP;
    ctx->current->layout->type = panel_type::PANEL_TOOLTIP;
    return ret;
  }

  NK_API void
  tooltip_end(struct context* ctx) {
    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    if (!ctx || !ctx->current)
      return;
    ctx->current->seq--;
    popup_close(ctx);
    popup_end(ctx);
  }
  NK_API void
  tooltip(struct context* ctx, const char* text) {
    const struct style* style;
    vec2f padding;

    int text_len;
    float text_width;
    float text_height;

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    NK_ASSERT(text);
    if (!ctx || !ctx->current || !ctx->current->layout || !text)
      return;

    /* fetch configuration data */
    style = &ctx->style;
    padding = style->window.padding;

    /* calculate size of the text and tooltip */
    text_len = strlen(text);
    text_width = style->font->width(style->font->userdata,
                                    style->font->height, text, text_len);
    text_width += (4 * padding.x);
    text_height = (style->font->height + 2 * padding.y);

    /* execute tooltip and fill with text */
    if (tooltip_begin(ctx, (float) text_width)) {
      layout_row_dynamic(ctx, (float) text_height, 1);
      text_string(ctx, text, text_len, NK_TEXT_LEFT);
      tooltip_end(ctx);
    }
  }
#ifdef NK_INCLUDE_STANDARD_VARARGS
  NK_API void
  tooltipf(struct context* ctx, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    tooltipfv(ctx, fmt, args);
    va_end(args);
  }
  NK_API void
  tooltipfv(struct context* ctx, const char* fmt, va_list args) {
    char buf[256];
    strfmt(buf, NK_LEN(buf), fmt, args);
    tooltip(ctx, buf);
  }
#endif
} // namespace nk
