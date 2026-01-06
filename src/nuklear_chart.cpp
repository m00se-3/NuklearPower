#include <cstring>
#include <utility>
#include <nk/nuklear.hpp>

namespace nk {
  /* ==============================================================
   *
   *                          CHART
   *
   * ===============================================================*/
  NK_API bool
  chart_begin_colored(context* ctx, const chart_type type,
                      const color color, const struct color highlight,
                      const int count, float min_value, float max_value) {
    chart* chart;

    rectf bounds = {0, 0, 0, 0};

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);

    if (!ctx || !ctx->current || !ctx->current->layout)
      return 0;
    if (!widget(&bounds, ctx)) {
      chart = &ctx->current->layout->chart;
      zero(chart, sizeof(*chart));
      return 0;
    }

    window* win = ctx->current;
    const style* config = &ctx->style;
    chart = &win->layout->chart;
    const style_chart* style = &config->chart;

    /* setup basic generic chart  */
    zero(chart, sizeof(*chart));
    chart->x = bounds.x + style->padding.x;
    chart->y = bounds.y + style->padding.y;
    chart->w = bounds.w - 2 * style->padding.x;
    chart->h = bounds.h - 2 * style->padding.y;
    chart->w = std::max(chart->w, 2 * style->padding.x);
    chart->h = std::max(chart->h, 2 * style->padding.y);

    /* add first slot into chart */
    {
      chart_slot* slot = &chart->slots[chart->slot++];
      slot->type = type;
      slot->count = count;
      slot->color = rgb_factor(color, style->color_factor);
      slot->highlight = highlight;
      slot->min = std::min(min_value, max_value);
      slot->max = std::max(min_value, max_value);
      slot->range = slot->max - slot->min;
      slot->show_markers = style->show_markers;
    }

    /* draw chart background */
    const style_item* background = &style->background;

