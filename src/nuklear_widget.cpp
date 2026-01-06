#include <nk/nuklear.hpp>

namespace nk {
  /* ===============================================================
   *
   *                              WIDGET
   *
   * ===============================================================*/
  NK_API rectf
  widget_bounds(const context* ctx) {
    rectf bounds;
    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    if (!ctx || !ctx->current)
      return rect(0, 0, 0, 0);
    layout_peek(&bounds, ctx);
    return bounds;
  }
  NK_API vec2f
  widget_position(const context* ctx) {
    rectf bounds;
    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    if (!ctx || !ctx->current)
      return vec2_from_floats(0.0f, 0.0f);

    layout_peek(&bounds, ctx);
    return vec2_from_floats(bounds.x, bounds.y);
  }
  NK_API vec2f
  widget_size(const context* ctx) {
    rectf bounds;
    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    if (!ctx || !ctx->current)
      return vec2_from_floats(0.0f, 0.0f);

    layout_peek(&bounds, ctx);
    return vec2_from_floats(bounds.w, bounds.h);
  }
  NK_API float
  widget_width(const context* ctx) {
    rectf bounds;
    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    if (!ctx || !ctx->current)
      return 0;

    layout_peek(&bounds, ctx);
    return bounds.w;
  }
  NK_API float
  widget_height(const context* ctx) {
    rectf bounds;
    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    if (!ctx || !ctx->current)
      return 0;

    layout_peek(&bounds, ctx);
    return bounds.h;
  }
  NK_API bool
  widget_is_hovered(const context* ctx) {
    rectf v;
    rectf bounds;
    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    if (!ctx || !ctx->current || ctx->active != ctx->current)
      return 0;

    rectf c = ctx->current->layout->clip;
    c.x = (float) ((int) c.x);
    c.y = (float) ((int) c.y);
    c.w = (float) ((int) c.w);
    c.h = (float) ((int) c.h);

    layout_peek(&bounds, ctx);
    unify(&v, &c, bounds.x, bounds.y, bounds.x + bounds.w, bounds.y + bounds.h);
    if (!INTERSECT(c.x, c.y, c.w, c.h, bounds.x, bounds.y, bounds.w, bounds.h))
      return 0;
    return input_is_mouse_hovering_rect(&ctx->input, bounds);
  }
  NK_API bool
  widget_is_mouse_clicked(const context* ctx, const buttons btn) {
    rectf v;
    rectf bounds;
    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    if (!ctx || !ctx->current || ctx->active != ctx->current)
      return 0;

    rectf c = ctx->current->layout->clip;
    c.x = (float) ((int) c.x);
    c.y = (float) ((int) c.y);
    c.w = (float) ((int) c.w);
    c.h = (float) ((int) c.h);

    layout_peek(&bounds, ctx);
    unify(&v, &c, bounds.x, bounds.y, bounds.x + bounds.w, bounds.y + bounds.h);
    if (!INTERSECT(c.x, c.y, c.w, c.h, bounds.x, bounds.y, bounds.w, bounds.h))
      return 0;
    return input_mouse_clicked(&ctx->input, btn, bounds);
  }
  NK_API bool
  widget_has_mouse_click_down(const context* ctx, const buttons btn, const bool down) {
    rectf v;
    rectf bounds;
    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    if (!ctx || !ctx->current || ctx->active != ctx->current)
      return 0;

    rectf c = ctx->current->layout->clip;
    c.x = (float) ((int) c.x);
    c.y = (float) ((int) c.y);
    c.w = (float) ((int) c.w);
    c.h = (float) ((int) c.h);

    layout_peek(&bounds, ctx);
    unify(&v, &c, bounds.x, bounds.y, bounds.x + bounds.w, bounds.y + bounds.h);
    if (!INTERSECT(c.x, c.y, c.w, c.h, bounds.x, bounds.y, bounds.w, bounds.h))
      return 0;
    return input_has_mouse_click_down_in_rect(&ctx->input, btn, bounds, down);
  }
  NK_API widget_layout_states
  widget(rectf* bounds, const context* ctx) {
    rectf v;

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout)
      return NK_WIDGET_INVALID;

    /* allocate space and check if the widget needs to be updated and drawn */
    panel_alloc_space(bounds, ctx);
    const window* win = ctx->current;
    const panel* layout = win->layout;
    const input* in = &ctx->input;
    rectf c = layout->clip;

