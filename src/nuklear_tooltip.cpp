#include <nk/nuklear.hpp>

namespace nk {
  /* ===============================================================
   *
   *                              TOOLTIP
   *
   * ===============================================================*/
  NK_API bool
  tooltip_begin(context* ctx, float width) {
    rectf bounds;

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout)
      return 0;

    /* make sure that no nonblocking popup is currently active */
    window* win = ctx->current;
    const input* in = &ctx->input;
    if (win->popup.win && ((int) win->popup.type & static_cast<int>(panel_set::PANEL_SET_NONBLOCK)))
      return 0;

    const int w = iceilf(width);
    const int h = iceilf(null_rect.h);
    const int x = ifloorf(in->mouse.pos.x + 1) - (int) win->layout->clip.x;
    const int y = ifloorf(in->mouse.pos.y + 1) - (int) win->layout->clip.y;

    bounds.x = (float) x;
    bounds.y = (float) y;
    bounds.w = (float) w;
    bounds.h = (float) h;

    const int ret = popup_begin(ctx, popup_type::POPUP_DYNAMIC,
                          "__##Tooltip##__", static_cast<flag>(panel_flags::WINDOW_NO_SCROLLBAR | panel_flags::WINDOW_BORDER), bounds);
    if (ret)
      win->layout->flags &= ~static_cast<flag>(window_flags::WINDOW_ROM);
    win->popup.type = panel_type::PANEL_TOOLTIP;
    ctx->current->layout->type = panel_type::PANEL_TOOLTIP;
    return ret;
  }

  NK_API void
  tooltip_end(context* ctx) {
    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    if (!ctx || !ctx->current)
      return;
    ctx->current->seq--;
    popup_close(ctx);
    popup_end(ctx);
  }
  NK_API void
  tooltip(context* ctx, const char* text) {

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    NK_ASSERT(text);
    if (!ctx || !ctx->current || !ctx->current->layout || !text)
      return;

    /* fetch configuration data */
    const style* style = &ctx->style;
    const vec2f padding = style->window.padding;

    /* calculate size of the text and tooltip */
    const int text_len = strlen(text);
    float text_width = style->font->width(style->font->userdata,
                                          style->font->height, text, text_len);
    text_width += (4 * padding.x);
    float text_height = (style->font->height + 2 * padding.y);

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
