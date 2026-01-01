#include "nuklear.h"
#include "nuklear_internal.h"

namespace nk {
  /* ===============================================================
   *
   *                          FILTER
   *
   * ===============================================================*/
  NK_API bool
  filter_default(const struct text_edit *box, rune unicode)
  {
    NK_UNUSED(unicode);
    NK_UNUSED(box);
    return true;
  }
  NK_API bool
  filter_ascii(const struct text_edit *box, rune unicode)
  {
    NK_UNUSED(box);
    if (unicode > 128) return false;
    else return true;
  }
  NK_API bool
  filter_float(const struct text_edit *box, rune unicode)
  {
    NK_UNUSED(box);
    if ((unicode < '0' || unicode > '9') && unicode != '.' && unicode != '-')
      return false;
    else return true;
  }
  NK_API bool
  filter_decimal(const struct text_edit *box, rune unicode)
  {
    NK_UNUSED(box);
    if ((unicode < '0' || unicode > '9') && unicode != '-')
      return false;
    else return true;
  }
  NK_API bool
  filter_hex(const struct text_edit *box, rune unicode)
  {
    NK_UNUSED(box);
    if ((unicode < '0' || unicode > '9') &&
        (unicode < 'a' || unicode > 'f') &&
        (unicode < 'A' || unicode > 'F'))
      return false;
    else return true;
  }
  NK_API bool
  filter_oct(const struct text_edit *box, rune unicode)
  {
    NK_UNUSED(box);
    if (unicode < '0' || unicode > '7')
      return false;
    else return true;
  }
  NK_API bool
  filter_binary(const struct text_edit *box, rune unicode)
  {
    NK_UNUSED(box);
    if (unicode != '0' && unicode != '1')
      return false;
    else return true;
  }

  /* ===============================================================
   *
   *                          EDIT
   *
   * ===============================================================*/
  NK_LIB void
  edit_draw_text(struct command_buffer *out,
      const struct style_edit *style, float pos_x, float pos_y,
      float x_offset, const char *text, int byte_len, float row_height,
      const struct user_font *font, struct color background,
      struct color foreground, bool is_selected)
  {
    NK_ASSERT(out);
    NK_ASSERT(font);
    NK_ASSERT(style);
    if (!text || !byte_len || !out || !style) return;

    {int glyph_len = 0;
      rune unicode = 0;
      int text_len = 0;
      float line_width = 0;
      float glyph_width;
      const char *line = text;
      float line_offset = 0;
      int line_count = 0;

      struct text txt;
      txt.padding = vec2_from_floats(0,0);
      txt.background = background;
      txt.txt = foreground;

      foreground = rgb_factor(foreground, style->color_factor);
      background = rgb_factor(background, style->color_factor);

      glyph_len = utf_decode(text+text_len, &unicode, byte_len-text_len);
      if (!glyph_len) return;
      while ((text_len < byte_len) && glyph_len)
      {
        if (unicode == '\n') {
          /* new line separator so draw previous line */
          struct rectf label;
          label.y = pos_y + line_offset;
          label.h = row_height;
          label.w = line_width;
          label.x = pos_x;
          if (!line_count)
            label.x += x_offset;

          if (is_selected) /* selection needs to draw different background color */
            fill_rect(out, label, 0, background);
          widget_text(out, label, line, (int)((text + text_len) - line),
              &txt, NK_TEXT_CENTERED, font);

          text_len++;
          line_count++;
          line_width = 0;
          line = text + text_len;
          line_offset += row_height;
          glyph_len = utf_decode(text + text_len, &unicode, (int)(byte_len-text_len));
          continue;
        }
        if (unicode == '\r') {
          text_len++;
          glyph_len = utf_decode(text + text_len, &unicode, byte_len-text_len);
          continue;
        }
        glyph_width = font->width(font->userdata, font->height, text+text_len, glyph_len);
        line_width += (float)glyph_width;
        text_len += glyph_len;
        glyph_len = utf_decode(text + text_len, &unicode, byte_len-text_len);
        continue;
      }
      if (line_width > 0) {
        /* draw last line */
        struct rectf label;
        label.y = pos_y + line_offset;
        label.h = row_height;
        label.w = line_width;
        label.x = pos_x;
        if (!line_count)
          label.x += x_offset;

        if (is_selected)
          fill_rect(out, label, 0, background);
        widget_text(out, label, line, (int)((text + text_len) - line),
            &txt, NK_TEXT_LEFT, font);
      }}
  }
  NK_LIB flag
  do_edit(flag *state, struct command_buffer *out,
      struct rectf bounds, flag flags, plugin_filter filter,
      struct text_edit *edit, const struct style_edit *style,
      struct input *in, const struct user_font *font)
  {
    struct rectf area;
    flag ret = 0;
    float row_height;
    char prev_state = 0;
    char is_hovered = 0;
    char select_all = 0;
    char cursor_follow = 0;
    struct rectf old_clip;
    struct rectf clip;

