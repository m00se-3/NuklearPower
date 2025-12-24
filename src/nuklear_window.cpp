#include "nuklear.h"
#include "nuklear_internal.h"
#include <cstring>

namespace nk {
  /* ===============================================================
   *
   *                              WINDOW
   *
   * ===============================================================*/
  NK_LIB void*
  create_window(struct context *ctx)
  {
    struct page_element *elem;
    elem = create_page_element(ctx);
    if (elem == nullptr) return 0;
    elem->data.win.seq = ctx->seq;
    return &elem->data.win;
  }
  NK_LIB void
  free_window(struct context *ctx, struct window *win)
  {
    /* unlink windows from list */
    struct table *it = win->tables;
    if (win->popup.win != nullptr) {
      free_window(ctx, win->popup.win);
      win->popup.win = 0;
    }
    win->next = 0;
    win->prev = 0;

    while (it != nullptr) {
      /*free window state tables */
      struct table *n = it->next;
      remove_table(win, it);
      free_table(ctx, it);
      if (it == win->tables)
        win->tables = n;
      it = n;
    }

    /* link windows into freelist */
    {union page_data *pd = NK_CONTAINER_OF(win, union page_data, win);
      struct page_element *pe = NK_CONTAINER_OF(pd, struct page_element, data);
      free_page_element(ctx, pe);}
  }
  NK_LIB struct window*
  find_window(const struct context *ctx, hash hash, const char *name)
  {
    struct window *iter;
    iter = ctx->begin;
    while (iter != nullptr) {
      NK_ASSERT(iter != iter->next);
      if (iter->name == hash) {
        int max_len = strlen(iter->name_string);
        if (stricmpn(iter->name_string, name, max_len) == 0)
          return iter;
      }
      iter = iter->next;
    }
    return 0;
  }
  NK_LIB void
  insert_window(struct context *ctx, struct window *win,
      enum window_insert_location loc)
  {
    const struct window *iter;
    NK_ASSERT(ctx);
    NK_ASSERT(win);
    if ((win == nullptr) || (ctx == nullptr)) return;

    iter = ctx->begin;
    while (iter != nullptr) {
      NK_ASSERT(iter != iter->next);
      NK_ASSERT(iter != win);
      if (iter == win) return;
      iter = iter->next;
    }

