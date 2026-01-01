#include "nuklear.h"
#include "nuklear_internal.h"

namespace nk {

  /* ===============================================================
   *
   *                          GROUP
   *
   * ===============================================================*/
  NK_API bool
  group_scrolled_offset_begin(context* ctx,
                              unsigned int* x_offset, unsigned int* y_offset, const char* title, flag flags) {
    rectf bounds;
    window panel;
    window* win;

    win = ctx->current;
    panel_alloc_space(&bounds, ctx);
    {
      const rectf* c = &win->layout->clip;
      if (!intERSECT(c->x, c->y, c->w, c->h, bounds.x, bounds.y, bounds.w, bounds.h) &&
          !(flags & panel_flags::WINDOW_MOVABLE)) {
        return false;
      }
    }
    if ((win->flags & window_flags::WINDOW_ROM) != 0u) {
      flags |= window_flags::WINDOW_ROM;
    }

    /* initialize a fake window to create the panel from */
    zero(&panel, sizeof(panel));
    panel.bounds = bounds;
    panel.flags = flags;
    panel.scrollbar.x = *x_offset;
    panel.scrollbar.y = *y_offset;
    panel.buffer = win->buffer;
    panel.layout = (struct panel*) create_panel(ctx);
    ctx->current = &panel;
    panel_begin(ctx, (flags & panel_flags::WINDOW_TITLE) ? title : 0, panel_type::PANEL_GROUP);

    win->buffer = panel.buffer;
    win->buffer.clip = panel.layout->clip;
    panel.layout->offset_x = x_offset;
    panel.layout->offset_y = y_offset;
    panel.layout->parent = win->layout;
    win->layout = panel.layout;

    ctx->current = win;
    if (((panel.layout->flags & window_flags::WINDOW_CLOSED) != 0u) ||
        ((panel.layout->flags & window_flags::WINDOW_MINIMIZED) != 0u)) {
      flag f = panel.layout->flags;
      group_scrolled_end(ctx);
      if ((f & window_flags::WINDOW_CLOSED) != 0u) {
        return static_cast<bool>(window_flags::WINDOW_CLOSED);
      }
      if ((f & window_flags::WINDOW_MINIMIZED) != 0u) {
        return static_cast<bool>(window_flags::WINDOW_MINIMIZED);
      }
    }
    return 1;
  }
  NK_API void
  group_scrolled_end(context* ctx) {
    window* win;
    panel* parent;
    panel* g;

    rectf clip;
    window pan;
    vec2f panel_padding;

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    if (!ctx || !ctx->current)
      return;

    /* make sure group_begin was called correctly */
    NK_ASSERT(ctx->current);
    win = ctx->current;
    NK_ASSERT(win->layout);
    g = win->layout;
    NK_ASSERT(g->parent);
    parent = g->parent;

    /* dummy window */
    zero_struct(pan);
    panel_padding = panel_get_padding(&ctx->style, panel_type::PANEL_GROUP);
    pan.bounds.y = g->bounds.y - (g->header_height + g->menu.h);
    pan.bounds.x = g->bounds.x - panel_padding.x;
    pan.bounds.w = g->bounds.w + 2 * panel_padding.x;
    pan.bounds.h = g->bounds.h + g->header_height + g->menu.h;
    if (g->flags & panel_flags::WINDOW_BORDER) {
      pan.bounds.x -= g->border;
      pan.bounds.y -= g->border;
      pan.bounds.w += 2 * g->border;
      pan.bounds.h += 2 * g->border;
    }
    if (!(g->flags & panel_flags::WINDOW_NO_SCROLLBAR)) {
      pan.bounds.w += ctx->style.window.scrollbar_size.x;
      pan.bounds.h += ctx->style.window.scrollbar_size.y;
    }
    pan.scrollbar.x = *g->offset_x;
    pan.scrollbar.y = *g->offset_y;
    pan.flags = g->flags;
    pan.buffer = win->buffer;
    pan.layout = g;
    pan.parent = win;
    ctx->current = &pan;

    /* make sure group has correct clipping rectangle */
    unify(&clip, &parent->clip, pan.bounds.x, pan.bounds.y,
          pan.bounds.x + pan.bounds.w, pan.bounds.y + pan.bounds.h + panel_padding.x);
    push_scissor(&pan.buffer, clip);
    end(ctx);

    win->buffer = pan.buffer;
    push_scissor(&win->buffer, parent->clip);
    ctx->current = win;
    win->layout = parent;
    g->bounds = pan.bounds;
    return;
  }
  NK_API bool
  group_scrolled_begin(context* ctx,
                       scroll* scroll, const char* title, const flag flags) {
    return group_scrolled_offset_begin(ctx, &scroll->x, &scroll->y, title, flags);
  }
  NK_API bool
  group_begin_titled(context* ctx, const char* id,
                     const char* title, const flag flags) {
    unsigned int* y_offset;

    NK_ASSERT(ctx);
    NK_ASSERT(id);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout || !id)
      return 0;

