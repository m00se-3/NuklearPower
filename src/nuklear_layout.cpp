#include <nk/nuklear.hpp>
#include <algorithm>

namespace nk {
  /* ===============================================================
   *
   *                          LAYOUT
   *
   * ===============================================================*/
  NK_API void
  layout_set_min_row_height(context* ctx, float height) {

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout)
      return;

    const window* win = ctx->current;
    panel* layout = win->layout;
    layout->row.min_height = height;
  }
  NK_API void
  layout_reset_min_row_height(context* ctx) {

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout)
      return;

    const window* win = ctx->current;
    panel* layout = win->layout;
    layout->row.min_height = ctx->style.font->height;
    layout->row.min_height += ctx->style.text.padding.y * 2;
    layout->row.min_height += ctx->style.window.min_row_height_padding * 2;
  }
  NK_LIB float
  layout_row_calculate_usable_space(const style* style, const panel_type::value_type type,
                                    float total_space, const int columns) {

    NK_UNUSED(type);

    const vec2f spacing = style->window.spacing;

    /* calculate the usable panel space */
    float panel_spacing = (float) std::max(columns - 1, 0) * spacing.x;
    float panel_space = total_space - panel_spacing;
    return panel_space;
  }
  NK_LIB void
  panel_layout(const context* ctx, window* win,
               float height, const int cols) {

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout)
      return;

    /* prefetch some configuration data */
    panel* layout = win->layout;
    const style* style = &ctx->style;
    command_buffer* out = &win->buffer;
    const color color = style->window.background;
    const vec2f item_spacing = style->window.spacing;

    /*  if one of these triggers you forgot to add an `if` condition around either
        a window, group, popup, combobox or contextual menu `begin` and `end` block.
        Example:
            if (begin(...) {...} end(...); or
            if (group_begin(...) { group_end(...);} */
    NK_ASSERT(!(layout->flags & window_flags::WINDOW_MINIMIZED));
    NK_ASSERT(!(layout->flags & window_flags::WINDOW_HIDDEN));
    NK_ASSERT(!(layout->flags & window_flags::WINDOW_CLOSED));

    /* update the current row and set the current row layout */
    layout->row.index = 0;
    layout->at_y += layout->row.height;
    layout->row.columns = cols;
    if (height == 0.0f)
      layout->row.height = std::max(height, layout->row.min_height) + item_spacing.y;
    else
      layout->row.height = height + item_spacing.y;

    layout->row.item_offset = 0;
    if (layout->flags & window_flags::WINDOW_DYNAMIC) {
      /* draw background for dynamic panels */
      rectf background;
      background.x = win->bounds.x;
      background.w = win->bounds.w;
      background.y = layout->at_y - 1.0f;
      background.h = layout->row.height + 1.0f;
      fill_rect(out, background, 0, color);
    }
  }
  NK_LIB void
  row_layout(context* ctx, const layout_format fmt,
             float height, const int cols, const int width) {
    /* update the current row and set the current row layout */
    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout)
      return;

    window* win = ctx->current;
    panel_layout(ctx, win, height, cols);
    if (fmt == layout_format::DYNAMIC)
      win->layout->row.type = panel_row_layout_type::LAYOUT_DYNAMIC_FIXED;
    else
      win->layout->row.type = panel_row_layout_type::LAYOUT_STATIC_FIXED;

    win->layout->row.ratio = 0;
    win->layout->row.filled = 0;
    win->layout->row.item_offset = 0;
    win->layout->row.item_width = (float) width;
  }
  NK_API float
  layout_ratio_from_pixel(const context* ctx, float pixel_width) {
    NK_ASSERT(ctx);
    NK_ASSERT(pixel_width);
    if (!ctx || !ctx->current || !ctx->current->layout)
      return 0;
    const window* win = ctx->current;
    return std::clamp(0.0f, pixel_width / win->bounds.x, 1.0f);
  }
  NK_API void
  layout_row_dynamic(context* ctx, float height, const int cols) {
    row_layout(ctx, layout_format::DYNAMIC, height, cols, 0);
  }
  NK_API void
  layout_row_static(context* ctx, float height, const int item_width, const int cols) {
    row_layout(ctx, layout_format::STATIC, height, cols, item_width);
  }
  NK_API void
  layout_row_begin(context* ctx, const layout_format fmt,
                   float row_height, const int cols) {

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout)
      return;

    window* win = ctx->current;
    panel* layout = win->layout;
    panel_layout(ctx, win, row_height, cols);
    if (fmt == layout_format::DYNAMIC)
      layout->row.type = panel_row_layout_type::LAYOUT_DYNAMIC_ROW;
    else
      layout->row.type = panel_row_layout_type::LAYOUT_STATIC_ROW;

    layout->row.ratio = 0;
    layout->row.filled = 0;
    layout->row.item_width = 0;
    layout->row.item_offset = 0;
    layout->row.columns = cols;
  }
  NK_API void
  layout_row_push(context* ctx, float ratio_or_width) {

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout)
      return;

    const window* win = ctx->current;
    panel* layout = win->layout;
    NK_ASSERT(layout->row.type == panel_row_layout_type::LAYOUT_STATIC_ROW || layout->row.type == panel_row_layout_type::LAYOUT_DYNAMIC_ROW);
    if (layout->row.type != panel_row_layout_type::LAYOUT_STATIC_ROW && layout->row.type != panel_row_layout_type::LAYOUT_DYNAMIC_ROW)
      return;

    if (layout->row.type == panel_row_layout_type::LAYOUT_DYNAMIC_ROW) {
      float ratio = ratio_or_width;
      if ((ratio + layout->row.filled) > 1.0f)
        return;
      if (ratio > 0.0f)
        layout->row.item_width = NK_SATURATE(ratio);
      else
        layout->row.item_width = 1.0f - layout->row.filled;
    } else
      layout->row.item_width = ratio_or_width;
  }
  NK_API void
  layout_row_end(context* ctx) {

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout)
      return;

    const window* win = ctx->current;
    panel* layout = win->layout;
    NK_ASSERT(layout->row.type == panel_row_layout_type::LAYOUT_STATIC_ROW || layout->row.type == panel_row_layout_type::LAYOUT_DYNAMIC_ROW);
    if (layout->row.type != panel_row_layout_type::LAYOUT_STATIC_ROW && layout->row.type != panel_row_layout_type::LAYOUT_DYNAMIC_ROW)
      return;
    layout->row.item_width = 0;
    layout->row.item_offset = 0;
  }
  NK_API void
  layout_row(context* ctx, const layout_format fmt,
             float height, const int cols, const float* ratio) {
    int n_undef = 0;

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout)
      return;

    window* win = ctx->current;
    panel* layout = win->layout;
    panel_layout(ctx, win, height, cols);
    if (fmt == layout_format::DYNAMIC) {
      /* calculate width of undefined widget ratios */
      float r = 0;
      layout->row.ratio = ratio;
      for (int i = 0; i < cols; ++i) {
        if (ratio[i] < 0.0f)
          n_undef++;
        else
          r += ratio[i];
      }
      r = NK_SATURATE(1.0f - r);
      layout->row.type = panel_row_layout_type::LAYOUT_DYNAMIC;
      layout->row.item_width = (r > 0 && n_undef > 0) ? (r / (float) n_undef) : 0;
    } else {
      layout->row.ratio = ratio;
      layout->row.type = panel_row_layout_type::LAYOUT_STATIC;
      layout->row.item_width = 0;
      layout->row.item_offset = 0;
    }
    layout->row.item_offset = 0;
    layout->row.filled = 0;
  }
  NK_API void
  layout_row_template_begin(context* ctx, float height) {

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout)
      return;

    window* win = ctx->current;
    panel* layout = win->layout;
    panel_layout(ctx, win, height, 1);
    layout->row.type = panel_row_layout_type::LAYOUT_TEMPLATE;
    layout->row.columns = 0;
    layout->row.ratio = 0;
    layout->row.item_width = 0;
    layout->row.item_height = 0;
    layout->row.item_offset = 0;
    layout->row.filled = 0;
    layout->row.item.x = 0;
    layout->row.item.y = 0;
    layout->row.item.w = 0;
    layout->row.item.h = 0;
  }
  NK_API void
  layout_row_template_push_dynamic(context* ctx) {

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout)
      return;

    const window* win = ctx->current;
    panel* layout = win->layout;
    NK_ASSERT(layout->row.type == panel_row_layout_type::LAYOUT_TEMPLATE);
    NK_ASSERT(layout->row.columns < NK_MAX_LAYOUT_ROW_TEMPLATE_COLUMNS);
    if (layout->row.type != panel_row_layout_type::LAYOUT_TEMPLATE)
      return;
    if (layout->row.columns >= NK_MAX_LAYOUT_ROW_TEMPLATE_COLUMNS)
      return;
    layout->row.templates[layout->row.columns++] = -1.0f;
  }
  NK_API void
  layout_row_template_push_variable(context* ctx, float min_width) {

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout)
      return;

    const window* win = ctx->current;
    panel* layout = win->layout;
    NK_ASSERT(layout->row.type == panel_row_layout_type::LAYOUT_TEMPLATE);
    NK_ASSERT(layout->row.columns < NK_MAX_LAYOUT_ROW_TEMPLATE_COLUMNS);
    if (layout->row.type != panel_row_layout_type::LAYOUT_TEMPLATE)
      return;
    if (layout->row.columns >= NK_MAX_LAYOUT_ROW_TEMPLATE_COLUMNS)
      return;
    layout->row.templates[layout->row.columns++] = -min_width;
  }
  NK_API void
  layout_row_template_push_static(context* ctx, float width) {

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout)
      return;

    const window* win = ctx->current;
    panel* layout = win->layout;
    NK_ASSERT(layout->row.type == panel_row_layout_type::LAYOUT_TEMPLATE);
    NK_ASSERT(layout->row.columns < NK_MAX_LAYOUT_ROW_TEMPLATE_COLUMNS);
    if (layout->row.type != panel_row_layout_type::LAYOUT_TEMPLATE)
      return;
    if (layout->row.columns >= NK_MAX_LAYOUT_ROW_TEMPLATE_COLUMNS)
      return;
    layout->row.templates[layout->row.columns++] = width;
  }
  NK_API void
  layout_row_template_end(context* ctx) {

    int i = 0;
    int variable_count = 0;
    int min_variable_count = 0;
    float min_fixed_width = 0.0f;
    float total_fixed_width = 0.0f;
    float max_variable_width = 0.0f;

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout)
      return;

    const window* win = ctx->current;
    panel* layout = win->layout;
    NK_ASSERT(layout->row.type == panel_row_layout_type::LAYOUT_TEMPLATE);
    if (layout->row.type != panel_row_layout_type::LAYOUT_TEMPLATE)
      return;
    for (i = 0; i < layout->row.columns; ++i) {
      float width = layout->row.templates[i];
      if (width >= 0.0f) {
        total_fixed_width += width;
        min_fixed_width += width;
      } else if (width < -1.0f) {
        width = -width;
        total_fixed_width += width;
        max_variable_width = std::max(max_variable_width, width);
        variable_count++;
      } else {
        min_variable_count++;
        variable_count++;
      }
    }
    if (variable_count) {
      float space = layout_row_calculate_usable_space(&ctx->style, layout->type,
                                                      layout->bounds.w, layout->row.columns);
      float var_width = (std::max(space - min_fixed_width, 0.0f)) / (float) variable_count;
      const int enough_space = var_width >= max_variable_width;
      if (!enough_space)
        var_width = (std::max(space - total_fixed_width, 0.0f)) / (float) min_variable_count;
      for (i = 0; i < layout->row.columns; ++i) {
        float* width = &layout->row.templates[i];
        *width = (*width >= 0.0f) ? *width : (*width < -1.0f && !enough_space) ? -(*width)
                                                                               : var_width;
      }
    }
  }
  NK_API void
  layout_space_begin(context* ctx, const layout_format fmt,
                     float height, const int widget_count) {

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout)
      return;

    window* win = ctx->current;
    panel* layout = win->layout;
    panel_layout(ctx, win, height, widget_count);
    if (fmt == layout_format::STATIC)
      layout->row.type = panel_row_layout_type::LAYOUT_STATIC_FREE;
    else
      layout->row.type = panel_row_layout_type::LAYOUT_DYNAMIC_FREE;

    layout->row.ratio = 0;
    layout->row.filled = 0;
    layout->row.item_width = 0;
    layout->row.item_offset = 0;
  }
  NK_API void
  layout_space_end(context* ctx) {

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout)
      return;

    const window* win = ctx->current;
    panel* layout = win->layout;
    layout->row.item_width = 0;
    layout->row.item_height = 0;
    layout->row.item_offset = 0;
    zero(&layout->row.item, sizeof(layout->row.item));
  }
  NK_API void
  layout_space_push(context* ctx, const rectf rect) {

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout)
      return;

    const window* win = ctx->current;
    panel* layout = win->layout;
    layout->row.item = rect;
  }
  NK_API rectf
  layout_space_bounds(const context* ctx) {
    rectf ret;

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    const window* win = ctx->current;
    const panel* layout = win->layout;

    ret.x = layout->clip.x;
    ret.y = layout->clip.y;
    ret.w = layout->clip.w;
    ret.h = layout->row.height;
    return ret;
  }
  NK_API rectf
  layout_widget_bounds(const context* ctx) {
    rectf ret;

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    const window* win = ctx->current;
    const panel* layout = win->layout;

    ret.x = layout->at_x;
    ret.y = layout->at_y;
    ret.w = layout->bounds.w - std::max(layout->at_x - layout->bounds.x, 0.0f);
    ret.h = layout->row.height;
    return ret;
  }
  NK_API vec2f
  layout_space_to_screen(const context* ctx, vec2f ret) {

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    const window* win = ctx->current;
    const panel* layout = win->layout;

    ret.x += layout->at_x - (float) *layout->offset_x;
    ret.y += layout->at_y - (float) *layout->offset_y;
    return ret;
  }
  NK_API vec2f
  layout_space_to_local(const context* ctx, vec2f ret) {

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    const window* win = ctx->current;
    const panel* layout = win->layout;

    ret.x += -layout->at_x + (float) *layout->offset_x;
    ret.y += -layout->at_y + (float) *layout->offset_y;
    return ret;
  }
  NK_API rectf
  layout_space_rect_to_screen(const context* ctx, rectf ret) {

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    const window* win = ctx->current;
    const panel* layout = win->layout;

    ret.x += layout->at_x - (float) *layout->offset_x;
    ret.y += layout->at_y - (float) *layout->offset_y;
    return ret;
  }
  NK_API rectf
  layout_space_rect_to_local(const context* ctx, rectf ret) {

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    const window* win = ctx->current;
    const panel* layout = win->layout;

    ret.x += -layout->at_x + (float) *layout->offset_x;
    ret.y += -layout->at_y + (float) *layout->offset_y;
    return ret;
  }
  NK_LIB void
  panel_alloc_row(const context* ctx, window* win) {
    const panel* layout = win->layout;
    const vec2f spacing = ctx->style.window.spacing;
    const float row_height = layout->row.height - spacing.y;
    panel_layout(ctx, win, row_height, layout->row.columns);
  }
  NK_LIB void
  layout_widget_space(rectf* bounds, const context* ctx,
                      window* win, const int modify) {

    float item_offset = 0;
    float item_width = 0;
    float item_spacing = 0;
    float panel_space = 0;

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout)
      return;

    win = ctx->current;
    panel* layout = win->layout;
    const style* style = &ctx->style;
    NK_ASSERT(bounds);

    const vec2f spacing = style->window.spacing;
    panel_space = layout_row_calculate_usable_space(&ctx->style, layout->type,
                                                    layout->bounds.w, layout->row.columns);

