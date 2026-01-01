#include "nuklear.h"
#include "nuklear_internal.h"

namespace nk {
  /* ===============================================================
   *
   *                          LAYOUT
   *
   * ===============================================================*/
  NK_API void
  layout_set_min_row_height(struct context* ctx, float height) {
    struct window* win;
    struct panel* layout;

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout)
      return;

    win = ctx->current;
    layout = win->layout;
    layout->row.min_height = height;
  }
  NK_API void
  layout_reset_min_row_height(struct context* ctx) {
    struct window* win;
    struct panel* layout;

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout)
      return;

    win = ctx->current;
    layout = win->layout;
    layout->row.min_height = ctx->style.font->height;
    layout->row.min_height += ctx->style.text.padding.y * 2;
    layout->row.min_height += ctx->style.window.min_row_height_padding * 2;
  }
  NK_LIB float
  layout_row_calculate_usable_space(const struct style* style, panel_type::value_type type,
                                    float total_space, int columns) {
    float panel_spacing;
    float panel_space;

    struct vec2f spacing;

    NK_UNUSED(type);

    spacing = style->window.spacing;

    /* calculate the usable panel space */
    panel_spacing = (float) NK_MAX(columns - 1, 0) * spacing.x;
    panel_space = total_space - panel_spacing;
    return panel_space;
  }
  NK_LIB void
  panel_layout(const struct context* ctx, struct window* win,
               float height, int cols) {
    struct panel* layout;
    const struct style* style;
    struct command_buffer* out;

    struct vec2f item_spacing;
    struct color color;

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout)
      return;

    /* prefetch some configuration data */
    layout = win->layout;
    style = &ctx->style;
    out = &win->buffer;
    color = style->window.background;
    item_spacing = style->window.spacing;

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
      layout->row.height = NK_MAX(height, layout->row.min_height) + item_spacing.y;
    else
      layout->row.height = height + item_spacing.y;

    layout->row.item_offset = 0;
    if (layout->flags & window_flags::WINDOW_DYNAMIC) {
      /* draw background for dynamic panels */
      struct rectf background;
      background.x = win->bounds.x;
      background.w = win->bounds.w;
      background.y = layout->at_y - 1.0f;
      background.h = layout->row.height + 1.0f;
      fill_rect(out, background, 0, color);
    }
  }
  NK_LIB void
  row_layout(struct context* ctx, enum layout_format fmt,
             float height, int cols, int width) {
    /* update the current row and set the current row layout */
    struct window* win;
    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout)
      return;

    win = ctx->current;
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
  layout_ratio_from_pixel(const struct context* ctx, float pixel_width) {
    struct window* win;
    NK_ASSERT(ctx);
    NK_ASSERT(pixel_width);
    if (!ctx || !ctx->current || !ctx->current->layout)
      return 0;
    win = ctx->current;
    return NK_CLAMP(0.0f, pixel_width / win->bounds.x, 1.0f);
  }
  NK_API void
  layout_row_dynamic(struct context* ctx, float height, int cols) {
    row_layout(ctx, layout_format::DYNAMIC, height, cols, 0);
  }
  NK_API void
  layout_row_static(struct context* ctx, float height, int item_width, int cols) {
    row_layout(ctx, layout_format::STATIC, height, cols, item_width);
  }
  NK_API void
  layout_row_begin(struct context* ctx, enum layout_format fmt,
                   float row_height, int cols) {
    struct window* win;
    struct panel* layout;

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout)
      return;

    win = ctx->current;
    layout = win->layout;
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
  layout_row_push(struct context* ctx, float ratio_or_width) {
    struct window* win;
    struct panel* layout;

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout)
      return;

    win = ctx->current;
    layout = win->layout;
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
  layout_row_end(struct context* ctx) {
    struct window* win;
    struct panel* layout;

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout)
      return;

    win = ctx->current;
    layout = win->layout;
    NK_ASSERT(layout->row.type == panel_row_layout_type::LAYOUT_STATIC_ROW || layout->row.type == panel_row_layout_type::LAYOUT_DYNAMIC_ROW);
    if (layout->row.type != panel_row_layout_type::LAYOUT_STATIC_ROW && layout->row.type != panel_row_layout_type::LAYOUT_DYNAMIC_ROW)
      return;
    layout->row.item_width = 0;
    layout->row.item_offset = 0;
  }
  NK_API void
  layout_row(struct context* ctx, enum layout_format fmt,
             float height, int cols, const float* ratio) {
    int i;
    int n_undef = 0;
    struct window* win;
    struct panel* layout;

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout)
      return;

    win = ctx->current;
    layout = win->layout;
    panel_layout(ctx, win, height, cols);
    if (fmt == layout_format::DYNAMIC) {
      /* calculate width of undefined widget ratios */
      float r = 0;
      layout->row.ratio = ratio;
      for (i = 0; i < cols; ++i) {
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
  layout_row_template_begin(struct context* ctx, float height) {
    struct window* win;
    struct panel* layout;

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout)
      return;

    win = ctx->current;
    layout = win->layout;
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
  layout_row_template_push_dynamic(struct context* ctx) {
    struct window* win;
    struct panel* layout;

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout)
      return;

    win = ctx->current;
    layout = win->layout;
    NK_ASSERT(layout->row.type == panel_row_layout_type::LAYOUT_TEMPLATE);
    NK_ASSERT(layout->row.columns < NK_MAX_LAYOUT_ROW_TEMPLATE_COLUMNS);
    if (layout->row.type != panel_row_layout_type::LAYOUT_TEMPLATE)
      return;
    if (layout->row.columns >= NK_MAX_LAYOUT_ROW_TEMPLATE_COLUMNS)
      return;
    layout->row.templates[layout->row.columns++] = -1.0f;
  }
  NK_API void
  layout_row_template_push_variable(struct context* ctx, float min_width) {
    struct window* win;
    struct panel* layout;

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout)
      return;

    win = ctx->current;
    layout = win->layout;
    NK_ASSERT(layout->row.type == panel_row_layout_type::LAYOUT_TEMPLATE);
    NK_ASSERT(layout->row.columns < NK_MAX_LAYOUT_ROW_TEMPLATE_COLUMNS);
    if (layout->row.type != panel_row_layout_type::LAYOUT_TEMPLATE)
      return;
    if (layout->row.columns >= NK_MAX_LAYOUT_ROW_TEMPLATE_COLUMNS)
      return;
    layout->row.templates[layout->row.columns++] = -min_width;
  }
  NK_API void
  layout_row_template_push_static(struct context* ctx, float width) {
    struct window* win;
    struct panel* layout;

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout)
      return;

    win = ctx->current;
    layout = win->layout;
    NK_ASSERT(layout->row.type == panel_row_layout_type::LAYOUT_TEMPLATE);
    NK_ASSERT(layout->row.columns < NK_MAX_LAYOUT_ROW_TEMPLATE_COLUMNS);
    if (layout->row.type != panel_row_layout_type::LAYOUT_TEMPLATE)
      return;
    if (layout->row.columns >= NK_MAX_LAYOUT_ROW_TEMPLATE_COLUMNS)
      return;
    layout->row.templates[layout->row.columns++] = width;
  }
  NK_API void
  layout_row_template_end(struct context* ctx) {
    struct window* win;
    struct panel* layout;

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

    win = ctx->current;
    layout = win->layout;
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
        max_variable_width = NK_MAX(max_variable_width, width);
        variable_count++;
      } else {
        min_variable_count++;
        variable_count++;
      }
    }
    if (variable_count) {
      float space = layout_row_calculate_usable_space(&ctx->style, layout->type,
                                                      layout->bounds.w, layout->row.columns);
      float var_width = (NK_MAX(space - min_fixed_width, 0.0f)) / (float) variable_count;
      int enough_space = var_width >= max_variable_width;
      if (!enough_space)
        var_width = (NK_MAX(space - total_fixed_width, 0)) / (float) min_variable_count;
      for (i = 0; i < layout->row.columns; ++i) {
        float* width = &layout->row.templates[i];
        *width = (*width >= 0.0f) ? *width : (*width < -1.0f && !enough_space) ? -(*width)
                                                                               : var_width;
      }
    }
  }
  NK_API void
  layout_space_begin(struct context* ctx, enum layout_format fmt,
                     float height, int widget_count) {
    struct window* win;
    struct panel* layout;

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout)
      return;

    win = ctx->current;
    layout = win->layout;
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
  layout_space_end(struct context* ctx) {
    struct window* win;
    struct panel* layout;

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout)
      return;

    win = ctx->current;
    layout = win->layout;
    layout->row.item_width = 0;
    layout->row.item_height = 0;
    layout->row.item_offset = 0;
    zero(&layout->row.item, sizeof(layout->row.item));
  }
  NK_API void
  layout_space_push(struct context* ctx, struct rectf rect) {
    struct window* win;
    struct panel* layout;

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout)
      return;

    win = ctx->current;
    layout = win->layout;
    layout->row.item = rect;
  }
  NK_API struct rectf
  layout_space_bounds(const struct context* ctx) {
    struct rectf ret;
    struct window* win;
    struct panel* layout;

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    win = ctx->current;
    layout = win->layout;

    ret.x = layout->clip.x;
    ret.y = layout->clip.y;
    ret.w = layout->clip.w;
    ret.h = layout->row.height;
    return ret;
  }
  NK_API struct rectf
  layout_widget_bounds(const struct context* ctx) {
    struct rectf ret;
    struct window* win;
    struct panel* layout;

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    win = ctx->current;
    layout = win->layout;

    ret.x = layout->at_x;
    ret.y = layout->at_y;
    ret.w = layout->bounds.w - NK_MAX(layout->at_x - layout->bounds.x, 0);
    ret.h = layout->row.height;
    return ret;
  }
  NK_API struct vec2f
  layout_space_to_screen(const struct context* ctx, struct vec2f ret) {
    struct window* win;
    struct panel* layout;

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    win = ctx->current;
    layout = win->layout;

    ret.x += layout->at_x - (float) *layout->offset_x;
    ret.y += layout->at_y - (float) *layout->offset_y;
    return ret;
  }
  NK_API struct vec2f
  layout_space_to_local(const struct context* ctx, struct vec2f ret) {
    struct window* win;
    struct panel* layout;

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    win = ctx->current;
    layout = win->layout;

    ret.x += -layout->at_x + (float) *layout->offset_x;
    ret.y += -layout->at_y + (float) *layout->offset_y;
    return ret;
  }
  NK_API struct rectf
  layout_space_rect_to_screen(const struct context* ctx, struct rectf ret) {
    struct window* win;
    struct panel* layout;

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    win = ctx->current;
    layout = win->layout;

    ret.x += layout->at_x - (float) *layout->offset_x;
    ret.y += layout->at_y - (float) *layout->offset_y;
    return ret;
  }
  NK_API struct rectf
  layout_space_rect_to_local(const struct context* ctx, struct rectf ret) {
    struct window* win;
    struct panel* layout;

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    win = ctx->current;
    layout = win->layout;

    ret.x += -layout->at_x + (float) *layout->offset_x;
    ret.y += -layout->at_y + (float) *layout->offset_y;
    return ret;
  }
  NK_LIB void
  panel_alloc_row(const struct context* ctx, struct window* win) {
    struct panel* layout = win->layout;
    struct vec2f spacing = ctx->style.window.spacing;
    const float row_height = layout->row.height - spacing.y;
    panel_layout(ctx, win, row_height, layout->row.columns);
  }
  NK_LIB void
  layout_widget_space(struct rectf* bounds, const struct context* ctx,
                      struct window* win, int modify) {
    struct panel* layout;
    const struct style* style;

    struct vec2f spacing;

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
    layout = win->layout;
    style = &ctx->style;
    NK_ASSERT(bounds);

    spacing = style->window.spacing;
    panel_space = layout_row_calculate_usable_space(&ctx->style, layout->type,
                                                    layout->bounds.w, layout->row.columns);

#define NK_FRAC(x) (x - (float) (int) roundf(x)) /* will be used to remove fookin gaps */
    /* calculate the width of one item inside the current layout space */
    switch (layout->row.type) {
      case panel_row_layout_type::LAYOUT_DYNAMIC_FIXED: {
        /* scaling fixed size widgets item width */
        float w = NK_MAX(1.0f, panel_space) / (float) layout->row.columns;
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
        float ratio, w;
        NK_ASSERT(layout->row.ratio);
        ratio = (layout->row.ratio[layout->row.index] < 0) ? layout->row.item_width : layout->row.ratio[layout->row.index];

        w = (ratio * panel_space);
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
        float w;
        NK_ASSERT(layout->row.index < layout->row.columns);
        NK_ASSERT(layout->row.index < NK_MAX_LAYOUT_ROW_TEMPLATE_COLUMNS);
        w = layout->row.templates[layout->row.index];
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
  panel_alloc_space(struct rectf* bounds, const struct context* ctx) {
    struct window* win;
    struct panel* layout;

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout)
      return;

    /* check if the end of the row has been hit and begin new row if so */
    win = ctx->current;
    layout = win->layout;
    if (layout->row.index >= layout->row.columns)
      panel_alloc_row(ctx, win);

    /* calculate widget position and size */
    layout_widget_space(bounds, ctx, win, true);
    layout->row.index++;
  }
  NK_LIB void
  layout_peek(struct rectf* bounds, const struct context* ctx) {
    float y;
    int index;
    struct window* win;
    struct panel* layout;

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout) {
      *bounds = rect(0, 0, 0, 0);
      return;
    }

    win = ctx->current;
    layout = win->layout;
    y = layout->at_y;
    index = layout->row.index;
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
  spacer(struct context* ctx) {
    struct rectf dummy_rect = {0, 0, 0, 0};
    panel_alloc_space(&dummy_rect, ctx);
  }
} // namespace nk
