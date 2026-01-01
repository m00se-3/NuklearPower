#include "nuklear.h"
#include "nuklear_internal.h"

namespace nk {
  /* ===============================================================
   *
   *                              PANEL
   *
   * ===============================================================*/
  NK_LIB void*
  create_panel(struct context* ctx) {
    struct page_element* elem;
    elem = create_page_element(ctx);
    if (!elem)
      return 0;
    zero_struct(*elem);
    return &elem->data.pan;
  }
  NK_LIB void
  free_panel(struct context* ctx, struct panel* pan) {
    union page_data* pd = NK_CONTAINER_OF(pan, union page_data, pan);
    struct page_element* pe = NK_CONTAINER_OF(pd, struct page_element, data);
    free_page_element(ctx, pe);
  }
  NK_LIB bool
  panel_has_header(flag flags, const char* title) {
    bool active = 0;
    active = (flags & (panel_flags::WINDOW_CLOSABLE | panel_flags::WINDOW_MINIMIZABLE));
    active = active || (flags & panel_flags::WINDOW_TITLE);
    active = active && !(flags & window_flags::WINDOW_HIDDEN) && title;
    return active;
  }
  NK_LIB struct vec2f
  panel_get_padding(const struct style* style, panel_type::value_type type) {
    switch (type) {
      default:
      case panel_type::PANEL_WINDOW:
        return style->window.padding;
      case panel_type::PANEL_GROUP:
        return style->window.group_padding;
      case panel_type::PANEL_POPUP:
        return style->window.popup_padding;
      case panel_type::PANEL_CONTEXTUAL:
        return style->window.contextual_padding;
      case panel_type::PANEL_COMBO:
        return style->window.combo_padding;
      case panel_type::PANEL_MENU:
        return style->window.menu_padding;
      case panel_type::PANEL_TOOLTIP:
        return style->window.menu_padding;
    }
  }
  NK_LIB float
  panel_get_border(const struct style* style, flag flags,
                   panel_type::value_type type) {
    if (flags & panel_flags::WINDOW_BORDER) {
      switch (type) {
        default:
        case panel_type::PANEL_WINDOW:
          return style->window.border;
        case panel_type::PANEL_GROUP:
          return style->window.group_border;
        case panel_type::PANEL_POPUP:
          return style->window.popup_border;
        case panel_type::PANEL_CONTEXTUAL:
          return style->window.contextual_border;
        case panel_type::PANEL_COMBO:
          return style->window.combo_border;
        case panel_type::PANEL_MENU:
          return style->window.menu_border;
        case panel_type::PANEL_TOOLTIP:
          return style->window.menu_border;
      }
    } else
      return 0;
  }
  NK_LIB struct color
  panel_get_border_color(const struct style* style, panel_type::value_type type) {
    switch (type) {
      default:
      case panel_type::PANEL_WINDOW:
        return style->window.border_color;
      case panel_type::PANEL_GROUP:
        return style->window.group_border_color;
      case panel_type::PANEL_POPUP:
        return style->window.popup_border_color;
      case panel_type::PANEL_CONTEXTUAL:
        return style->window.contextual_border_color;
      case panel_type::PANEL_COMBO:
        return style->window.combo_border_color;
      case panel_type::PANEL_MENU:
        return style->window.menu_border_color;
      case panel_type::PANEL_TOOLTIP:
        return style->window.menu_border_color;
    }
  }
  NK_LIB bool
  panel_is_sub(panel_type::value_type type) {
    return ((int) type & (int) panel_set::PANEL_SET_SUB) ? 1 : 0;
  }
  NK_LIB bool
  panel_is_nonblock(panel_type::value_type type) {
    return ((int) type & (int) panel_set::PANEL_SET_NONBLOCK) ? 1 : 0;
  }
  NK_LIB bool
  panel_begin(struct context* ctx, const char* title, panel_type::value_type panel_type) {
    struct input* in;
    struct window* win;
    struct panel* layout;
    struct command_buffer* out;
    const struct style* style;
    const struct user_font* font;

    struct vec2f scrollbar_size;
    struct vec2f panel_padding;

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout)
      return 0;
    zero(ctx->current->layout, sizeof(*ctx->current->layout));
    if ((ctx->current->flags & window_flags::WINDOW_HIDDEN) || (ctx->current->flags & window_flags::WINDOW_CLOSED)) {
      zero(ctx->current->layout, sizeof(struct panel));
      ctx->current->layout->type = panel_type;
      return 0;
    }
    /* pull state into local stack */
    style = &ctx->style;
    font = style->font;
    win = ctx->current;
    layout = win->layout;
    out = &win->buffer;
    in = (win->flags & panel_flags::WINDOW_NO_INPUT) ? 0 : &ctx->input;
#ifdef NK_INCLUDE_COMMAND_USERDATA
    win->buffer.userdata = ctx->userdata;
#endif
    /* pull style configuration into local stack */
    scrollbar_size = style->window.scrollbar_size;
    panel_padding = panel_get_padding(style, panel_type);