    /* find persistent group scrollbar value */
    window* win = ctx->current;
    const int id_len = (int) strlen(id);
    const hash id_hash = murmur_hash(id, (int) id_len, static_cast<hash>(panel_type::PANEL_GROUP));
    unsigned int* x_offset = find_value(win, id_hash);
    if (!x_offset) {
      x_offset = add_value(ctx, win, id_hash, 0);
      y_offset = add_value(ctx, win, id_hash + 1, 0);

      NK_ASSERT(x_offset);
      NK_ASSERT(y_offset);
      if (!x_offset || !y_offset)
        return 0;
      *x_offset = *y_offset = 0;
    } else
      y_offset = find_value(win, id_hash + 1);
    return group_scrolled_offset_begin(ctx, x_offset, y_offset, title, flags);
  }
  NK_API bool
  group_begin(context* ctx, const char* title, const flag flags) {
    return group_begin_titled(ctx, title, title, flags);
  }
  NK_API void
  group_end(context* ctx) {
    group_scrolled_end(ctx);
  }
  NK_API void
  group_get_scroll(context* ctx, const char* id, unsigned int* x_offset, unsigned int* y_offset) {
    unsigned int* y_offset_ptr;

    NK_ASSERT(ctx);
    NK_ASSERT(id);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout || !id)
      return;

    /* find persistent group scrollbar value */
    window* win = ctx->current;
    const int id_len = (int) strlen(id);
    const hash id_hash = murmur_hash(id, (int) id_len, static_cast<hash>(panel_type::PANEL_GROUP));
    unsigned int* x_offset_ptr = find_value(win, id_hash);
    if (!x_offset_ptr) {
      x_offset_ptr = add_value(ctx, win, id_hash, 0);
      y_offset_ptr = add_value(ctx, win, id_hash + 1, 0);

      NK_ASSERT(x_offset_ptr);
      NK_ASSERT(y_offset_ptr);
      if (!x_offset_ptr || !y_offset_ptr)
        return;
      *x_offset_ptr = *y_offset_ptr = 0;
    } else
      y_offset_ptr = find_value(win, id_hash + 1);
    if (x_offset)
      *x_offset = *x_offset_ptr;
    if (y_offset)
      *y_offset = *y_offset_ptr;
  }
  NK_API void
  group_set_scroll(context* ctx, const char* id, const unsigned int x_offset, const unsigned int y_offset) {
    unsigned int* y_offset_ptr;

    NK_ASSERT(ctx);
    NK_ASSERT(id);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout || !id)
      return;

    /* find persistent group scrollbar value */
    window* win = ctx->current;
    const int id_len = (int) strlen(id);
    const hash id_hash = murmur_hash(id, (int) id_len, static_cast<hash>(panel_type::PANEL_GROUP));
    unsigned int* x_offset_ptr = find_value(win, id_hash);
    if (!x_offset_ptr) {
      x_offset_ptr = add_value(ctx, win, id_hash, 0);
      y_offset_ptr = add_value(ctx, win, id_hash + 1, 0);

      NK_ASSERT(x_offset_ptr);
      NK_ASSERT(y_offset_ptr);
      if (!x_offset_ptr || !y_offset_ptr)
        return;
      *x_offset_ptr = *y_offset_ptr = 0;
    } else
      y_offset_ptr = find_value(win, id_hash + 1);
    *x_offset_ptr = x_offset;
    *y_offset_ptr = y_offset;
  }
} // namespace nk