#define NK_FRAC(x) (x - (float) (int) roundf(x)) /* will be used to remove fookin gaps */
    /* calculate the width of one item inside the current layout space */
    switch (layout->row.type) {
      case panel_row_layout_type::LAYOUT_DYNAMIC_FIXED: {
        /* scaling fixed size widgets item width */
        float w = std::max(1.0f, panel_space) / (float) layout->row.columns;
        item_offset = (float) layout->row.index * w;
        item_width = w + NK_FRAC(item_offset);
        item_spacing = (float) layout->row.index * spacing.x;
      } break;
      case panel_row_layout_type::LAYOUT_DYNAMIC_ROW: {
        /* scaling single ratio widget width */
        float w = layout->row.item_width * panel_space;
        item_offset = layout->row.item_offset;
        item_width = w + NK_FRAC(item_offset);
        item_spacing = 0;

        if (modify) {
          layout->row.item_offset += w + spacing.x;
          layout->row.filled += layout->row.item_width;
          layout->row.index = 0;
        }
      } break;
      case panel_row_layout_type::LAYOUT_DYNAMIC_FREE: {
        /* panel width depended free widget placing */
        bounds->x = layout->at_x + (layout->bounds.w * layout->row.item.x);
        bounds->x -= (float) *layout->offset_x;
        bounds->y = layout->at_y + (layout->row.height * layout->row.item.y);
        bounds->y -= (float) *layout->offset_y;
        bounds->w = layout->bounds.w * layout->row.item.w + NK_FRAC(bounds->x);
        bounds->h = layout->row.height * layout->row.item.h + NK_FRAC(bounds->y);
        return;
      }
      case panel_row_layout_type::LAYOUT_DYNAMIC: {
        /* scaling arrays of panel width ratios for every widget */
        NK_ASSERT(layout->row.ratio);
        float ratio = (layout->row.ratio[layout->row.index] < 0) ? layout->row.item_width : layout->row.ratio[layout->row.index];

        float w = (ratio * panel_space);
        item_spacing = (float) layout->row.index * spacing.x;
        item_offset = layout->row.item_offset;
        item_width = w + NK_FRAC(item_offset);

        if (modify) {
          layout->row.item_offset += w;
          layout->row.filled += ratio;
        }
      } break;
      case panel_row_layout_type::LAYOUT_STATIC_FIXED: {
        /* non-scaling fixed widgets item width */
        item_width = layout->row.item_width;
        item_offset = (float) layout->row.index * item_width;
        item_spacing = (float) layout->row.index * spacing.x;
      } break;
      case panel_row_layout_type::LAYOUT_STATIC_ROW: {
        /* scaling single ratio widget width */
        item_width = layout->row.item_width;
        item_offset = layout->row.item_offset;
        item_spacing = (float) layout->row.index * spacing.x;
        if (modify)
          layout->row.item_offset += item_width;
      } break;
      case panel_row_layout_type::LAYOUT_STATIC_FREE: {
        /* free widget placing */
        bounds->x = layout->at_x + layout->row.item.x;
        bounds->w = layout->row.item.w;
        if (((bounds->x + bounds->w) > layout->max_x) && modify)
          layout->max_x = (bounds->x + bounds->w);
        bounds->x -= (float) *layout->offset_x;
        bounds->y = layout->at_y + layout->row.item.y;
        bounds->y -= (float) *layout->offset_y;
        bounds->h = layout->row.item.h;
        return;
      }
      case panel_row_layout_type::LAYOUT_STATIC: {
        /* non-scaling array of panel pixel width for every widget */
        item_spacing = (float) layout->row.index * spacing.x;
        item_width = layout->row.ratio[layout->row.index];
        item_offset = layout->row.item_offset;
        if (modify)
          layout->row.item_offset += item_width;
      } break;
      case panel_row_layout_type::LAYOUT_TEMPLATE: {
        /* stretchy row layout with combined dynamic/static widget width*/
        NK_ASSERT(layout->row.index < layout->row.columns);
        NK_ASSERT(layout->row.index < NK_MAX_LAYOUT_ROW_TEMPLATE_COLUMNS);
        float w = layout->row.templates[layout->row.index];
        item_offset = layout->row.item_offset;
        item_width = w + NK_FRAC(item_offset);
        item_spacing = (float) layout->row.index * spacing.x;
        if (modify)
          layout->row.item_offset += w;
      } break;
#undef NK_FRAC
      default:
        NK_ASSERT(0);
        break;
    };

    /* set the bounds of the newly allocated widget */
    bounds->w = item_width;
    bounds->h = layout->row.height - spacing.y;
    bounds->y = layout->at_y - (float) *layout->offset_y;
    bounds->x = layout->at_x + item_offset + item_spacing;
    if (((bounds->x + bounds->w) > layout->max_x) && modify)
      layout->max_x = bounds->x + bounds->w;
    bounds->x -= (float) *layout->offset_x;
  }
  NK_LIB void
  panel_alloc_space(rectf* bounds, const context* ctx) {

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout)
      return;

    /* check if the end of the row has been hit and begin new row if so */
    window* win = ctx->current;
    panel* layout = win->layout;
    if (layout->row.index >= layout->row.columns)
      panel_alloc_row(ctx, win);

    /* calculate widget position and size */
    layout_widget_space(bounds, ctx, win, true);
    layout->row.index++;
  }
  NK_LIB void
  layout_peek(rectf* bounds, const context* ctx) {

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout) {
      *bounds = rect(0, 0, 0, 0);
      return;
    }

    window* win = ctx->current;
    panel* layout = win->layout;
    float y = layout->at_y;
    const int index = layout->row.index;
    if (layout->row.index >= layout->row.columns) {
      layout->at_y += layout->row.height;
      layout->row.index = 0;
    }
    layout_widget_space(bounds, ctx, win, false);
    if (!layout->row.index) {
      bounds->x -= layout->row.item_offset;
    }
    layout->at_y = y;
    layout->row.index = index;
  }
  NK_API void
  spacer(context* ctx) {
    rectf dummy_rect = {0, 0, 0, 0};
    panel_alloc_space(&dummy_rect, ctx);
  }
} // namespace nk