    NK_ASSERT(state);
    NK_ASSERT(out);
    NK_ASSERT(style);
    if (!state || !out || !style)
      return ret;

    /* visible text area calculation */
    area.x = bounds.x + style->padding.x + style->border;
    area.y = bounds.y + style->padding.y + style->border;
    area.w = bounds.w - (2.0f * style->padding.x + 2 * style->border);
    area.h = bounds.h - (2.0f * style->padding.y + 2 * style->border);
    if (static_cast<bool>(flags & edit_flags::EDIT_MULTILINE))
      area.w = NK_MAX(0, area.w - style->scrollbar_size.x);
    row_height = (static_cast<bool>(flags & edit_flags::EDIT_MULTILINE))? font->height + style->row_padding: area.h;

    /* calculate clipping rectangle */
    old_clip = out->clip;
    unify(&clip, &old_clip, area.x, area.y, area.x + area.w, area.y + area.h);

    /* update edit state */
    prev_state = (char)edit->active;
    if (in && in->mouse.buttons[NK_BUTTON_LEFT].clicked && in->mouse.buttons[NK_BUTTON_LEFT].down) {
      edit->active = NK_INBOX(in->mouse.pos.x, in->mouse.pos.y,
                              bounds.x, bounds.y, bounds.w, bounds.h);
    }

    /* (de)activate text editor */
    if (!prev_state && edit->active) {
      const enum text_edit_type type = (flags & static_cast<decltype(flags)>(edit_flags::EDIT_MULTILINE)) ? text_edit_type::TEXT_EDIT_MULTI_LINE : text_edit_type::TEXT_EDIT_SINGLE_LINE;
      /* keep scroll position when re-activating edit widget */
      struct vec2f oldscrollbar = edit->scrollbar;
      textedit_clear_state(edit, type, filter);
      edit->scrollbar = oldscrollbar;
      if (flags & static_cast<decltype(flags)>(edit_flags::EDIT_AUTO_SELECT))
        select_all = true;
      if (flags & static_cast<decltype(flags)>(edit_flags::EDIT_GOTO_END_ON_ACTIVATE)) {
        edit->cursor = edit->string.len;
        in = 0;
      }
    } else if (!edit->active) edit->mode = static_cast<unsigned char>(text_edit_mode::TEXT_EDIT_MODE_VIEW);
    if (static_cast<bool>(flags & edit_flags::EDIT_READ_ONLY))
      edit->mode = static_cast<unsigned char>(text_edit_mode::TEXT_EDIT_MODE_VIEW);
    else if (flags & static_cast<decltype(flags)>(flags & edit_flags::EDIT_ALWAYS_INSERT_MODE))
      edit->mode = static_cast<unsigned char>(text_edit_mode::TEXT_EDIT_MODE_INSERT);

    ret = static_cast<flag>(edit->active ? edit_events::EDIT_ACTIVE : edit_events::EDIT_INACTIVE);
    if (prev_state != edit->active)
      ret |= ((edit->active) != 0u) ? std::to_underlying(edit_events::EDIT_ACTIVATED) : std::to_underlying(edit_events::EDIT_DEACTIVATED);