    /* window movement */
    if ((win->flags & panel_flags::WINDOW_MOVABLE) && !(win->flags & window_flags::WINDOW_ROM)) {
      bool left_mouse_down;
      unsigned int left_mouse_clicked;
      int left_mouse_click_in_cursor;

      /* calculate draggable window space */
      rectf header;
      header.x = win->bounds.x;
      header.y = win->bounds.y;
      header.w = win->bounds.w;
      if (panel_has_header(win->flags, title)) {
        header.h = font->height + 2.0f * style->window.header.padding.y;
        header.h += 2.0f * style->window.header.label_padding.y;
      } else
        header.h = panel_padding.y;

      /* window movement by dragging */
      left_mouse_down = in->mouse.buttons[NK_BUTTON_LEFT].down;
      left_mouse_clicked = in->mouse.buttons[NK_BUTTON_LEFT].clicked;
      left_mouse_click_in_cursor = input_has_mouse_click_down_in_rect(in,
                                                                      NK_BUTTON_LEFT, header, true);
      if (left_mouse_down && left_mouse_click_in_cursor && !left_mouse_clicked) {
        win->bounds.x = win->bounds.x + in->mouse.delta.x;
        win->bounds.y = win->bounds.y + in->mouse.delta.y;
        in->mouse.buttons[NK_BUTTON_LEFT].clicked_pos.x += in->mouse.delta.x;
        in->mouse.buttons[NK_BUTTON_LEFT].clicked_pos.y += in->mouse.delta.y;
        ctx->style.cursor_active = ctx->style.cursors[static_cast<unsigned>(style_cursor::CURSOR_MOVE)];
      }
    }

    /* setup panel */
    layout->type = panel_type;
    layout->flags = win->flags;
    layout->bounds = win->bounds;
    layout->bounds.x += panel_padding.x;
    layout->bounds.w -= 2 * panel_padding.x;
    if (win->flags & panel_flags::WINDOW_BORDER) {
      layout->border = panel_get_border(style, win->flags, panel_type);
      layout->bounds = shrirect(layout->bounds, layout->border);
    } else
      layout->border = 0;
    layout->at_y = layout->bounds.y;
    layout->at_x = layout->bounds.x;
    layout->max_x = 0;
    layout->header_height = 0;
    layout->footer_height = 0;
    layout_reset_min_row_height(ctx);
    layout->row.index = 0;
    layout->row.columns = 0;
    layout->row.ratio = 0;
    layout->row.item_width = 0;
    layout->row.tree_depth = 0;
    layout->row.height = panel_padding.y;
    layout->has_scrolling = true;
    if (!(win->flags & panel_flags::WINDOW_NO_SCROLLBAR))
      layout->bounds.w -= scrollbar_size.x;
    if (!panel_is_nonblock(panel_type)) {
      layout->footer_height = 0;
      if (!(win->flags & panel_flags::WINDOW_NO_SCROLLBAR) || win->flags & panel_flags::WINDOW_SCALABLE)
        layout->footer_height = scrollbar_size.y;
      layout->bounds.h -= layout->footer_height;
    }

