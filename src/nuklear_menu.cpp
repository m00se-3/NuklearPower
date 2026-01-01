#include "nuklear.h"
#include "nuklear_internal.h"

namespace nk {
  /* ===============================================================
   *
   *                              MENU
   *
   * ===============================================================*/
  NK_API void
  menubar_begin(struct context* ctx) {
    struct panel* layout;
    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout)
      return;

    layout = ctx->current->layout;
    NK_ASSERT(layout->at_y == layout->bounds.y);
    /* if this assert triggers you allocated space between begin and menubar_begin.
    If you want a menubar the first nuklear function after `begin` has to be a
    `menubar_begin` call. Inside the menubar you then have to allocate space for
    widgets (also supports multiple rows).
    Example:
        if (begin(...)) {
            menubar_begin(...);
                layout_xxxx(...);
                button(...);
                layout_xxxx(...);
                button(...);
            menubar_end(...);
        }
        end(...);
    */
    if (layout->flags & window_flags::WINDOW_HIDDEN || layout->flags & window_flags::WINDOW_MINIMIZED)
      return;

    layout->menu.x = layout->at_x;
    layout->menu.y = layout->at_y + layout->row.height;
    layout->menu.w = layout->bounds.w;
    layout->menu.offset.x = *layout->offset_x;
    layout->menu.offset.y = *layout->offset_y;
    *layout->offset_y = 0;
  }
  NK_API void
  menubar_end(struct context* ctx) {
    struct window* win;
    struct panel* layout;
    struct command_buffer* out;

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout)
      return;

    win = ctx->current;
    out = &win->buffer;
    layout = win->layout;
    if (layout->flags & window_flags::WINDOW_HIDDEN || layout->flags & window_flags::WINDOW_MINIMIZED)
      return;

    layout->menu.h = layout->at_y - layout->menu.y;
    layout->menu.h += layout->row.height + ctx->style.window.spacing.y;

    layout->bounds.y += layout->menu.h;
    layout->bounds.h -= layout->menu.h;

    *layout->offset_x = layout->menu.offset.x;
    *layout->offset_y = layout->menu.offset.y;
    layout->at_y = layout->bounds.y - layout->row.height;

    layout->clip.y = layout->bounds.y;
    layout->clip.h = layout->bounds.h;
    push_scissor(out, layout->clip);
  }
  INTERN int
  menu_begin(struct context* ctx, struct window* win,
             const char* id, int is_clicked, struct rectf header, struct vec2f size) {
    int is_open = 0;
    int is_active = 0;
    struct rectf body;
    window* popup;
    hash hsh = murmur_hash(id, (int) strlen(id), static_cast<hash>(panel_type::PANEL_MENU));

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout)
      return 0;

    body.x = header.x;
    body.w = size.x;
    body.y = header.y + header.h;
    body.h = size.y;

    popup = win->popup.win;
    is_open = popup ? true : false;
    is_active = (popup && (win->popup.name == hsh) && win->popup.type == panel_type::PANEL_MENU);
    if ((is_clicked && is_open && !is_active) || (is_open && !is_active) ||
        (!is_open && !is_active && !is_clicked))
      return 0;
    if (!nonblock_begin(ctx, static_cast<flag>(panel_flags::WINDOW_NO_SCROLLBAR), body, header, panel_type::PANEL_MENU))
      return 0;

    win->popup.type = panel_type::PANEL_MENU;
    win->popup.name = hsh;
    return 1;
  }
  NK_API bool
  menu_begin_text(struct context* ctx, const char* title, int len,
                  flag align, struct vec2f size) {
    struct window* win;
    const struct input* in;
    struct rectf header;
    int is_clicked = false;
    flag state;

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout)
      return 0;

    win = ctx->current;
    state = widget(&header, ctx);
    if (!state)
      return 0;
    in = (state == NK_WIDGET_ROM || state == NK_WIDGET_DISABLED || win->flags & window_flags::WINDOW_ROM) ? 0 : &ctx->input;
    if (do_button_text(&ctx->last_widget_state, &win->buffer, header,
                       title, len, align, btn_behavior::BUTTON_DEFAULT, &ctx->style.menu_button, in, ctx->style.font))
      is_clicked = true;
    return menu_begin(ctx, win, title, is_clicked, header, size);
  }
  NK_API bool menu_begin_label(struct context* ctx,
                               const char* text, flag align, struct vec2f size) {
    return menu_begin_text(ctx, text, strlen(text), align, size);
  }
  NK_API bool
  menu_begin_image(struct context* ctx, const char* id, struct image img,
                   struct vec2f size) {
    struct window* win;
    struct rectf header;
    const struct input* in;
    int is_clicked = false;
    flag state;

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout)
      return 0;

    win = ctx->current;
    state = widget(&header, ctx);
    if (!state)
      return 0;
    in = (state == NK_WIDGET_ROM || state == NK_WIDGET_DISABLED || win->layout->flags & window_flags::WINDOW_ROM) ? 0 : &ctx->input;
    if (do_button_image(&ctx->last_widget_state, &win->buffer, header,
                        img, btn_behavior::BUTTON_DEFAULT, &ctx->style.menu_button, in))
      is_clicked = true;
    return menu_begin(ctx, win, id, is_clicked, header, size);
  }
  NK_API bool
  menu_begin_symbol(struct context* ctx, const char* id,
                    enum symbol_type sym, struct vec2f size) {
    window* win;
    const struct input* in;
    rectf header;
    int is_clicked = false;
    flag state;

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout)
      return 0;

    win = ctx->current;
    state = widget(&header, ctx);
    if (!state)
      return 0;
    in = (state == NK_WIDGET_ROM || state == NK_WIDGET_DISABLED || win->layout->flags & window_flags::WINDOW_ROM) ? 0 : &ctx->input;
    if (do_button_symbol(&ctx->last_widget_state, &win->buffer, header,
                         sym, btn_behavior::BUTTON_DEFAULT, &ctx->style.menu_button, in, ctx->style.font))
      is_clicked = true;
    return menu_begin(ctx, win, id, is_clicked, header, size);
  }
  NK_API bool
  menu_begin_image_text(struct context* ctx, const char* title, int len,
                        flag align, struct image img, struct vec2f size) {
    struct window* win;
    struct rectf header;
    const struct input* in;
    int is_clicked = false;
    flag state;

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout)
      return 0;

    win = ctx->current;
    state = widget(&header, ctx);
    if (!state)
      return 0;
    in = (state == NK_WIDGET_ROM || state == NK_WIDGET_DISABLED || win->layout->flags & window_flags::WINDOW_ROM) ? 0 : &ctx->input;
    if (do_button_text_image(&ctx->last_widget_state, &win->buffer,
                             header, img, title, len, align, btn_behavior::BUTTON_DEFAULT, &ctx->style.menu_button,
                             ctx->style.font, in))
      is_clicked = true;
    return menu_begin(ctx, win, title, is_clicked, header, size);
  }
  NK_API bool
  menu_begin_image_label(struct context* ctx,
                         const char* title, flag align, struct image img, struct vec2f size) {
    return menu_begin_image_text(ctx, title, strlen(title), align, img, size);
  }
  NK_API bool
  menu_begin_symbol_text(struct context* ctx, const char* title, int len,
                         flag align, enum symbol_type sym, struct vec2f size) {
    struct window* win;
    struct rectf header;
    const struct input* in;
    int is_clicked = false;
    flag state;

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout)
      return 0;

    win = ctx->current;
    state = widget(&header, ctx);
    if (!state)
      return 0;

    in = (state == NK_WIDGET_ROM || state == NK_WIDGET_DISABLED || win->layout->flags & window_flags::WINDOW_ROM) ? 0 : &ctx->input;
    if (do_button_text_symbol(&ctx->last_widget_state, &win->buffer,
                              header, sym, title, len, align, btn_behavior::BUTTON_DEFAULT, &ctx->style.menu_button,
                              ctx->style.font, in))
      is_clicked = true;
    return menu_begin(ctx, win, title, is_clicked, header, size);
  }
  NK_API bool
  menu_begin_symbol_label(struct context* ctx,
                          const char* title, flag align, enum symbol_type sym, struct vec2f size) {
    return menu_begin_symbol_text(ctx, title, strlen(title), align, sym, size);
  }
  NK_API bool
  menu_item_text(struct context* ctx, const char* title, int len, flag align) {
    return contextual_item_text(ctx, title, len, align);
  }
  NK_API bool
  menu_item_label(struct context* ctx, const char* label, flag align) {
    return contextual_item_label(ctx, label, align);
  }
  NK_API bool
  menu_item_image_label(struct context* ctx, struct image img,
                        const char* label, flag align) {
    return contextual_item_image_label(ctx, img, label, align);
  }
  NK_API bool
  menu_item_image_text(struct context* ctx, struct image img,
                       const char* text, int len, flag align) {
    return contextual_item_image_text(ctx, img, text, len, align);
  }
  NK_API bool menu_item_symbol_text(struct context* ctx, enum symbol_type sym,
                                    const char* text, int len, flag align) {
    return contextual_item_symbol_text(ctx, sym, text, len, align);
  }
  NK_API bool menu_item_symbol_label(struct context* ctx, enum symbol_type sym,
                                     const char* label, flag align) {
    return contextual_item_symbol_label(ctx, sym, label, align);
  }
  NK_API void menu_close(struct context* ctx) {
    contextual_close(ctx);
  }
  NK_API void
  menu_end(struct context* ctx) {
    contextual_end(ctx);
  }
} // namespace nk