    /*  if one of these triggers you forgot to add an `if` condition around either
        a window, group, popup, combobox or contextual menu `begin` and `end` block.
        Example:
            if (begin(...) {...} end(...); or
            if (group_begin(...) { group_end(...);} */
    NK_ASSERT(!(layout->flags & window_flags::WINDOW_MINIMIZED));
    NK_ASSERT(!(layout->flags & window_flags::WINDOW_HIDDEN));
    NK_ASSERT(!(layout->flags & window_flags::WINDOW_CLOSED));

    /* need to convert to int here to remove floating point errors */
    bounds->x = (float) ((int) bounds->x);
    bounds->y = (float) ((int) bounds->y);
    bounds->w = (float) ((int) bounds->w);
    bounds->h = (float) ((int) bounds->h);

    c.x = (float) ((int) c.x);
    c.y = (float) ((int) c.y);
    c.w = (float) ((int) c.w);
    c.h = (float) ((int) c.h);

    unify(&v, &c, bounds->x, bounds->y, bounds->x + bounds->w, bounds->y + bounds->h);
    if (!INTERSECT(c.x, c.y, c.w, c.h, bounds->x, bounds->y, bounds->w, bounds->h))
      return NK_WIDGET_INVALID;
    if (win->widgets_disabled)
      return NK_WIDGET_DISABLED;
    if (!NK_INBOX(in->mouse.pos.x, in->mouse.pos.y, v.x, v.y, v.w, v.h))
      return NK_WIDGET_ROM;
    return NK_WIDGET_VALID;
  }
  NK_API widget_layout_states
  widget_fitting(rectf* bounds, const context* ctx,
                 const vec2f item_padding) {
    /* update the bounds to stand without padding  */
    NK_UNUSED(item_padding);

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout)
      return NK_WIDGET_INVALID;

    const widget_layout_states state = widget(bounds, ctx);
    return state;
  }
  NK_API void
  spacing(context* ctx, int cols) {
    rectf none;
    int i;

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout)
      return;

    /* spacing over row boundaries */
    window* win = ctx->current;
    panel* layout = win->layout;
    const int index = (layout->row.index + cols) % layout->row.columns;
    const int rows = (layout->row.index + cols) / layout->row.columns;
    if (rows) {
      for (i = 0; i < rows; ++i)
        panel_alloc_row(ctx, win);
      cols = index;
    }
    /* non table layout need to allocate space */
    if (layout->row.type != panel_row_layout_type::LAYOUT_DYNAMIC_FIXED &&
        layout->row.type != panel_row_layout_type::LAYOUT_STATIC_FIXED) {
      for (i = 0; i < cols; ++i)
        panel_alloc_space(&none, ctx);
    }
    layout->row.index = index;
  }
  NK_API void
  widget_disable_begin(context* ctx) {

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);

    if (!ctx || !ctx->current)
      return;

    window* win = ctx->current;
    style* style = &ctx->style;

    win->widgets_disabled = true;

    style->button.color_factor_text = style->button.disabled_factor;
    style->button.color_factor_background = style->button.disabled_factor;
    style->chart.color_factor = style->chart.disabled_factor;
    style->checkbox.color_factor = style->checkbox.disabled_factor;
    style->combo.color_factor = style->combo.disabled_factor;
    style->combo.button.color_factor_text = style->combo.button.disabled_factor;
    style->combo.button.color_factor_background = style->combo.button.disabled_factor;
    style->contextual_button.color_factor_text = style->contextual_button.disabled_factor;
    style->contextual_button.color_factor_background = style->contextual_button.disabled_factor;
    style->edit.color_factor = style->edit.disabled_factor;
    style->edit.scrollbar.color_factor = style->edit.scrollbar.disabled_factor;
    style->menu_button.color_factor_text = style->menu_button.disabled_factor;
    style->menu_button.color_factor_background = style->menu_button.disabled_factor;
    style->option.color_factor = style->option.disabled_factor;
    style->progress.color_factor = style->progress.disabled_factor;
    style->property.color_factor = style->property.disabled_factor;
    style->property.inc_button.color_factor_text = style->property.inc_button.disabled_factor;
    style->property.inc_button.color_factor_background = style->property.inc_button.disabled_factor;
    style->property.dec_button.color_factor_text = style->property.dec_button.disabled_factor;
    style->property.dec_button.color_factor_background = style->property.dec_button.disabled_factor;
    style->property.edit.color_factor = style->property.edit.disabled_factor;
    style->scrollh.color_factor = style->scrollh.disabled_factor;
    style->scrollh.inc_button.color_factor_text = style->scrollh.inc_button.disabled_factor;
    style->scrollh.inc_button.color_factor_background = style->scrollh.inc_button.disabled_factor;
    style->scrollh.dec_button.color_factor_text = style->scrollh.dec_button.disabled_factor;
    style->scrollh.dec_button.color_factor_background = style->scrollh.dec_button.disabled_factor;
    style->scrollv.color_factor = style->scrollv.disabled_factor;
    style->scrollv.inc_button.color_factor_text = style->scrollv.inc_button.disabled_factor;
    style->scrollv.inc_button.color_factor_background = style->scrollv.inc_button.disabled_factor;
    style->scrollv.dec_button.color_factor_text = style->scrollv.dec_button.disabled_factor;
    style->scrollv.dec_button.color_factor_background = style->scrollv.dec_button.disabled_factor;
    style->selectable.color_factor = style->selectable.disabled_factor;
    style->slider.color_factor = style->slider.disabled_factor;
    style->slider.inc_button.color_factor_text = style->slider.inc_button.disabled_factor;
    style->slider.inc_button.color_factor_background = style->slider.inc_button.disabled_factor;
    style->slider.dec_button.color_factor_text = style->slider.dec_button.disabled_factor;
    style->slider.dec_button.color_factor_background = style->slider.dec_button.disabled_factor;
    style->tab.color_factor = style->tab.disabled_factor;
    style->tab.node_maximize_button.color_factor_text = style->tab.node_maximize_button.disabled_factor;
    style->tab.node_minimize_button.color_factor_text = style->tab.node_minimize_button.disabled_factor;
    style->tab.tab_maximize_button.color_factor_text = style->tab.tab_maximize_button.disabled_factor;
    style->tab.tab_maximize_button.color_factor_background = style->tab.tab_maximize_button.disabled_factor;
    style->tab.tab_minimize_button.color_factor_text = style->tab.tab_minimize_button.disabled_factor;
    style->tab.tab_minimize_button.color_factor_background = style->tab.tab_minimize_button.disabled_factor;
    style->text.color_factor = style->text.disabled_factor;
  }
  NK_API void
  widget_disable_end(context* ctx) {

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);

    if (!ctx || !ctx->current)
      return;

    window* win = ctx->current;
    style* style = &ctx->style;

    win->widgets_disabled = false;

    style->button.color_factor_text = 1.0f;
    style->button.color_factor_background = 1.0f;
    style->chart.color_factor = 1.0f;
    style->checkbox.color_factor = 1.0f;
    style->combo.color_factor = 1.0f;
    style->combo.button.color_factor_text = 1.0f;
    style->combo.button.color_factor_background = 1.0f;
    style->contextual_button.color_factor_text = 1.0f;
    style->contextual_button.color_factor_background = 1.0f;
    style->edit.color_factor = 1.0f;
    style->edit.scrollbar.color_factor = 1.0f;
    style->menu_button.color_factor_text = 1.0f;
    style->menu_button.color_factor_background = 1.0f;
    style->option.color_factor = 1.0f;
    style->progress.color_factor = 1.0f;
    style->property.color_factor = 1.0f;
    style->property.inc_button.color_factor_text = 1.0f;
    style->property.inc_button.color_factor_background = 1.0f;
    style->property.dec_button.color_factor_text = 1.0f;
    style->property.dec_button.color_factor_background = 1.0f;
    style->property.edit.color_factor = 1.0f;
    style->scrollh.color_factor = 1.0f;
    style->scrollh.inc_button.color_factor_text = 1.0f;
    style->scrollh.inc_button.color_factor_background = 1.0f;
    style->scrollh.dec_button.color_factor_text = 1.0f;
    style->scrollh.dec_button.color_factor_background = 1.0f;
    style->scrollv.color_factor = 1.0f;
    style->scrollv.inc_button.color_factor_text = 1.0f;
    style->scrollv.inc_button.color_factor_background = 1.0f;
    style->scrollv.dec_button.color_factor_text = 1.0f;
    style->scrollv.dec_button.color_factor_background = 1.0f;
    style->selectable.color_factor = 1.0f;
    style->slider.color_factor = 1.0f;
    style->slider.inc_button.color_factor_text = 1.0f;
    style->slider.inc_button.color_factor_background = 1.0f;
    style->slider.dec_button.color_factor_text = 1.0f;
    style->slider.dec_button.color_factor_background = 1.0f;
    style->tab.color_factor = 1.0f;
    style->tab.node_maximize_button.color_factor_text = 1.0f;
    style->tab.node_minimize_button.color_factor_text = 1.0f;
    style->tab.tab_maximize_button.color_factor_text = 1.0f;
    style->tab.tab_maximize_button.color_factor_background = 1.0f;
    style->tab.tab_minimize_button.color_factor_text = 1.0f;
    style->tab.tab_minimize_button.color_factor_background = 1.0f;
    style->text.color_factor = 1.0f;
  }
} // namespace nk