    /* panel header */
    if (panel_has_header(win->flags, title)) {
      struct text text;
      rectf header;
      const struct style_item* background = 0;

      /* calculate header bounds */
      header.x = win->bounds.x;
      header.y = win->bounds.y;
      header.w = win->bounds.w;
      header.h = font->height + 2.0f * style->window.header.padding.y;
      header.h += (2.0f * style->window.header.label_padding.y);

      /* shrink panel by header */
      layout->header_height = header.h;
      layout->bounds.y += header.h;
      layout->bounds.h -= header.h;
      layout->at_y += header.h;

      /* select correct header background and text color */
      if (ctx->active == win) {
        background = &style->window.header.active;
        text.txt = style->window.header.label_active;
      } else if (input_is_mouse_hovering_rect(&ctx->input, header)) {
        background = &style->window.header.hover;
        text.txt = style->window.header.label_hover;
      } else {
        background = &style->window.header.normal;
        text.txt = style->window.header.label_normal;
      }

      /* draw header background */
      header.h += 1.0f;

      switch (background->type) {
        case style_item_type::STYLE_ITEM_IMAGE:
          text.background = rgba(0, 0, 0, 0);
          draw_image(&win->buffer, header, &background->data.image, white);
          break;
        case style_item_type::STYLE_ITEM_NINE_SLICE:
          text.background = rgba(0, 0, 0, 0);
          draw_nine_slice(&win->buffer, header, &background->data.slice, white);
          break;
        case style_item_type::STYLE_ITEM_COLOR:
          text.background = background->data.color;
          fill_rect(out, header, 0, background->data.color);
          break;
      }

      /* window close button */
      {
        rectf button;
        button.y = header.y + style->window.header.padding.y;
        button.h = header.h - 2 * style->window.header.padding.y;
        button.w = button.h;
        if (win->flags & panel_flags::WINDOW_CLOSABLE) {
          flag ws = 0;
          if (style->window.header.align == style_header_align::HEADER_RIGHT) {
            button.x = (header.w + header.x) - (button.w + style->window.header.padding.x);
            header.w -= button.w + style->window.header.spacing.x + style->window.header.padding.x;
          } else {
            button.x = header.x + style->window.header.padding.x;
            header.x += button.w + style->window.header.spacing.x + style->window.header.padding.x;
          }

          if (do_button_symbol(&ws, &win->buffer, button,
                               style->window.header.close_symbol, btn_behavior::BUTTON_DEFAULT,
                               &style->window.header.close_button, in, style->font) &&
              !(win->flags & window_flags::WINDOW_ROM)) {
            layout->flags |= window_flags::WINDOW_HIDDEN;
            layout->flags &= ~static_cast<flag>(window_flags::WINDOW_MINIMIZED);
          }
        }

        /* window minimize button */
        if (win->flags & panel_flags::WINDOW_MINIMIZABLE) {
          flag ws = 0;
          if (style->window.header.align == style_header_align::HEADER_RIGHT) {
            button.x = (header.w + header.x) - button.w;
            if (!(win->flags & panel_flags::WINDOW_CLOSABLE)) {
              button.x -= style->window.header.padding.x;
              header.w -= style->window.header.padding.x;
            }
            header.w -= button.w + style->window.header.spacing.x;
          } else {
            button.x = header.x;
            header.x += button.w + style->window.header.spacing.x + style->window.header.padding.x;
          }
          if (do_button_symbol(&ws, &win->buffer, button, (layout->flags & window_flags::WINDOW_MINIMIZED) ? style->window.header.maximize_symbol : style->window.header.minimize_symbol,
                               btn_behavior::BUTTON_DEFAULT, &style->window.header.minimize_button, in, style->font) &&
              !(win->flags & window_flags::WINDOW_ROM))
            layout->flags = (layout->flags & window_flags::WINDOW_MINIMIZED) ? layout->flags & ~static_cast<flag>(window_flags::WINDOW_MINIMIZED) : layout->flags | window_flags::WINDOW_MINIMIZED;
        }
      }

      { /* window header title */
        int text_len = strlen(title);
        rectf label = {0, 0, 0, 0};
        float t = font->width(font->userdata, font->height, title, text_len);
        text.padding = vec2_from_floats(0, 0);

        label.x = header.x + style->window.header.padding.x;
        label.x += style->window.header.label_padding.x;
        label.y = header.y + style->window.header.label_padding.y;
        label.h = font->height + 2 * style->window.header.label_padding.y;
        label.w = t + 2 * style->window.header.spacing.x;
        label.w = NK_CLAMP(0, label.w, header.x + header.w - label.x);
        widget_text(out, label, (const char*) title, text_len, &text, NK_TEXT_LEFT, font);
      }
    }

