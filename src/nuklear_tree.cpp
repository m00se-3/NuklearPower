#include "nuklear.h"
#include "nuklear_internal.h"

namespace nk {
  /* ===============================================================
   *
   *                              TREE
   *
   * ===============================================================*/
  INTERN int
  tree_state_base(context* ctx, const tree_type type,
                  struct image* img, const char* title, collapse_states* state) {
    const style_button* button;
    symbol_type symbol;

    rectf header = {0, 0, 0, 0};
    rectf sym = {0, 0, 0, 0};
    text text;

    flag ws = 0;

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout)
      return 0;

    /* cache some data */
    window* win = ctx->current;
    panel* layout = win->layout;
    command_buffer* out = &win->buffer;
    const style* style = &ctx->style;
    const vec2f item_spacing = style->window.spacing;

    /* calculate header bounds and draw background */
    float row_height = style->font->height + 2 * style->tab.padding.y;
    layout_set_min_row_height(ctx, row_height);
    layout_row_dynamic(ctx, row_height, 1);
    layout_reset_min_row_height(ctx);

    const widget_layout_states widget_state = widget(&header, ctx);
    if (type == tree_type::TREE_TAB) {
      const style_item* background = &style->tab.background;

      switch (background->type) {
        case style_item_type::STYLE_ITEM_IMAGE:
          draw_image(out, header, &background->data.image, rgb_factor(white, style->tab.color_factor));
          break;
        case style_item_type::STYLE_ITEM_NINE_SLICE:
          draw_nine_slice(out, header, &background->data.slice, rgb_factor(white, style->tab.color_factor));
          break;
        case style_item_type::STYLE_ITEM_COLOR:
          fill_rect(out, header, 0, rgb_factor(style->tab.border_color, style->tab.color_factor));
          fill_rect(out, shrirect(header, style->tab.border),
                    style->tab.rounding, rgb_factor(background->data.color, style->tab.color_factor));
          break;
      }
    } else
      text.background = style->window.background;

    /* update node state */
    const input* in = (!(layout->flags & window_flags::WINDOW_ROM)) ? &ctx->input : 0;
    in = (in && widget_state == NK_WIDGET_VALID) ? &ctx->input : 0;
    if (button_behavior(&ws, header, in, btn_behavior::BUTTON_DEFAULT))
      *state = (*state == collapse_states::MAXIMIZED) ? collapse_states::MINIMIZED : collapse_states::MAXIMIZED;

    /* select correct button style */
    if (*state == collapse_states::MAXIMIZED) {
      symbol = style->tab.sym_maximize;
      if (type == tree_type::TREE_TAB)
        button = &style->tab.tab_maximize_button;
      else
        button = &style->tab.node_maximize_button;
    } else {
      symbol = style->tab.sym_minimize;
      if (type == tree_type::TREE_TAB)
        button = &style->tab.tab_minimize_button;
      else
        button = &style->tab.node_minimize_button;
    }

    { /* draw triangle button */
      sym.w = sym.h = style->font->height;
      sym.y = header.y + style->tab.padding.y;
      sym.x = header.x + style->tab.padding.x;
      do_button_symbol(&ws, &win->buffer, sym, symbol, btn_behavior::BUTTON_DEFAULT,
                       button, 0, style->font);

      if (img) {
        /* draw optional image icon */
        sym.x = sym.x + sym.w + 4 * item_spacing.x;
        draw_image(&win->buffer, sym, img, white);
        sym.w = style->font->height + style->tab.spacing.x;
      }
    }

    { /* draw label */
      rectf label;
      header.w = NK_MAX(header.w, sym.w + item_spacing.x);
      label.x = sym.x + sym.w + item_spacing.x;
      label.y = sym.y;
      label.w = header.w - (sym.w + item_spacing.y + style->tab.indent);
      label.h = style->font->height;
      text.txt = rgb_factor(style->tab.text, style->tab.color_factor);
      text.padding = vec2_from_floats(0.0f, 0.0f);
      widget_text(out, label, title, strlen(title), &text,
                  NK_TEXT_LEFT, style->font);
    }