    if (ctx->begin == nullptr) {
      win->next = 0;
      win->prev = 0;
      ctx->begin = win;
      ctx->end = win;
      ctx->count = 1;
      return;
    }
    if (loc == NK_INSERT_BACK) {
      struct window *end;
      end = ctx->end;
      end->flags |= window_flags::WINDOW_ROM;
      end->next = win;
      win->prev = ctx->end;
      win->next = 0;
      ctx->end = win;
      ctx->active = ctx->end;
      ctx->end->flags &= ~static_cast<flag>(window_flags::WINDOW_ROM);
    } else {
      /*ctx->end->flag |= NK_WINDOW_ROM;*/
      ctx->begin->prev = win;
      win->next = ctx->begin;
      win->prev = 0;
      ctx->begin = win;
      ctx->begin->flags &= ~static_cast<flag>(window_flags::WINDOW_ROM);
    }
    ctx->count++;
  }
  NK_LIB void
  remove_window(struct context *ctx, struct window *win)
  {
    if (win == ctx->begin || win == ctx->end) {
      if (win == ctx->begin) {
        ctx->begin = win->next;
        if (win->next != nullptr)
          win->next->prev = 0;
      }
      if (win == ctx->end) {
        ctx->end = win->prev;
        if (win->prev)
          win->prev->next = 0;
      }
    } else {
      if (win->next != nullptr)
        win->next->prev = win->prev;
      if (win->prev != nullptr)
        win->prev->next = win->next;
    }
    if (win == ctx->active || (ctx->active == nullptr)) {
      ctx->active = ctx->end;
      if (ctx->end)
        ctx->end->flags &= ~static_cast<flag>(window_flags::WINDOW_ROM);
    }
    win->next = 0;
    win->prev = 0;
    ctx->count--;
  }
  NK_API bool
  begin(struct context *ctx, const char *title,
      rectf bounds, flag flags)
  {
    return begin_titled(ctx, title, title, bounds, flags);
  }
  NK_API bool
  begin_titled(struct context *ctx, const char *name, const char *title,
      rectf bounds, flag flags)
  {
    struct window *win;
    struct style *style;
    hash name_hash;
    int name_len;
    int ret = 0;

    NK_ASSERT(ctx);
    NK_ASSERT(name);
    NK_ASSERT(title);
    NK_ASSERT(ctx->style.font && ctx->style.font->width && "if this triggers you forgot to add a font");
    NK_ASSERT(!ctx->current && "if this triggers you missed a `end` call");
    if (!ctx || ctx->current || !title || !name)
      return 0;

    /* find or create window */
    style = &ctx->style;
    name_len = (int)strlen(name);
    name_hash = murmur_hash(name, (int)name_len, static_cast<hash>(panel_flags::WINDOW_TITLE));
    win = find_window(ctx, name_hash, name);
    if (!win) {
      /* create new window */
      std::size_t name_length = (std::size_t)name_len;
      win = (struct window*)create_window(ctx);
      NK_ASSERT(win);
      if (!win) return 0;

      if (flags & panel_flags::WINDOW_BACKGROUND)
        insert_window(ctx, win, NK_INSERT_FRONT);
      else insert_window(ctx, win, NK_INSERT_BACK);
      command_buffer_init(&win->buffer, &ctx->memory, NK_CLIPPING_ON);

      win->flags = flags;
      win->bounds = bounds;
      win->name = name_hash;
      name_length = NK_MIN(name_length, NK_WINDOW_MAX_NAME-1);
      std::memcpy(win->name_string, name, name_length);
      win->name_string[name_length] = 0;
      win->popup.win = 0;
      win->widgets_disabled = true;
      if (!ctx->active)
        ctx->active = win;
    } else {
      /* update window */
      win->flags &= ~static_cast<flag>(std::to_underlying(window_flags::WINDOW_PRIVATE) - 1);
      win->flags |= flags;
      if (!(win->flags & (panel_flags::WINDOW_MOVABLE | panel_flags::WINDOW_SCALABLE)))
        win->bounds = bounds;
      /* If this assert triggers you either:
       *
       * I.) Have more than one window with the same name or
       * II.) You forgot to actually draw the window.
       *      More specific you did not call `clear` (clear will be
       *      automatically called for you if you are using one of the
       *      provided demo backends). */
      NK_ASSERT(win->seq != ctx->seq);
      win->seq = ctx->seq;
      if (!ctx->active && !(win->flags & window_flags::WINDOW_HIDDEN)) {
        ctx->active = win;
        ctx->end = win;
      }
    }
    if (win->flags & window_flags::WINDOW_HIDDEN) {
      ctx->current = win;
      win->layout = 0;
      return 0;
    } else start(ctx, win);

    /* window overlapping */
    if (!(win->flags & window_flags::WINDOW_HIDDEN) && !(win->flags & panel_flags::WINDOW_NO_INPUT))
    {
      int inpanel, ishovered;
      struct window *iter = win;
      float h = ctx->style.font->height + 2.0f * style->window.header.padding.y +
          (2.0f * style->window.header.label_padding.y);
      rectf win_bounds = (!(win->flags & window_flags::WINDOW_MINIMIZED))?
          win->bounds: rect(win->bounds.x, win->bounds.y, win->bounds.w, h);

      /* activate window if hovered and no other window is overlapping this window */
      inpanel = input_has_mouse_click_down_in_rect(&ctx->input, NK_BUTTON_LEFT, win_bounds, true);
      inpanel = inpanel && ctx->input.mouse.buttons[NK_BUTTON_LEFT].clicked;
      ishovered = input_is_mouse_hovering_rect(&ctx->input, win_bounds);
      if ((win != ctx->active) && ishovered && !ctx->input.mouse.buttons[NK_BUTTON_LEFT].down) {
        iter = win->next;
        while (iter) {
          rectf iter_bounds = (!(iter->flags & window_flags::WINDOW_MINIMIZED))?
              iter->bounds: rect(iter->bounds.x, iter->bounds.y, iter->bounds.w, h);
          if (intERSECT(win_bounds.x, win_bounds.y, win_bounds.w, win_bounds.h,
              iter_bounds.x, iter_bounds.y, iter_bounds.w, iter_bounds.h) &&
              (!(iter->flags & window_flags::WINDOW_HIDDEN)))
            break;

          if (iter->popup.win && iter->popup.active && !(iter->flags & window_flags::WINDOW_HIDDEN) &&
              intERSECT(win->bounds.x, win_bounds.y, win_bounds.w, win_bounds.h,
              iter->popup.win->bounds.x, iter->popup.win->bounds.y,
              iter->popup.win->bounds.w, iter->popup.win->bounds.h))
            break;
          iter = iter->next;
        }
      }

      /* activate window if clicked */
      if (iter && inpanel && (win != ctx->end)) {
        iter = win->next;
        while (iter) {
          /* try to find a panel with higher priority in the same position */
          rectf iter_bounds = (!(iter->flags & window_flags::WINDOW_MINIMIZED))?
          iter->bounds: rect(iter->bounds.x, iter->bounds.y, iter->bounds.w, h);
          if (NK_INBOX(ctx->input.mouse.pos.x, ctx->input.mouse.pos.y,
              iter_bounds.x, iter_bounds.y, iter_bounds.w, iter_bounds.h) &&
              !(iter->flags & window_flags::WINDOW_HIDDEN))
            break;
          if (iter->popup.win && iter->popup.active && !(iter->flags & window_flags::WINDOW_HIDDEN) &&
              intERSECT(win_bounds.x, win_bounds.y, win_bounds.w, win_bounds.h,
              iter->popup.win->bounds.x, iter->popup.win->bounds.y,
              iter->popup.win->bounds.w, iter->popup.win->bounds.h))
            break;
          iter = iter->next;
        }
      }
      if (iter && !(win->flags & window_flags::WINDOW_ROM) && (win->flags & panel_flags::WINDOW_BACKGROUND)) {
        win->flags |= static_cast<flag>(window_flags::WINDOW_ROM);
        iter->flags &= ~static_cast<flag>(window_flags::WINDOW_ROM);
        ctx->active = iter;
        if (!(iter->flags & panel_flags::WINDOW_BACKGROUND)) {
          /* current window is active in that position so transfer to top
           * at the highest priority in stack */
          remove_window(ctx, iter);
          insert_window(ctx, iter, NK_INSERT_BACK);
        }
      } else {
        if (!iter && ctx->end != win) {
          if (!(win->flags & panel_flags::WINDOW_BACKGROUND)) {
            /* current window is active in that position so transfer to top
             * at the highest priority in stack */
            remove_window(ctx, win);
            insert_window(ctx, win, NK_INSERT_BACK);
          }
          win->flags &= ~static_cast<flag>(window_flags::WINDOW_ROM);
          ctx->active = win;
        }
        if (ctx->end != win && !(win->flags & panel_flags::WINDOW_BACKGROUND))
          win->flags |= window_flags::WINDOW_ROM;
      }
    }
    win->layout = (panel*)create_panel(ctx);
    ctx->current = win;
    ret = panel_begin(ctx, title, panel_type::PANEL_WINDOW);
    win->layout->offset_x = &win->scrollbar.x;
    win->layout->offset_y = &win->scrollbar.y;
    return ret;
  }
  NK_API void
  end(context *ctx)
  {
    panel *layout;
    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current && "if this triggers you forgot to call `begin`");
    if (!ctx || !ctx->current)
      return;