    /* handle user input */
    if (edit->active && in)
    {
      int shift_mod = in->keyboard.keys[NK_KEY_SHIFT].down;
      const float mouse_x = (in->mouse.pos.x - area.x) + edit->scrollbar.x;
      const float mouse_y = (in->mouse.pos.y - area.y) + edit->scrollbar.y;

      /* mouse click handler */
      is_hovered = (char)input_is_mouse_hovering_rect(in, area);
      if (select_all) {
        textedit_select_all(edit);
      } else if (is_hovered && in->mouse.buttons[NK_BUTTON_LEFT].down &&
          in->mouse.buttons[NK_BUTTON_LEFT].clicked) {
        textedit_click(edit, mouse_x, mouse_y, font, row_height);
          } else if (is_hovered && in->mouse.buttons[NK_BUTTON_LEFT].down &&
              (in->mouse.delta.x != 0.0f || in->mouse.delta.y != 0.0f)) {
            textedit_drag(edit, mouse_x, mouse_y, font, row_height);
            cursor_follow = true;
              } else if (is_hovered && in->mouse.buttons[NK_BUTTON_RIGHT].clicked &&
                  in->mouse.buttons[NK_BUTTON_RIGHT].down) {
                textedit_key(edit, NK_KEY_TEXT_WORD_LEFT, false, font, row_height);
                textedit_key(edit, NK_KEY_TEXT_WORD_RIGHT, true, font, row_height);
                cursor_follow = true;
                  }

      {int i; /* keyboard input */
        int old_mode = edit->mode;
        for (i = 0; i < NK_KEY_MAX; ++i) {
          if (i == NK_KEY_ENTER || i == NK_KEY_TAB) continue; /* special case */
          if (input_is_key_pressed(in, (enum keys)i)) {
            textedit_key(edit, (enum keys)i, shift_mod, font, row_height);
            cursor_follow = true;
          }
        }
        if (old_mode != edit->mode) {
          in->keyboard.text_len = 0;
        }}

      /* text input */
      edit->filter = filter;
      if (in->keyboard.text_len) {
        textedit_text(edit, in->keyboard.text, in->keyboard.text_len);
        cursor_follow = true;
        in->keyboard.text_len = 0;
      }

      /* enter key handler */
      if (input_is_key_pressed(in, NK_KEY_ENTER)) {
        cursor_follow = true;
        if (flags & edit_flags::EDIT_CTRL_ENTER_NEWLINE && shift_mod)
          textedit_text(edit, "\n", 1);
        else if (flags & edit_flags::EDIT_SIG_ENTER)
          ret |= std::to_underlying(edit_events::EDIT_COMMITED);
        else textedit_text(edit, "\n", 1);
      }

      /* cut & copy handler */
      {int copy= input_is_key_pressed(in, NK_KEY_COPY);
        int cut = input_is_key_pressed(in, NK_KEY_CUT);
        if ((copy || cut) && (flags & edit_flags::EDIT_CLIPBOARD))
        {
          int glyph_len;
          rune unicode;
          const char *text;
          int b = edit->select_start;
          int e = edit->select_end;

          int begin = NK_MIN(b, e);
          int end = NK_MAX(b, e);
          text = str_at_const(&edit->string, begin, &unicode, &glyph_len);
          if (edit->clip.copy)
            edit->clip.copy(edit->clip.userdata, text, end - begin);
          if (cut && !(flags & edit_flags::EDIT_READ_ONLY)){
            textedit_cut(edit);
            cursor_follow = true;
          }
        }}

      /* paste handler */
      {int paste = input_is_key_pressed(in, NK_KEY_PASTE);
        if (paste && (flags & edit_flags::EDIT_CLIPBOARD) && edit->clip.paste) {
          edit->clip.paste(edit->clip.userdata, edit);
          cursor_follow = true;
        }}

      /* tab handler */
      {int tab = input_is_key_pressed(in, NK_KEY_TAB);
        if (tab && (flags & edit_flags::EDIT_ALLOW_TAB)) {
          textedit_text(edit, "    ", 4);
          cursor_follow = true;
        }}
    }