    /* increase x-axis cursor widget position pointer */
    if (*state == collapse_states::MAXIMIZED) {
      layout->at_x = header.x + (float) *layout->offset_x + style->tab.indent;
      layout->bounds.w = NK_MAX(layout->bounds.w, style->tab.indent);
      layout->bounds.w -= (style->tab.indent + style->window.padding.x);
      layout->row.tree_depth++;
      return true;
    } else
      return false;
  }
  INTERN int
  tree_base(context* ctx, const tree_type type,
            struct image* img, const char* title, const collapse_states initial_state,
            const char* hash_str, const int len, const int line) {
    window* win = ctx->current;
    int title_len = 0;
    hash tree_hash = 0;
    std::uint32_t* state = 0;

    /* retrieve tree state from internal widget state tables */
    if (!hash_str) {
      title_len = (int) strlen(title);
      tree_hash = murmur_hash(title, (int) title_len, (hash) line);
    } else
      tree_hash = murmur_hash(hash_str, len, (hash) line);
    state = find_value(win, tree_hash);
    if (!state) {
      state = add_value(ctx, win, tree_hash, 0);
      *state = (initial_state == collapse_states::MINIMIZED) ? 0u : 1u;
    }
    return tree_state_base(ctx, type, img, title, (collapse_states*) state);
  }
  NK_API bool
  tree_state_push(context* ctx, const tree_type type,
                  const char* title, collapse_states* state) {
    return tree_state_base(ctx, type, 0, title, state);
  }
  NK_API bool
  tree_state_image_push(context* ctx, const tree_type type,
                        struct image img, const char* title, collapse_states* state) {
    return tree_state_base(ctx, type, &img, title, state);
  }
  NK_API void
  tree_state_pop(context* ctx) {
    const window* win = 0;
    panel* layout = 0;

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout)
      return;

    win = ctx->current;
    layout = win->layout;
    layout->at_x -= ctx->style.tab.indent + (float) *layout->offset_x;
    layout->bounds.w += ctx->style.tab.indent + ctx->style.window.padding.x;
    NK_ASSERT(layout->row.tree_depth);
    layout->row.tree_depth--;
  }
  NK_API bool
  tree_push_hashed(context* ctx, const tree_type type,
                   const char* title, const collapse_states initial_state,
                   const char* hash, const int len, const int line) {
    return tree_base(ctx, type, 0, title, initial_state, hash, len, line);
  }
  NK_API bool
  tree_image_push_hashed(context* ctx, const tree_type type,
                         struct image img, const char* title, const collapse_states initial_state,
                         const char* hash, const int len, const int seed) {
    return tree_base(ctx, type, &img, title, initial_state, hash, len, seed);
  }
  NK_API void
  tree_pop(context* ctx) {
    tree_state_pop(ctx);
  }
  INTERN int
  tree_element_image_push_hashed_base(context* ctx, const tree_type type,
                                      struct image* img, const char* title, const int title_len,
                                      collapse_states* state, bool* selected) {
    const style_button* button;
    symbol_type symbol;

    rectf header = {0, 0, 0, 0};
    rectf sym = {0, 0, 0, 0};

    flag ws = 0;

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout)
      return 0;

    /* cache some data */
    window* win = ctx->current;
    panel* layout = win->layout;
    command_buffer* out = &win->buffer;
    const style* style = &ctx->style;
    const vec2f item_spacing = style->window.spacing;
    const vec2f padding = style->selectable.padding;

    /* calculate header bounds and draw background */
    float row_height = style->font->height + 2 * style->tab.padding.y;
    layout_set_min_row_height(ctx, row_height);
    layout_row_dynamic(ctx, row_height, 1);
    layout_reset_min_row_height(ctx);

    const widget_layout_states widget_state = widget(&header, ctx);
    if (type == tree_type::TREE_TAB) {
      const style_item* background = &style->tab.background;

      switch (background->type) {
        case style_item_type::STYLE_ITEM_IMAGE:
          draw_image(out, header, &background->data.image, rgb_factor(white, style->tab.color_factor));
          break;
        case style_item_type::STYLE_ITEM_NINE_SLICE:
          draw_nine_slice(out, header, &background->data.slice, rgb_factor(white, style->tab.color_factor));
          break;
        case style_item_type::STYLE_ITEM_COLOR:
          fill_rect(out, header, 0, rgb_factor(style->tab.border_color, style->tab.color_factor));
          fill_rect(out, shrirect(header, style->tab.border),
                    style->tab.rounding, rgb_factor(background->data.color, style->tab.color_factor));

          break;
      }
    }

    const input* in = (!(layout->flags & window_flags::WINDOW_ROM)) ? &ctx->input : 0;
    in = (in && widget_state == NK_WIDGET_VALID) ? &ctx->input : 0;

    /* select correct button style */
    if (*state == collapse_states::MAXIMIZED) {
      symbol = style->tab.sym_maximize;
      if (type == tree_type::TREE_TAB)
        button = &style->tab.tab_maximize_button;
      else
        button = &style->tab.node_maximize_button;
    } else {
      symbol = style->tab.sym_minimize;
      if (type == tree_type::TREE_TAB)
        button = &style->tab.tab_minimize_button;
      else
        button = &style->tab.node_minimize_button;
    }
    { /* draw triangle button */
      sym.w = sym.h = style->font->height;
      sym.y = header.y + style->tab.padding.y;
      sym.x = header.x + style->tab.padding.x;
      if (do_button_symbol(&ws, &win->buffer, sym, symbol, btn_behavior::BUTTON_DEFAULT, button, in, style->font))
        *state = (*state == collapse_states::MAXIMIZED) ? collapse_states::MINIMIZED : collapse_states::MAXIMIZED;
    }

    /* draw label */
    {
      flag dummy = 0;
      rectf label;
      /* calculate size of the text and tooltip */
      const int text_len = strlen(title);
      float text_width = style->font->width(style->font->userdata, style->font->height, title, text_len);
      text_width += (4 * padding.x);

      header.w = NK_MAX(header.w, sym.w + item_spacing.x);
      label.x = sym.x + sym.w + item_spacing.x;
      label.y = sym.y;
      label.w = NK_MIN(header.w - (sym.w + item_spacing.y + style->tab.indent), text_width);
      label.h = style->font->height;

      if (img) {
        do_selectable_image(&dummy, &win->buffer, label, title, title_len, NK_TEXT_LEFT,
                            selected, img, &style->selectable, in, style->font);
      } else
        do_selectable(&dummy, &win->buffer, label, title, title_len, NK_TEXT_LEFT,
                      selected, &style->selectable, in, style->font);
    }
    /* increase x-axis cursor widget position pointer */
    if (*state == collapse_states::MAXIMIZED) {
      layout->at_x = header.x + (float) *layout->offset_x + style->tab.indent;
      layout->bounds.w = NK_MAX(layout->bounds.w, style->tab.indent);
      layout->bounds.w -= (style->tab.indent + style->window.padding.x);
      layout->row.tree_depth++;
      return true;
    } else
      return false;
  }
  INTERN int
  tree_element_base(context* ctx, const tree_type type,
                    struct image* img, const char* title, const collapse_states initial_state,
                    bool* selected, const char* hash_str, const int len, const int line) {
    window* win = ctx->current;
    int title_len = 0;
    hash tree_hash = 0;
    std::uint32_t* state = 0;

    /* retrieve tree state from internal widget state tables */
    if (!hash_str) {
      title_len = (int) strlen(title);
      tree_hash = murmur_hash(title, (int) title_len, (hash) line);
    } else
      tree_hash = murmur_hash(hash_str, len, (hash) line);
    state = find_value(win, tree_hash);
    if (!state) {
      state = add_value(ctx, win, tree_hash, 0);
      *state = (initial_state == collapse_states::MINIMIZED) ? 0u : 1u;
    }
    return tree_element_image_push_hashed_base(ctx, type, img, title,
                                               strlen(title), (collapse_states*) state, selected);
  }
  NK_API bool
  tree_element_push_hashed(context* ctx, const tree_type type,
                           const char* title, const collapse_states initial_state,
                           bool* selected, const char* hash, const int len, const int seed) {
    return tree_element_base(ctx, type, 0, title, initial_state, selected, hash, len, seed);
  }
  NK_API bool
  tree_element_image_push_hashed(context* ctx, const tree_type type,
                                 struct image img, const char* title, const collapse_states initial_state,
                                 bool* selected, const char* hash, const int len, const int seed) {
    return tree_element_base(ctx, type, &img, title, initial_state, selected, hash, len, seed);
  }
  NK_API void
  tree_element_pop(context* ctx) {
    tree_state_pop(ctx);
  }
} // namespace nk
