#include "nuklear.h"
#include "nuklear_internal.h"

namespace nk {
  /* ===============================================================
   *
   *                              POPUP
   *
   * ===============================================================*/
  NK_API bool
  popup_begin(context* ctx, const popup_type type,
              const char* title, const flag flags, rectf rect) {

    NK_ASSERT(ctx);
    NK_ASSERT(title);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout)
      return 0;

    window* win = ctx->current;
    const panel* panel = win->layout;
    NK_ASSERT(!((int) panel->type & static_cast<int>(panel_set::PANEL_SET_POPUP)) && "popups are not allowed to have popups");
    (void) panel;
    const int title_len = (int) strlen(title);
    const hash title_hash = murmur_hash(title, (int) title_len, static_cast<hash>(panel_type::PANEL_POPUP));

    window* popup = win->popup.win;
    if (!popup) {
      popup = (window*) create_window(ctx);
      popup->parent = win;
      win->popup.win = popup;
      win->popup.active = 0;
      win->popup.type = panel_type::PANEL_POPUP;
    }

    /* make sure we have correct popup */
    if (win->popup.name != title_hash) {
      if (!win->popup.active) {
        zero(popup, sizeof(*popup));
        win->popup.name = title_hash;
        win->popup.active = 1;
        win->popup.type = panel_type::PANEL_POPUP;
      } else
        return 0;
    }

    /* popup position is local to window */
    ctx->current = popup;
    rect.x += win->layout->clip.x;
    rect.y += win->layout->clip.y;

    /* setup popup data */
    popup->parent = win;
    popup->bounds = rect;
    popup->seq = ctx->seq;
    popup->layout = (struct panel*) create_panel(ctx);
    popup->flags = flags;
    popup->flags |= panel_flags::WINDOW_BORDER;
    if (type == popup_type::POPUP_DYNAMIC)
      popup->flags |= window_flags::WINDOW_DYNAMIC;

    popup->buffer = win->buffer;
    start_popup(ctx, win);
    const std::size_t allocated = ctx->memory.allocated;
    push_scissor(&popup->buffer, null_rect);

    if (panel_begin(ctx, title, panel_type::PANEL_POPUP)) {
      /* popup is running therefore invalidate parent panels */
      struct panel* root = win->layout;
      while (root) {
        root->flags |= window_flags::WINDOW_ROM;
        root->flags &= ~static_cast<flag>(window_flags::WINDOW_REMOVE_ROM);
        root = root->parent;
      }
      win->popup.active = 1;
      popup->layout->offset_x = &popup->scrollbar.x;
      popup->layout->offset_y = &popup->scrollbar.y;
      popup->layout->parent = win->layout;
      return 1;
    } else {
      /* popup was closed/is invalid so cleanup */
      struct panel* root = win->layout;
      while (root) {
        root->flags |= window_flags::WINDOW_REMOVE_ROM;
        root = root->parent;
      }
      win->popup.buf.active = 0;
      win->popup.active = 0;
      ctx->memory.allocated = allocated;
      ctx->current = win;
      free_panel(ctx, popup->layout);
      popup->layout = 0;
      return 0;
    }
  }
  NK_LIB bool
  nonblock_begin(context* ctx,
                 const flag flags, const rectf body, const rectf header,
                 const panel_type::value_type panel_type) {
    int is_active = true;

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout)
      return 0;

    /* popups cannot have popups */
    window* win = ctx->current;
    const panel* panel = win->layout;
    NK_ASSERT(!((int) panel->type & static_cast<int>(panel_set::PANEL_SET_POPUP)));
    (void) panel;
    window* popup = win->popup.win;
    if (!popup) {
      /* create window for nonblocking popup */
      popup = (window*) create_window(ctx);
      popup->parent = win;
      win->popup.win = popup;
      win->popup.type = panel_type;
      command_buffer_init(&popup->buffer, &ctx->memory, NK_CLIPPING_ON);
    } else {
      /* close the popup if user pressed outside or in the header */
      int pressed, in_body, in_header;
#ifdef NK_BUTTON_TRIGGER_ON_RELEASE
      pressed = input_is_mouse_released(&ctx->input, NK_BUTTON_LEFT);
#else
      pressed = input_is_mouse_pressed(&ctx->input, NK_BUTTON_LEFT);
#endif
      in_body = input_is_mouse_hovering_rect(&ctx->input, body);
      in_header = input_is_mouse_hovering_rect(&ctx->input, header);
      if (pressed && (!in_body || in_header))
        is_active = false;
    }
    win->popup.header = header;

    if (!is_active) {
      /* remove read only mode from all parent panels */
      struct panel* root = win->layout;
      while (root) {
        root->flags |= window_flags::WINDOW_REMOVE_ROM;
        root = root->parent;
      }
      return is_active;
    }
    popup->bounds = body;
    popup->parent = win;
    popup->layout = (struct panel*) create_panel(ctx);
    popup->flags = flags;
    popup->flags |= panel_flags::WINDOW_BORDER;
    popup->flags |= window_flags::WINDOW_DYNAMIC;
    popup->seq = ctx->seq;
    win->popup.active = 1;
    NK_ASSERT(popup->layout);

    start_popup(ctx, win);
    popup->buffer = win->buffer;
    push_scissor(&popup->buffer, null_rect);
    ctx->current = popup;

    panel_begin(ctx, 0, panel_type);
    win->buffer = popup->buffer;
    popup->layout->parent = win->layout;
    popup->layout->offset_x = &popup->scrollbar.x;
    popup->layout->offset_y = &popup->scrollbar.y;

    /* set read only mode to all parent panels */
    {
      struct panel* root = win->layout;
      while (root) {
        root->flags |= window_flags::WINDOW_ROM;
        root = root->parent;
      }
    }
    return is_active;
  }
  NK_API void
  popup_close(context* ctx) {
    NK_ASSERT(ctx);
    if (!ctx || !ctx->current)
      return;

    window* popup = ctx->current;
    NK_ASSERT(popup->parent);
    NK_ASSERT((int) popup->layout->type & static_cast<int>(panel_set::PANEL_SET_POPUP));
    popup->flags |= window_flags::WINDOW_HIDDEN;
  }
  NK_API void
  popup_end(context* ctx) {

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout)
      return;

    window* popup = ctx->current;
    if (!popup->parent)
      return;
    window* win = popup->parent;
    if (popup->flags & window_flags::WINDOW_HIDDEN) {
      panel* root = win->layout;
      while (root) {
        root->flags |= window_flags::WINDOW_REMOVE_ROM;
        root = root->parent;
      }
      win->popup.active = 0;
    }
    push_scissor(&popup->buffer, null_rect);
    end(ctx);

    win->buffer = popup->buffer;
    finish_popup(ctx, win);
    ctx->current = win;
    push_scissor(&win->buffer, win->layout->clip);
  }
  NK_API void
  popup_get_scroll(const context* ctx, unsigned int* offset_x, unsigned int* offset_y) {

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout)
      return;

    const window* popup = ctx->current;
    if (offset_x)
      *offset_x = popup->scrollbar.x;
    if (offset_y)
      *offset_y = popup->scrollbar.y;
  }
  NK_API void
  popup_set_scroll(context* ctx, const unsigned int offset_x, const unsigned int offset_y) {

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout)
      return;

    window* popup = ctx->current;
    popup->scrollbar.x = offset_x;
    popup->scrollbar.y = offset_y;
  }
} // namespace nk