    /* set widget state */
    if (edit->active)
      *state = NK_WIDGET_STATE_ACTIVE;
    else nk::widget_state_reset(state);

    if (is_hovered)
      *state |= NK_WIDGET_STATE_HOVERED;

    /* DRAW EDIT */
    {const char *text = str_get_const(&edit->string);
      int len = str_len_char(&edit->string);

      {/* select background colors/images  */
      const struct style_item *background;
      if (*state & NK_WIDGET_STATE_ACTIVED)
        background = &style->active;
      else if (*state & NK_WIDGET_STATE_HOVER)
        background = &style->hover;
      else background = &style->normal;

      /* draw background frame */
      switch(background->type) {
        case style_item_type::STYLE_ITEM_IMAGE:
          draw_image(out, bounds, &background->data.image, rgb_factor(white, style->color_factor));
          break;
        case style_item_type::STYLE_ITEM_NINE_SLICE:
          draw_nine_slice(out, bounds, &background->data.slice, rgb_factor(white, style->color_factor));
          break;
        case style_item_type::STYLE_ITEM_COLOR:
          fill_rect(out, bounds, style->rounding, rgb_factor(background->data.color, style->color_factor));
          stroke_rect(out, bounds, style->rounding, style->border, rgb_factor(style->border_color, style->color_factor));
          break;
      }}


      area.w = NK_MAX(0, area.w - style->cursor_size);
      if (edit->active)
      {
        int total_lines = 1;
        struct vec2f text_size = vec2_from_floats(0,0);

        /* text pointer positions */
        const char *cursor_ptr = 0;
        const char *select_begin_ptr = 0;
        const char *select_end_ptr = 0;

        /* 2D pixel positions */
        struct vec2f cursor_pos = vec2_from_floats(0,0);
        struct vec2f selection_offset_start = vec2_from_floats(0,0);
        struct vec2f selection_offset_end = vec2_from_floats(0,0);

        int selection_begin = NK_MIN(edit->select_start, edit->select_end);
        int selection_end = NK_MAX(edit->select_start, edit->select_end);

        /* calculate total line count + total space + cursor/selection position */
        float line_width = 0.0f;
        if (text && len)
        {
          /* utf8 encoding */
          float glyph_width;
          int glyph_len = 0;
          rune unicode = 0;
          int text_len = 0;
          int glyphs = 0;
          int row_begin = 0;

          glyph_len = utf_decode(text, &unicode, len);
          glyph_width = font->width(font->userdata, font->height, text, glyph_len);
          line_width = 0;

          /* iterate all lines */
          while ((text_len < len) && glyph_len)
          {
            /* set cursor 2D position and line */
            if (!cursor_ptr && glyphs == edit->cursor)
            {
              int glyph_offset;
              struct vec2f out_offset;
              struct vec2f row_size;
              const char *remaining;

              /* calculate 2d position */
              cursor_pos.y = (float)(total_lines-1) * row_height;
              row_size = text_calculate_text_bounds(font, text+row_begin,
                          text_len-row_begin, row_height, &remaining,
                          &out_offset, &glyph_offset, NK_STOP_ON_NEW_LINE);
              cursor_pos.x = row_size.x;
              cursor_ptr = text + text_len;
            }

            /* set start selection 2D position and line */
            if (!select_begin_ptr && edit->select_start != edit->select_end &&
                glyphs == selection_begin)
            {
              int glyph_offset;
              struct vec2f out_offset;
              struct vec2f row_size;
              const char *remaining;

              /* calculate 2d position */
              selection_offset_start.y = (float)(NK_MAX(total_lines-1,0)) * row_height;
              row_size = text_calculate_text_bounds(font, text+row_begin,
                          text_len-row_begin, row_height, &remaining,
                          &out_offset, &glyph_offset, NK_STOP_ON_NEW_LINE);
              selection_offset_start.x = row_size.x;
              select_begin_ptr = text + text_len;
            }

            /* set end selection 2D position and line */
            if (!select_end_ptr && edit->select_start != edit->select_end &&
                glyphs == selection_end)
            {
              int glyph_offset;
              struct vec2f out_offset;
              struct vec2f row_size;
              const char *remaining;

              /* calculate 2d position */
              selection_offset_end.y = (float)(total_lines-1) * row_height;
              row_size = text_calculate_text_bounds(font, text+row_begin,
                          text_len-row_begin, row_height, &remaining,
                          &out_offset, &glyph_offset, NK_STOP_ON_NEW_LINE);
              selection_offset_end.x = row_size.x;
              select_end_ptr = text + text_len;
            }
            if (unicode == '\n') {
              text_size.x = NK_MAX(text_size.x, line_width);
              total_lines++;
              line_width = 0;
              text_len++;
              glyphs++;
              row_begin = text_len;
              glyph_len = utf_decode(text + text_len, &unicode, len-text_len);
              glyph_width = font->width(font->userdata, font->height, text+text_len, glyph_len);
              continue;
            }

            glyphs++;
            text_len += glyph_len;
            line_width += (float)glyph_width;

            glyph_len = utf_decode(text + text_len, &unicode, len-text_len);
            glyph_width = font->width(font->userdata, font->height,
                text+text_len, glyph_len);
            continue;
          }
          text_size.y = (float)total_lines * row_height;

          /* handle case when cursor is at end of text buffer */
          if (!cursor_ptr && edit->cursor == edit->string.len) {
            cursor_pos.x = line_width;
            cursor_pos.y = text_size.y - row_height;
          }
        }
        {
          /* scrollbar */
          if (cursor_follow)
          {
            /* update scrollbar to follow cursor */
            if (!(flags & edit_flags::EDIT_NO_HORIZONTAL_SCROLL)) {
              /* horizontal scroll */
              const float scroll_increment = area.w * 0.25f;
              if (cursor_pos.x < edit->scrollbar.x)
                edit->scrollbar.x = (float)(int)NK_MAX(0.0f, cursor_pos.x - scroll_increment);
              if (cursor_pos.x >= edit->scrollbar.x + area.w)
                edit->scrollbar.x = (float)(int)NK_MAX(0.0f, cursor_pos.x - area.w + scroll_increment);
            } else edit->scrollbar.x = 0;

            if (flags & edit_flags::EDIT_MULTILINE) {
              /* vertical scroll: like horizontal, it only adjusts if the
               * cursor leaves the visible area, and then only just enough
               * to keep it visible */
              if (cursor_pos.y < edit->scrollbar.y)
                edit->scrollbar.y = NK_MAX(0.0f, cursor_pos.y);
              if (cursor_pos.y > edit->scrollbar.y + area.h - row_height)
                edit->scrollbar.y = edit->scrollbar.y + row_height;
            } else edit->scrollbar.y = 0;
          }

          /* scrollbar widget */
          if (flags & edit_flags::EDIT_MULTILINE)
          {
            flag ws;
            struct rectf scroll;
            float scroll_target;
            float scroll_offset;
            float scroll_step;
            float scroll_inc;

            scroll = area;
            scroll.x = (bounds.x + bounds.w - style->border) - style->scrollbar_size.x;
            scroll.w = style->scrollbar_size.x;

            scroll_offset = edit->scrollbar.y;
            scroll_step = scroll.h * 0.10f;
            scroll_inc = scroll.h * 0.01f;
            scroll_target = text_size.y;
            edit->scrollbar.y = do_scrollbarv(&ws, out, scroll, is_hovered,
                    scroll_offset, scroll_target, scroll_step, scroll_inc,
                    &style->scrollbar, in, font);
            /* Eat mouse scroll if we're active */
            if (is_hovered && in->mouse.scroll_delta.y != 0.0f) {
              in->mouse.scroll_delta.y = 0;
            }
          }
        }

        /* draw text */
        {struct color background_color;
          struct color text_color;
          struct color sel_background_color;
          struct color sel_text_color;
          struct color cursor_color;
          struct color cursor_text_color;
          const struct style_item *background;
          push_scissor(out, clip);

          /* select correct colors to draw */
          if (*state & NK_WIDGET_STATE_ACTIVED) {
            background = &style->active;
            text_color = style->text_active;
            sel_text_color = style->selected_text_hover;
            sel_background_color = style->selected_hover;
            cursor_color = style->cursor_hover;
            cursor_text_color = style->cursor_text_hover;
          } else if (*state & NK_WIDGET_STATE_HOVER) {
            background = &style->hover;
            text_color = style->text_hover;
            sel_text_color = style->selected_text_hover;
            sel_background_color = style->selected_hover;
            cursor_text_color = style->cursor_text_hover;
            cursor_color = style->cursor_hover;
          } else {
            background = &style->normal;
            text_color = style->text_normal;
            sel_text_color = style->selected_text_normal;
            sel_background_color = style->selected_normal;
            cursor_color = style->cursor_normal;
            cursor_text_color = style->cursor_text_normal;
          }
          if (background->type == style_item_type::STYLE_ITEM_IMAGE)
            background_color = rgba(0,0,0,0);
          else
            background_color = background->data.color;

          cursor_color = rgb_factor(cursor_color, style->color_factor);
          cursor_text_color = rgb_factor(cursor_text_color, style->color_factor);

          if (edit->select_start == edit->select_end) {
            /* no selection so just draw the complete text */
            const char *begin = str_get_const(&edit->string);
            int l = str_len_char(&edit->string);
            edit_draw_text(out, style, area.x - edit->scrollbar.x,
                area.y - edit->scrollbar.y, 0, begin, l, row_height, font,
                background_color, text_color, false);
          } else {
            /* edit has selection so draw 1-3 text chunks */
            if (edit->select_start != edit->select_end && selection_begin > 0){
              /* draw unselected text before selection */
              const char *begin = str_get_const(&edit->string);
              NK_ASSERT(select_begin_ptr);
              edit_draw_text(out, style, area.x - edit->scrollbar.x,
                  area.y - edit->scrollbar.y, 0, begin, (int)(select_begin_ptr - begin),
                  row_height, font, background_color, text_color, false);
            }
            if (edit->select_start != edit->select_end) {
              /* draw selected text */
              NK_ASSERT(select_begin_ptr);
              if (!select_end_ptr) {
                const char *begin = str_get_const(&edit->string);
                select_end_ptr = begin + str_len_char(&edit->string);
              }
              edit_draw_text(out, style,
                  area.x - edit->scrollbar.x,
                  area.y + selection_offset_start.y - edit->scrollbar.y,
                  selection_offset_start.x,
                  select_begin_ptr, (int)(select_end_ptr - select_begin_ptr),
                  row_height, font, sel_background_color, sel_text_color, true);
            }
            if ((edit->select_start != edit->select_end &&
                selection_end < edit->string.len))
            {
              /* draw unselected text after selected text */
              const char *begin = select_end_ptr;
              const char *end = str_get_const(&edit->string) +
                                  str_len_char(&edit->string);
              NK_ASSERT(select_end_ptr);
              edit_draw_text(out, style,
                  area.x - edit->scrollbar.x,
                  area.y + selection_offset_end.y - edit->scrollbar.y,
                  selection_offset_end.x,
                  begin, (int)(end - begin), row_height, font,
                  background_color, text_color, true);
            }
          }

          /* cursor */
          if (edit->select_start == edit->select_end)
          {
            if (edit->cursor >= str_len(&edit->string) ||
                (cursor_ptr && *cursor_ptr == '\n')) {
              /* draw cursor at end of line */
              struct rectf cursor;
              cursor.w = style->cursor_size;
              cursor.h = font->height;
              cursor.x = area.x + cursor_pos.x - edit->scrollbar.x;
              cursor.y = area.y + cursor_pos.y + row_height/2.0f - cursor.h/2.0f;
              cursor.y -= edit->scrollbar.y;
              fill_rect(out, cursor, 0, cursor_color);
                } else {
                  /* draw cursor inside text */
                  int glyph_len;
                  struct rectf label;
                  struct text txt;

                  rune unicode;
                  NK_ASSERT(cursor_ptr);
                  glyph_len = utf_decode(cursor_ptr, &unicode, 4);

                  label.x = area.x + cursor_pos.x - edit->scrollbar.x;
                  label.y = area.y + cursor_pos.y - edit->scrollbar.y;
                  label.w = font->width(font->userdata, font->height, cursor_ptr, glyph_len);
                  label.h = row_height;

                  txt.padding = vec2_from_floats(0,0);
                  txt.background = cursor_color;;
                  txt.txt = cursor_text_color;
                  fill_rect(out, label, 0, cursor_color);
                  widget_text(out, label, cursor_ptr, glyph_len, &txt, NK_TEXT_LEFT, font);
                }
          }}
      } else {
        /* not active so just draw text */
        int l = str_len_char(&edit->string);
        const char *begin = str_get_const(&edit->string);

        const struct style_item *background;
        struct color background_color;
        struct color text_color;
        push_scissor(out, clip);
        if (*state & NK_WIDGET_STATE_ACTIVED) {
          background = &style->active;
          text_color = style->text_active;
        } else if (*state & NK_WIDGET_STATE_HOVER) {
          background = &style->hover;
          text_color = style->text_hover;
        } else {
          background = &style->normal;
          text_color = style->text_normal;
        }
        if (background->type == style_item_type::STYLE_ITEM_IMAGE)
          background_color = rgba(0,0,0,0);
        else
          background_color = background->data.color;

        background_color = rgb_factor(background_color, style->color_factor);
        text_color = rgb_factor(text_color, style->color_factor);

        edit_draw_text(out, style, area.x - edit->scrollbar.x,
            area.y - edit->scrollbar.y, 0, begin, l, row_height, font,
            background_color, text_color, false);
      }
      push_scissor(out, old_clip);}
    return ret;
  }
  NK_API void
  edit_focus(struct context *ctx, flag flags)
  {
    hash hash;
    struct window *win;

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    if (!ctx || !ctx->current) return;

    win = ctx->current;
    hash = win->edit.seq;
    win->edit.active = true;
    win->edit.name = hash;
    if (flags & edit_flags::EDIT_ALWAYS_INSERT_MODE)
      win->edit.mode = static_cast<unsigned char>(text_edit_mode::TEXT_EDIT_MODE_INSERT);
  }
  NK_API void
  edit_unfocus(struct context *ctx)
  {
    struct window *win;
    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    if (!ctx || !ctx->current) return;

    win = ctx->current;
    win->edit.active = false;
    win->edit.name = 0;
  }
  NK_API flag
  edit_string(struct context *ctx, flag flags,
      char *memory, int *len, int max, plugin_filter filter)
  {
    hash hash;
    flag state;
    struct text_edit *edit;
    struct window *win;

    NK_ASSERT(ctx);
    NK_ASSERT(memory);
    NK_ASSERT(len);
    if (!ctx || !memory || !len)
      return 0;

    filter = (!filter) ? filter_default: filter;
    win = ctx->current;
    hash = win->edit.seq;
    edit = &ctx->text_edit;
    textedit_clear_state(&ctx->text_edit, (flags & edit_flags::EDIT_MULTILINE) ? text_edit_type::TEXT_EDIT_MULTI_LINE : text_edit_type::TEXT_EDIT_SINGLE_LINE, filter);

    if (win->edit.active && hash == win->edit.name) {
      if (flags & edit_flags::EDIT_NO_CURSOR)
        edit->cursor = utf_len(memory, *len);
      else edit->cursor = win->edit.cursor;
      if (!(flags & edit_flags::EDIT_SELECTABLE)) {
        edit->select_start = win->edit.cursor;
        edit->select_end = win->edit.cursor;
      } else {
        edit->select_start = win->edit.sel_start;
        edit->select_end = win->edit.sel_end;
      }
      edit->mode = win->edit.mode;
      edit->scrollbar.x = (float)win->edit.scrollbar.x;
      edit->scrollbar.y = (float)win->edit.scrollbar.y;
      edit->active = true;
    } else edit->active = false;

    max = NK_MAX(1, max);
    *len = NK_MIN(*len, max-1);
    str_init_fixed(&edit->string, memory, (std::size_t)max);
    edit->string.buffer.allocated = (std::size_t)*len;
    edit->string.len = utf_len(memory, *len);
    state = edit_buffer(ctx, flags, edit, filter);
    *len = (int)edit->string.buffer.allocated;

    if (edit->active) {
      win->edit.cursor = edit->cursor;
      win->edit.sel_start = edit->select_start;
      win->edit.sel_end = edit->select_end;
      win->edit.mode = edit->mode;
      win->edit.scrollbar.x = (unsigned int)edit->scrollbar.x;
      win->edit.scrollbar.y = (unsigned int)edit->scrollbar.y;
    } return state;
  }
  NK_API flag
  edit_buffer(struct context *ctx, flag flags,
      struct text_edit *edit, plugin_filter filter)
  {
    struct window *win;
    struct style *style;
    struct input *in;