    /* draw window background */
    if (!(layout->flags & window_flags::WINDOW_MINIMIZED) && !(layout->flags & window_flags::WINDOW_DYNAMIC)) {
      rectf body;
      body.x = win->bounds.x;
      body.w = win->bounds.w;
      body.y = (win->bounds.y + layout->header_height);
      body.h = (win->bounds.h - layout->header_height);

      switch (style->window.fixed_background.type) {
        case style_item_type::STYLE_ITEM_IMAGE:
          draw_image(out, body, &style->window.fixed_background.data.image, white);
          break;
        case style_item_type::STYLE_ITEM_NINE_SLICE:
          draw_nine_slice(out, body, &style->window.fixed_background.data.slice, white);
          break;
        case style_item_type::STYLE_ITEM_COLOR:
          fill_rect(out, body, style->window.rounding, style->window.fixed_background.data.color);
          break;
      }
    }

    /* set clipping rectangle */
    {
      rectf clip;
      layout->clip = layout->bounds;
      unify(&clip, &win->buffer.clip, layout->clip.x, layout->clip.y,
            layout->clip.x + layout->clip.w, layout->clip.y + layout->clip.h);
      push_scissor(out, clip);
      layout->clip = clip;
    }
    return !(layout->flags & window_flags::WINDOW_HIDDEN) && !(layout->flags & window_flags::WINDOW_MINIMIZED);
  }
  NK_LIB void
  panel_end(struct context* ctx) {
    struct input* in;
    struct window* window;
    struct panel* layout;
    const struct style* style;
    struct command_buffer* out;

    struct vec2f scrollbar_size;
    struct vec2f panel_padding;

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout)
      return;

    window = ctx->current;
    layout = window->layout;
    style = &ctx->style;
    out = &window->buffer;
    in = (layout->flags & window_flags::WINDOW_ROM || layout->flags & panel_flags::WINDOW_NO_INPUT) ? 0 : &ctx->input;
    if (!panel_is_sub(layout->type))
      push_scissor(out, null_rect);

    /* cache configuration data */
    scrollbar_size = style->window.scrollbar_size;
    panel_padding = panel_get_padding(style, layout->type);

    /* update the current cursor Y-position to point over the last added widget */
    layout->at_y += layout->row.height;

    /* dynamic panels */
    if (layout->flags & window_flags::WINDOW_DYNAMIC && !(layout->flags & window_flags::WINDOW_MINIMIZED)) {
      /* update panel height to fit dynamic growth */
      rectf empty_space;
      if (layout->at_y < (layout->bounds.y + layout->bounds.h))
        layout->bounds.h = layout->at_y - layout->bounds.y;

      /* fill top empty space */
      empty_space.x = window->bounds.x;
      empty_space.y = layout->bounds.y;
      empty_space.h = panel_padding.y;
      empty_space.w = window->bounds.w;
      fill_rect(out, empty_space, 0, style->window.background);

      /* fill left empty space */
      empty_space.x = window->bounds.x;
      empty_space.y = layout->bounds.y;
      empty_space.w = panel_padding.x + layout->border;
      empty_space.h = layout->bounds.h;
      fill_rect(out, empty_space, 0, style->window.background);

      /* fill right empty space */
      empty_space.x = layout->bounds.x + layout->bounds.w;
      empty_space.y = layout->bounds.y;
      empty_space.w = panel_padding.x + layout->border;
      empty_space.h = layout->bounds.h;
      if (*layout->offset_y == 0 && !(layout->flags & panel_flags::WINDOW_NO_SCROLLBAR))
        empty_space.w += scrollbar_size.x;
      fill_rect(out, empty_space, 0, style->window.background);

      /* fill bottom empty space */
      if (layout->footer_height > 0) {
        empty_space.x = window->bounds.x;
        empty_space.y = layout->bounds.y + layout->bounds.h;
        empty_space.w = window->bounds.w;
        empty_space.h = layout->footer_height;
        fill_rect(out, empty_space, 0, style->window.background);
      }
    }

    /* scrollbars */
    if (!(layout->flags & panel_flags::WINDOW_NO_SCROLLBAR) &&
        !(layout->flags & window_flags::WINDOW_MINIMIZED) &&
        window->scrollbar_hiding_timer < NK_SCROLLBAR_HIDING_TIMEOUT) {
      rectf scroll;
      int scroll_has_scrolling;
      float scroll_target;
      float scroll_offset;
      float scroll_step;
      float scroll_inc;

      /* mouse wheel scrolling */
      if (panel_is_sub(layout->type)) {
        /* sub-window mouse wheel scrolling */
        struct window* root_window = window;
        struct panel* root_panel = window->layout;
        while (root_panel->parent)
          root_panel = root_panel->parent;
        while (root_window->parent)
          root_window = root_window->parent;

        /* only allow scrolling if parent window is active */
        scroll_has_scrolling = false;
        if ((root_window == ctx->active) && layout->has_scrolling) {
          /* and panel is being hovered and inside clip rect*/
          if (input_is_mouse_hovering_rect(in, layout->bounds) &&
              intERSECT(layout->bounds.x, layout->bounds.y, layout->bounds.w, layout->bounds.h,
                        root_panel->clip.x, root_panel->clip.y, root_panel->clip.w, root_panel->clip.h)) {
            /* deactivate all parent scrolling */
            root_panel = window->layout;
            while (root_panel->parent) {
              root_panel->has_scrolling = false;
              root_panel = root_panel->parent;
            }
            root_panel->has_scrolling = false;
            scroll_has_scrolling = true;
          }
        }
      } else {
        /* window mouse wheel scrolling */
        scroll_has_scrolling = (window == ctx->active) && layout->has_scrolling;
        if (in && (in->mouse.scroll_delta.y > 0 || in->mouse.scroll_delta.x > 0) && scroll_has_scrolling)
          window->scrolled = true;
        else
          window->scrolled = false;
      }

      {
        /* vertical scrollbar */
        flag state = 0;
        scroll.x = layout->bounds.x + layout->bounds.w + panel_padding.x;
        scroll.y = layout->bounds.y;
        scroll.w = scrollbar_size.x;
        scroll.h = layout->bounds.h;

        scroll_offset = (float) *layout->offset_y;
        scroll_step = scroll.h * 0.10f;
        scroll_inc = scroll.h * 0.01f;
        scroll_target = (float) (int) (layout->at_y - scroll.y);
        scroll_offset = do_scrollbarv(&state, out, scroll, scroll_has_scrolling,
                                      scroll_offset, scroll_target, scroll_step, scroll_inc,
                                      &ctx->style.scrollv, in, style->font);
        *layout->offset_y = (unsigned int) scroll_offset;
        if (in && scroll_has_scrolling)
          in->mouse.scroll_delta.y = 0;
      }
      {
        /* horizontal scrollbar */
        flag state = 0;
        scroll.x = layout->bounds.x;
        scroll.y = layout->bounds.y + layout->bounds.h;
        scroll.w = layout->bounds.w;
        scroll.h = scrollbar_size.y;

        scroll_offset = (float) *layout->offset_x;
        scroll_target = (float) (int) (layout->max_x - scroll.x);
        scroll_step = layout->max_x * 0.05f;
        scroll_inc = layout->max_x * 0.005f;
        scroll_offset = do_scrollbarh(&state, out, scroll, scroll_has_scrolling,
                                      scroll_offset, scroll_target, scroll_step, scroll_inc,
                                      &ctx->style.scrollh, in, style->font);
        *layout->offset_x = (unsigned int) scroll_offset;
      }
    }

    /* hide scroll if no user input */
    if (window->flags & panel_flags::WINDOW_SCROLL_AUTO_HIDE) {
      int has_input = ctx->input.mouse.delta.x != 0 || ctx->input.mouse.delta.y != 0 || ctx->input.mouse.scroll_delta.y != 0;
      int is_window_hovered = window_is_hovered(ctx);
      int any_item_active = (ctx->last_widget_state & NK_WIDGET_STATE_MODIFIED);
      if ((!has_input && is_window_hovered) || (!is_window_hovered && !any_item_active))
        window->scrollbar_hiding_timer += ctx->delta_time_seconds;
      else
        window->scrollbar_hiding_timer = 0;
    } else
      window->scrollbar_hiding_timer = 0;

    /* window border */
    if (layout->flags & panel_flags::WINDOW_BORDER) {
      struct color border_color = panel_get_border_color(style, layout->type);
      const float padding_y = (layout->flags & window_flags::WINDOW_MINIMIZED)
                                  ? (style->window.border + window->bounds.y + layout->header_height)
                                  : ((layout->flags & window_flags::WINDOW_DYNAMIC)
                                         ? (layout->bounds.y + layout->bounds.h + layout->footer_height)
                                         : (window->bounds.y + window->bounds.h));
      rectf b = window->bounds;
      b.h = padding_y - window->bounds.y;
      stroke_rect(out, b, style->window.rounding, layout->border, border_color);
    }

    /* scaler */
    if ((layout->flags & panel_flags::WINDOW_SCALABLE) && in && !(layout->flags & window_flags::WINDOW_MINIMIZED)) {
      /* calculate scaler bounds */
      rectf scaler;
      scaler.w = scrollbar_size.x;
      scaler.h = scrollbar_size.y;
      scaler.y = layout->bounds.y + layout->bounds.h;
      if (layout->flags & panel_flags::WINDOW_SCALE_LEFT)
        scaler.x = layout->bounds.x - panel_padding.x * 0.5f;
      else
        scaler.x = layout->bounds.x + layout->bounds.w + panel_padding.x;
      if (layout->flags & panel_flags::WINDOW_NO_SCROLLBAR)
        scaler.x -= scaler.w;

      /* draw scaler */
      {
        const struct style_item* item = &style->window.scaler;
        if (item->type == style_item_type::STYLE_ITEM_IMAGE)
          draw_image(out, scaler, &item->data.image, white);
        else {
          if (layout->flags & panel_flags::WINDOW_SCALE_LEFT) {
            fill_triangle(out, scaler.x, scaler.y, scaler.x,
                          scaler.y + scaler.h, scaler.x + scaler.w,
                          scaler.y + scaler.h, item->data.color);
          } else {
            fill_triangle(out, scaler.x + scaler.w, scaler.y, scaler.x + scaler.w,
                          scaler.y + scaler.h, scaler.x, scaler.y + scaler.h, item->data.color);
          }
        }
      }

      /* do window scaling */
      if (!(window->flags & window_flags::WINDOW_ROM)) {
        struct vec2f window_size = style->window.min_size;
        int left_mouse_down = in->mouse.buttons[NK_BUTTON_LEFT].down;
        int left_mouse_click_in_scaler = input_has_mouse_click_down_in_rect(in,
                                                                            NK_BUTTON_LEFT, scaler, true);

        if (left_mouse_down && left_mouse_click_in_scaler) {
          float delta_x = in->mouse.delta.x;
          if (layout->flags & panel_flags::WINDOW_SCALE_LEFT) {
            delta_x = -delta_x;
            window->bounds.x += in->mouse.delta.x;
          }
          /* dragging in x-direction  */
          if (window->bounds.w + delta_x >= window_size.x) {
            if ((delta_x < 0) || (delta_x > 0 && in->mouse.pos.x >= scaler.x)) {
              window->bounds.w = window->bounds.w + delta_x;
              scaler.x += in->mouse.delta.x;
            }
          }
          /* dragging in y-direction (only possible if static window) */
          if (!(layout->flags & window_flags::WINDOW_DYNAMIC)) {
            if (window_size.y < window->bounds.h + in->mouse.delta.y) {
              if ((in->mouse.delta.y < 0) || (in->mouse.delta.y > 0 && in->mouse.pos.y >= scaler.y)) {
                window->bounds.h = window->bounds.h + in->mouse.delta.y;
                scaler.y += in->mouse.delta.y;
              }
            }
          }
          ctx->style.cursor_active = ctx->style.cursors[static_cast<unsigned>(style_cursor::CURSOR_RESIZE_TOP_RIGHT_DOWN_LEFT)];
          in->mouse.buttons[NK_BUTTON_LEFT].clicked_pos.x = scaler.x + scaler.w / 2.0f;
          in->mouse.buttons[NK_BUTTON_LEFT].clicked_pos.y = scaler.y + scaler.h / 2.0f;
        }
      }
    }
    if (!panel_is_sub(layout->type)) {
      /* window is hidden so clear command buffer  */
      if (layout->flags & window_flags::WINDOW_HIDDEN)
        command_buffer_reset(&window->buffer);
      /* window is visible and not tab */
      else
        finish(ctx, window);
    }

    /* NK_WINDOW_REMOVE_ROM flag was set so remove NK_WINDOW_ROM */
    if (layout->flags & window_flags::WINDOW_REMOVE_ROM) {
      layout->flags &= ~static_cast<flag>(window_flags::WINDOW_ROM);
      layout->flags &= ~static_cast<flag>(window_flags::WINDOW_REMOVE_ROM);
    }
    window->flags = layout->flags;

    /* property garbage collector */
    if (window->property.active && window->property.old != window->property.seq &&
        window->property.active == window->property.prev) {
      zero(&window->property, sizeof(window->property));
    } else {
      window->property.old = window->property.seq;
      window->property.prev = window->property.active;
      window->property.seq = 0;
    }
    /* edit garbage collector */
    if (window->edit.active && window->edit.old != window->edit.seq &&
        window->edit.active == window->edit.prev) {
      zero(&window->edit, sizeof(window->edit));
    } else {
      window->edit.old = window->edit.seq;
      window->edit.prev = window->edit.active;
      window->edit.seq = 0;
    }
    /* contextual garbage collector */
    if (window->popup.active_con && window->popup.con_old != window->popup.con_count) {
      window->popup.con_count = 0;
      window->popup.con_old = 0;
      window->popup.active_con = 0;
    } else {
      window->popup.con_old = window->popup.con_count;
      window->popup.con_count = 0;
    }
    window->popup.combo_count = 0;
    /* helper to make sure you have a 'tree_push' for every 'tree_pop' */
    NK_ASSERT(!layout->row.tree_depth);
  }
} // namespace nk