    switch (background->type) {
      case style_item_type::STYLE_ITEM_IMAGE:
        draw_image(&win->buffer, bounds, &background->data.image, rgb_factor(white, style->color_factor));
        break;
      case style_item_type::STYLE_ITEM_NINE_SLICE:
        draw_nine_slice(&win->buffer, bounds, &background->data.slice, rgb_factor(white, style->color_factor));
        break;
      case style_item_type::STYLE_ITEM_COLOR:
        fill_rect(&win->buffer, bounds, style->rounding, rgb_factor(style->border_color, style->color_factor));
        fill_rect(&win->buffer, shrirect(bounds, style->border),
                  style->rounding, rgb_factor(style->background.data.color, style->color_factor));
        break;
    }
    return 1;
  }
  NK_API bool
  chart_begin(context* ctx, const chart_type type,
              const int count, float min_value, float max_value) {
    return chart_begin_colored(ctx, type, ctx->style.chart.color,
                               ctx->style.chart.selected_color, count, min_value, max_value);
  }
  NK_API void
  chart_add_slot_colored(context* ctx, const chart_type type,
                         const color color, const struct color highlight,
                         const int count, float min_value, float max_value) {

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    NK_ASSERT(ctx->current->layout->chart.slot < NK_CHART_MAX_SLOT);
    if (!ctx || !ctx->current || !ctx->current->layout)
      return;
    if (ctx->current->layout->chart.slot >= NK_CHART_MAX_SLOT)
      return;

    const style_chart* style = &ctx->style.chart;

    /* add another slot into the graph */
    {
      chart* chart = &ctx->current->layout->chart;
      chart_slot* slot = &chart->slots[chart->slot++];
      slot->type = type;
      slot->count = count;
      slot->color = rgb_factor(color, style->color_factor);
      slot->highlight = highlight;
      slot->min = std::min(min_value, max_value);
      slot->max = std::max(min_value, max_value);
      slot->range = slot->max - slot->min;
      slot->show_markers = style->show_markers;
    }
  }
  NK_API void
  chart_add_slot(context* ctx, const chart_type type,
                 const int count, float min_value, float max_value) {
    chart_add_slot_colored(ctx, type, ctx->style.chart.color,
                           ctx->style.chart.selected_color, count, min_value, max_value);
  }
  INTERN flag
  chart_push_line(context* ctx, window* win,
                  chart* g, float value, const int slot) {
    panel* layout = win->layout;
    const input* i = ctx->current->widgets_disabled ? 0 : &ctx->input;
    command_buffer* out = &win->buffer;

    flag ret = 0;
    vec2f cur;
    rectf bounds;
    color color;

    NK_ASSERT(slot >= 0 && slot < NK_CHART_MAX_SLOT);
    float step = g->w / (float) g->slots[slot].count;
    float range = g->slots[slot].max - g->slots[slot].min;
    float ratio = (value - g->slots[slot].min) / range;

    if (g->slots[slot].index == 0) {
      /* first data point does not have a connection */
      g->slots[slot].last.x = g->x;
      g->slots[slot].last.y = (g->y + g->h) - ratio * (float) g->h;

      bounds.x = g->slots[slot].last.x - 2;
      bounds.y = g->slots[slot].last.y - 2;
      bounds.w = bounds.h = 4;

      color = g->slots[slot].color;
      if (!(layout->flags & static_cast<decltype(layout->flags)>(window_flags::WINDOW_ROM)) && i &&
          NK_INBOX(i->mouse.pos.x, i->mouse.pos.y, g->slots[slot].last.x - 3, g->slots[slot].last.y - 3, 6, 6)) {
        ret = input_is_mouse_hovering_rect(i, bounds) ? std::to_underlying(chart_event::CHART_HOVERING) : 0;
        ret |= (i->mouse.buttons[NK_BUTTON_LEFT].down &&
                i->mouse.buttons[NK_BUTTON_LEFT].clicked)
                   ? std::to_underlying(chart_event::CHART_CLICKED)
                   : 0;
        color = g->slots[slot].highlight;
      }
      if (g->slots[slot].show_markers) {
        fill_rect(out, bounds, 0, color);
      }
      g->slots[slot].index += 1;
      return ret;
    }

    /* draw a line between the last data point and the new one */
    color = g->slots[slot].color;
    cur.x = g->x + (float) (step * (float) g->slots[slot].index);
    cur.y = (g->y + g->h) - (ratio * (float) g->h);
    stroke_line(out, g->slots[slot].last.x, g->slots[slot].last.y, cur.x, cur.y, 1.0f, color);

    bounds.x = cur.x - 3;
    bounds.y = cur.y - 3;
    bounds.w = bounds.h = 6;

    /* user selection of current data point */
    if (!(layout->flags & static_cast<decltype(layout->flags)>(window_flags::WINDOW_ROM))) {
      if (input_is_mouse_hovering_rect(i, bounds)) {
        ret = std::to_underlying(chart_event::CHART_HOVERING);
        ret |= (!i->mouse.buttons[NK_BUTTON_LEFT].down &&
                i->mouse.buttons[NK_BUTTON_LEFT].clicked)
                   ? std::to_underlying(chart_event::CHART_CLICKED)
                   : 0;
        color = g->slots[slot].highlight;
      }
    }
    if (g->slots[slot].show_markers) {
      fill_rect(out, rect(cur.x - 2, cur.y - 2, 4, 4), 0, color);
    }

    /* save current data point position */
    g->slots[slot].last.x = cur.x;
    g->slots[slot].last.y = cur.y;
    g->slots[slot].index += 1;
    return ret;
  }
  INTERN flag
  chart_push_column(const context* ctx, window* win,
                    chart* chart, float value, const int slot) {
    command_buffer* out = &win->buffer;
    const input* in = ctx->current->widgets_disabled ? 0 : &ctx->input;
    panel* layout = win->layout;

    float ratio;
    flag ret = 0;
    rectf item = {0, 0, 0, 0};

    NK_ASSERT(slot >= 0 && slot < NK_CHART_MAX_SLOT);
    if (chart->slots[slot].index >= chart->slots[slot].count)
      return false;
    if (chart->slots[slot].count) {
      float padding = (float) (chart->slots[slot].count - 1);
      item.w = (chart->w - padding) / (float) (chart->slots[slot].count);
    }

    /* calculate bounds of current bar chart entry */
    color color = chart->slots[slot].color;
    ;
    item.h = chart->h * NK_ABS((value / chart->slots[slot].range));
    if (value >= 0) {
      ratio = (value + NK_ABS(chart->slots[slot].min)) / NK_ABS(chart->slots[slot].range);
      item.y = (chart->y + chart->h) - chart->h * ratio;
    } else {
      ratio = (value - chart->slots[slot].max) / chart->slots[slot].range;
      item.y = chart->y + (chart->h * NK_ABS(ratio)) - item.h;
    }
    item.x = chart->x + ((float) chart->slots[slot].index * item.w);
    item.x = item.x + ((float) chart->slots[slot].index);

    /* user chart bar selection */
    if (!(layout->flags & static_cast<decltype(layout->flags)>(window_flags::WINDOW_ROM)) && in &&
        NK_INBOX(in->mouse.pos.x, in->mouse.pos.y, item.x, item.y, item.w, item.h)) {
      ret = std::to_underlying(chart_event::CHART_HOVERING);
      ret |= (!in->mouse.buttons[NK_BUTTON_LEFT].down &&
              in->mouse.buttons[NK_BUTTON_LEFT].clicked)
                 ? std::to_underlying(chart_event::CHART_CLICKED)
                 : 0;
      color = chart->slots[slot].highlight;
    }
    fill_rect(out, item, 0, color);
    chart->slots[slot].index += 1;
    return ret;
  }
  NK_API flag
  chart_push_slot(context* ctx, float value, const int slot) {
    flag flags;

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(slot >= 0 && slot < NK_CHART_MAX_SLOT);
    NK_ASSERT(slot < ctx->current->layout->chart.slot);
    if (!ctx || !ctx->current || slot >= NK_CHART_MAX_SLOT)
      return false;
    if (slot >= ctx->current->layout->chart.slot)
      return false;

    window* win = ctx->current;
    if (win->layout->chart.slot < slot)
      return false;
    switch (win->layout->chart.slots[slot].type) {
      case chart_type::CHART_LINES:
        flags = chart_push_line(ctx, win, &win->layout->chart, value, slot);
        break;
      case chart_type::CHART_COLUMN:
        flags = chart_push_column(ctx, win, &win->layout->chart, value, slot);
        break;
      default:
      case chart_type::CHART_MAX:
        flags = 0;
    }
    return flags;
  }
  NK_API flag
  chart_push(context* ctx, float value) {
    return chart_push_slot(ctx, value, 0);
  }
  NK_API void
  chart_end(context* ctx) {

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    if (!ctx || !ctx->current)
      return;

    const window* win = ctx->current;
    chart* chart = &win->layout->chart;
    std::memcpy(chart, 0, sizeof(*chart));
    return;
  }
  NK_API void
  plot(context* ctx, const chart_type type, const float* values,
       const int count, const int offset) {
    int i = 0;

    NK_ASSERT(ctx);
    NK_ASSERT(values);
    if (!ctx || !values || !count)
      return;

    float min_value = values[offset];
    float max_value = values[offset];
    for (i = 0; i < count; ++i) {
      min_value = std::min(values[i + offset], min_value);
      max_value = std::max(values[i + offset], max_value);
    }

    if (chart_begin(ctx, type, count, min_value, max_value)) {
      for (i = 0; i < count; ++i)
        chart_push(ctx, values[i + offset]);
      chart_end(ctx);
    }
  }
  NK_API void
  plot_function(context* ctx, const chart_type type, void* userdata,
                float (*value_getter)(void* user, int index), const int count, const int offset) {
    int i = 0;
    float min_value;

    NK_ASSERT(ctx);
    NK_ASSERT(value_getter);
    if (!ctx || !value_getter || !count)
      return;

    float max_value = min_value = value_getter(userdata, offset);
    for (i = 0; i < count; ++i) {
      float value = value_getter(userdata, i + offset);
      min_value = std::min(value, min_value);
      max_value = std::max(value, max_value);
    }

    if (chart_begin(ctx, type, count, min_value, max_value)) {
      for (i = 0; i < count; ++i)
        chart_push(ctx, value_getter(userdata, i + offset));
      chart_end(ctx);
    }
  }
} // namespace nk