    enum widget_layout_states state;
    struct rectf bounds;

    flag ret_flags = 0;
    unsigned char prev_state;
    hash hash;

    /* make sure correct values */
    NK_ASSERT(ctx);
    NK_ASSERT(edit);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout)
      return 0;

    win = ctx->current;
    style = &ctx->style;
    state = widget(&bounds, ctx);
    if (!state) return state;
    else if (state == NK_WIDGET_DISABLED)
      flags |= edit_flags::EDIT_READ_ONLY;
    in = (win->layout->flags & window_flags::WINDOW_ROM) ? 0 : &ctx->input;

    /* check if edit is currently hot item */
    hash = win->edit.seq++;
    if (win->edit.active && hash == win->edit.name) {
      if (flags & edit_flags::EDIT_NO_CURSOR)
        edit->cursor = edit->string.len;
      if (!(flags & edit_flags::EDIT_SELECTABLE)) {
        edit->select_start = edit->cursor;
        edit->select_end = edit->cursor;
      }
      if (flags & edit_flags::EDIT_CLIPBOARD)
        edit->clip = ctx->clip;
      edit->active = (unsigned char)win->edit.active;
    } else edit->active = false;
    edit->mode = win->edit.mode;

    filter = (!filter) ? filter_default: filter;
    prev_state = (unsigned char)edit->active;
    in = (flags & edit_flags::EDIT_READ_ONLY) ? 0: in;
    ret_flags = do_edit(&ctx->last_widget_state, &win->buffer, bounds, flags,
                    filter, edit, &style->edit, in, style->font);

    if (ctx->last_widget_state & NK_WIDGET_STATE_HOVER)
      ctx->style.cursor_active = ctx->style.cursors[static_cast<unsigned>(style_cursor::CURSOR_TEXT)];
    if (edit->active && prev_state != edit->active) {
      /* current edit is now hot */
      win->edit.active = true;
      win->edit.name = hash;
    } else if (prev_state && !edit->active) {
      /* current edit is now cold */
      win->edit.active = false;
    } return ret_flags;
  }
  NK_API flag
  edit_string_zero_terminated(struct context *ctx, flag flags,
      char *buffer, int max, plugin_filter filter)
  {
    flag result;
    int len = strlen(buffer);
    result = edit_string(ctx, flags, buffer, &len, max, filter);
    buffer[NK_MIN(NK_MAX(max-1,0), len)] = '\0';
    return result;
  }
}