    layout = ctx->current->layout;
    if (!layout || (layout->type == panel_type::PANEL_WINDOW && (ctx->current->flags & window_flags::WINDOW_HIDDEN))) {
      ctx->current = 0;
      return;
    }
    panel_end(ctx);
    free_panel(ctx, ctx->current->layout);
    ctx->current = 0;
  }
  NK_API rectf
  window_get_bounds(const struct context *ctx)
  {
    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    if (!ctx || !ctx->current) return rect(0,0,0,0);
    return ctx->current->bounds;
  }
  NK_API vec2f
  window_get_position(const struct context *ctx)
  {
    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    if (!ctx || !ctx->current) return vec2_from_floats(0.0f,0.0f);
    return vec2_from_floats(ctx->current->bounds.x, ctx->current->bounds.y);
  }
  NK_API vec2f
  window_get_size(const struct context *ctx)
  {
    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    if (!ctx || !ctx->current) return vec2_from_floats(0.0f,0.0f);
    return vec2_from_floats(ctx->current->bounds.w, ctx->current->bounds.h);
  }
  NK_API float
  window_get_width(const struct context *ctx)
  {
    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    if (!ctx || !ctx->current) return 0;
    return ctx->current->bounds.w;
  }
  NK_API float
  window_get_height(const struct context *ctx)
  {
    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    if (!ctx || !ctx->current) return 0;
    return ctx->current->bounds.h;
  }
  NK_API rectf
  window_get_content_region(const struct context *ctx)
  {
    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    if (!ctx || !ctx->current) return rect(0,0,0,0);
    return ctx->current->layout->clip;
  }
  NK_API vec2f
  window_get_content_region_min(const struct context *ctx)
  {
    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current) return vec2_from_floats(0,0);
    return vec2_from_floats(ctx->current->layout->clip.x, ctx->current->layout->clip.y);
  }
  NK_API vec2f
  window_get_content_region_max(const struct context *ctx)
  {
    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current) return vec2_from_floats(0,0);
    return vec2_from_floats(ctx->current->layout->clip.x + ctx->current->layout->clip.w,
        ctx->current->layout->clip.y + ctx->current->layout->clip.h);
  }
  NK_API vec2f
  window_get_content_region_size(const struct context *ctx)
  {
    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current) return vec2_from_floats(0,0);
    return vec2_from_floats(ctx->current->layout->clip.w, ctx->current->layout->clip.h);
  }
  NK_API struct command_buffer*
  window_get_canvas(const struct context *ctx)
  {
    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current) return 0;
    return &ctx->current->buffer;
  }
  NK_API struct panel*
  window_get_panel(const struct context *ctx)
  {
    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    if (!ctx || !ctx->current) return 0;
    return ctx->current->layout;
  }
  NK_API void
  window_get_scroll(const struct context *ctx, std::uint32_t *offset_x, std::uint32_t *offset_y)
  {
    struct window *win;
    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    if (!ctx || !ctx->current)
      return ;
    win = ctx->current;
    if (offset_x)
      *offset_x = win->scrollbar.x;
    if (offset_y)
      *offset_y = win->scrollbar.y;
  }
  NK_API bool
  window_has_focus(const struct context *ctx)
  {
    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current) return 0;
    return ctx->current == ctx->active;
  }
  NK_API bool
  window_is_hovered(const struct context *ctx)
  {
    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    if (!ctx || !ctx->current || (ctx->current->flags & window_flags::WINDOW_HIDDEN))
      return 0;
    else {
      rectf actual_bounds = ctx->current->bounds;
      if (ctx->begin->flags & window_flags::WINDOW_MINIMIZED) {
        actual_bounds.h = ctx->current->layout->header_height;
      }
      return input_is_mouse_hovering_rect(&ctx->input, actual_bounds);
    }
  }
  NK_API bool
  window_is_any_hovered(const struct context *ctx)
  {
    struct window *iter;
    NK_ASSERT(ctx);
    if (!ctx) return 0;
    iter = ctx->begin;
    while (iter) {
      /* check if window is being hovered */
      if(!(iter->flags & window_flags::WINDOW_HIDDEN)) {
        /* check if window popup is being hovered */
        if (iter->popup.active && iter->popup.win && input_is_mouse_hovering_rect(&ctx->input, iter->popup.win->bounds))
          return 1;

        if (iter->flags & window_flags::WINDOW_MINIMIZED) {
          rectf header = iter->bounds;
          header.h = ctx->style.font->height + 2 * ctx->style.window.header.padding.y;
          if (input_is_mouse_hovering_rect(&ctx->input, header))
            return 1;
        } else if (input_is_mouse_hovering_rect(&ctx->input, iter->bounds)) {
          return 1;
        }
      }
      iter = iter->next;
    }
    return 0;
  }
  NK_API bool
  item_is_any_active(const struct context *ctx)
  {
    int any_hovered = window_is_any_hovered(ctx);
    int any_active = (ctx->last_widget_state & NK_WIDGET_STATE_MODIFIED);
    return any_hovered || any_active;
  }
  NK_API bool
  window_is_collapsed(const struct context *ctx, const char *name)
  {
    int title_len;
    hash title_hash;
    struct window *win;
    NK_ASSERT(ctx);
    if (!ctx) return 0;

    title_len = (int)strlen(name);
    title_hash = murmur_hash(name, (int)title_len, static_cast<hash>(panel_flags::WINDOW_TITLE));
    win = find_window(ctx, title_hash, name);
    if (!win) return 0;
    return win->flags & window_flags::WINDOW_MINIMIZED;
  }
  NK_API bool
  window_is_closed(const struct context *ctx, const char *name)
  {
    int title_len;
    hash title_hash;
    struct window *win;
    NK_ASSERT(ctx);
    if (!ctx) return 1;

    title_len = (int)strlen(name);
    title_hash = murmur_hash(name, (int)title_len, static_cast<hash>(panel_flags::WINDOW_TITLE));
    win = find_window(ctx, title_hash, name);
    if (!win) return 1;
    return (win->flags & window_flags::WINDOW_CLOSED);
  }
  NK_API bool
  window_is_hidden(const struct context *ctx, const char *name)
  {
    int title_len;
    hash title_hash;
    struct window *win;
    NK_ASSERT(ctx);
    if (!ctx) return 1;

    title_len = strlen(name);
    title_hash = murmur_hash(name, title_len, static_cast<hash>(panel_flags::WINDOW_TITLE));
    win = find_window(ctx, title_hash, name);
    if (!win) return 1;
    return (win->flags & window_flags::WINDOW_HIDDEN);
  }
  NK_API bool
  window_is_active(const context *ctx, const char *name)
  {
    int title_len;
    hash title_hash;
    struct window *win;
    NK_ASSERT(ctx);
    if (!ctx) return 0;

    title_len = (int)strlen(name);
    title_hash = murmur_hash(name, title_len, static_cast<hash>(panel_flags::WINDOW_TITLE));
    win = find_window(ctx, title_hash, name);
    if (!win) return 0;
    return win == ctx->active;
  }
  NK_API struct window*
  window_find(const struct context *ctx, const char *name)
  {
    int title_len;
    hash title_hash;
    title_len = (int)strlen(name);
    title_hash = murmur_hash(name, title_len, static_cast<hash>(panel_flags::WINDOW_TITLE));
    return find_window(ctx, title_hash, name);
  }
  NK_API void
  window_close(struct context *ctx, const char *name)
  {
    struct window *win;
    NK_ASSERT(ctx);
    if (!ctx) return;
    win = window_find(ctx, name);
    if (!win) return;
    NK_ASSERT(ctx->current != win && "You cannot close a currently active window");
    if (ctx->current == win) return;
    win->flags |= window_flags::WINDOW_HIDDEN;
    win->flags |= window_flags::WINDOW_CLOSED;
  }
  NK_API void
  window_set_bounds(struct context *ctx,
      const char *name, rectf bounds)
  {
    struct window *win;
    NK_ASSERT(ctx);
    if (!ctx) return;
    win = window_find(ctx, name);
    if (!win) return;
    win->bounds = bounds;
  }
  NK_API void
  window_set_position(struct context *ctx,
      const char *name, vec2f pos)
  {
    struct window *win = window_find(ctx, name);
    if (!win) return;
    win->bounds.x = pos.x;
    win->bounds.y = pos.y;
  }
  NK_API void
  window_set_size(struct context *ctx,
      const char *name, vec2f size)
  {
    struct window *win = window_find(ctx, name);
    if (!win) return;
    win->bounds.w = size.x;
    win->bounds.h = size.y;
  }
  NK_API void
  window_set_scroll(struct context *ctx, std::uint32_t offset_x, std::uint32_t offset_y)
  {
    struct window *win;
    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    if (!ctx || !ctx->current)
      return;
    win = ctx->current;
    win->scrollbar.x = offset_x;
    win->scrollbar.y = offset_y;
  }
  NK_API void
  window_collapse(struct context *ctx, const char *name,
                      enum collapse_states c)
  {
    int title_len;
    hash title_hash;
    struct window *win;
    NK_ASSERT(ctx);
    if (!ctx) return;

    title_len = strlen(name);
    title_hash = murmur_hash(name, title_len, static_cast<hash>(panel_flags::WINDOW_TITLE));
    win = find_window(ctx, title_hash, name);
    if (!win) return;
    if (c == collapse_states::MINIMIZED)
      win->flags |= window_flags::WINDOW_MINIMIZED;
    else win->flags &= ~static_cast<flag>(window_flags::WINDOW_MINIMIZED);
  }
  NK_API void
  window_collapse_if(struct context *ctx, const char *name,
      enum collapse_states c, int cond)
  {
    NK_ASSERT(ctx);
    if (!ctx || !cond) return;
    window_collapse(ctx, name, c);
  }
  NK_API void
  window_show(struct context *ctx, const char *name, enum show_states s)
  {
    int title_len;
    hash title_hash;
    struct window *win;
    NK_ASSERT(ctx);
    if (!ctx) return;

    title_len = strlen(name);
    title_hash = murmur_hash(name, title_len, static_cast<hash>(panel_flags::WINDOW_TITLE));
    win = find_window(ctx, title_hash, name);
    if (!win) return;
    if (s == show_states::HIDDEN) {
      win->flags |= window_flags::WINDOW_HIDDEN;
    } else win->flags &= ~static_cast<flag>(window_flags::WINDOW_HIDDEN);
  }
  NK_API void
  window_show_if(struct context *ctx, const char *name,
      enum show_states s, int cond)
  {
    NK_ASSERT(ctx);
    if (!ctx || !cond) return;
    window_show(ctx, name, s);
  }

  NK_API void
  window_set_focus(struct context *ctx, const char *name)
  {
    int title_len;
    hash title_hash;
    struct window *win;
    NK_ASSERT(ctx);
    if (!ctx) return;

    title_len = (int)strlen(name);
    title_hash = murmur_hash(name, title_len, static_cast<hash>(panel_flags::WINDOW_TITLE));
    win = find_window(ctx, title_hash, name);
    if (win && ctx->end != win) {
      remove_window(ctx, win);
      insert_window(ctx, win, NK_INSERT_BACK);
    }
    ctx->active = win;
  }
  NK_API void
  rule_horizontal(struct context *ctx, struct color color, bool rounding)
  {
    rectf space;
    enum widget_layout_states state = widget(&space, ctx);
    struct command_buffer *canvas = window_get_canvas(ctx);
    if (!state) return;
    fill_rect(canvas, space, rounding && space.h > 1.5f ? space.h / 2.0f : 0, color);
  }
}