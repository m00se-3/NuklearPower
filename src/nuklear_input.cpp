#include "nuklear.h"
#include "nuklear_internal.h"

namespace nk {
  /* ===============================================================
   *
   *                          INPUT
   *
   * ===============================================================*/
  NK_API void
  input_begin(context* ctx) {
    int i;
    NK_ASSERT(ctx);
    if (!ctx)
      return;
    input* in = &ctx->input;
    for (i = 0; i < NK_BUTTON_MAX; ++i)
      in->mouse.buttons[i].clicked = 0;

    in->keyboard.text_len = 0;
    in->mouse.scroll_delta = vec2_from_floats(0, 0);
    in->mouse.prev.x = in->mouse.pos.x;
    in->mouse.prev.y = in->mouse.pos.y;
    in->mouse.delta.x = 0;
    in->mouse.delta.y = 0;
    for (i = 0; i < NK_KEY_MAX; i++)
      in->keyboard.keys[i].clicked = 0;
  }
  NK_API void
  input_end(context* ctx) {
    NK_ASSERT(ctx);
    if (!ctx)
      return;
    input* in = &ctx->input;
    if (in->mouse.grab)
      in->mouse.grab = 0;
    if (in->mouse.ungrab) {
      in->mouse.grabbed = 0;
      in->mouse.ungrab = 0;
      in->mouse.grab = 0;
    }
  }
  NK_API void
  input_motion(context* ctx, const int x, const int y) {
    NK_ASSERT(ctx);
    if (!ctx)
      return;
    input* in = &ctx->input;
    in->mouse.pos.x = (float) x;
    in->mouse.pos.y = (float) y;
    in->mouse.delta.x = in->mouse.pos.x - in->mouse.prev.x;
    in->mouse.delta.y = in->mouse.pos.y - in->mouse.prev.y;
  }
  NK_API void
  input_key(context* ctx, const keys key, const bool down) {
    NK_ASSERT(ctx);
    if (!ctx)
      return;
    input* in = &ctx->input;
#ifdef NK_KEYSTATE_BASED_INPUT
    if (in->keyboard.keys[key].down != down)
      in->keyboard.keys[key].clicked++;
#else
    in->keyboard.keys[key].clicked++;
#endif
    in->keyboard.keys[key].down = down;
  }
  NK_API void
  input_button(context* ctx, const buttons id, const int x, const int y, const bool down) {
    NK_ASSERT(ctx);
    if (!ctx)
      return;
    input* in = &ctx->input;
    if (in->mouse.buttons[id].down == down)
      return;

    mouse_button* btn = &in->mouse.buttons[id];
    btn->clicked_pos.x = (float) x;
    btn->clicked_pos.y = (float) y;
    btn->down = down;
    btn->clicked++;

    /* Fix Click-Drag for touch events. */
    in->mouse.delta.x = 0;
    in->mouse.delta.y = 0;
#ifdef NK_BUTTON_TRIGGER_ON_RELEASE
    if (down == 1 && id == NK_BUTTON_LEFT) {
      in->mouse.down_pos.x = btn->clicked_pos.x;
      in->mouse.down_pos.y = btn->clicked_pos.y;
    }
#endif
  }
  NK_API void
  input_scroll(context* ctx, const vec2f val) {
    NK_ASSERT(ctx);
    if (!ctx)
      return;
    ctx->input.mouse.scroll_delta.x += val.x;
    ctx->input.mouse.scroll_delta.y += val.y;
  }
  NK_API void
  input_glyph(context* ctx, const glyph glyph) {
    int len = 0;
    rune unicode;

    NK_ASSERT(ctx);
    if (!ctx)
      return;
    input* in = &ctx->input;

    len = utf_decode(glyph, &unicode, NK_UTF_SIZE);
    if (len && ((in->keyboard.text_len + len) < NK_INPUT_MAX)) {
      utf_encode(unicode, &in->keyboard.text[in->keyboard.text_len],
                 NK_INPUT_MAX - in->keyboard.text_len);
      in->keyboard.text_len += len;
    }
  }
  NK_API void
  input_char(context* ctx, const char c) {
    glyph glyph = {0};
    NK_ASSERT(ctx);
    if (!ctx)
      return;
    glyph[0] = c;
    input_glyph(ctx, glyph);
  }
  NK_API void
  input_unicode(context* ctx, const rune unicode) {
    glyph rune;
    NK_ASSERT(ctx);
    if (!ctx)
      return;
    utf_encode(unicode, rune, NK_UTF_SIZE);
    input_glyph(ctx, rune);
  }
  NK_API bool
  input_has_mouse_click(const input* i, const buttons id) {
    if (!i)
      return false;
    const mouse_button* btn = &i->mouse.buttons[id];
    return (btn->clicked && btn->down == false) ? true : false;
  }
  NK_API bool
  input_has_mouse_click_in_rect(const input* i, const buttons id,
                                const rectf b) {
    if (!i)
      return false;
    const mouse_button* btn = &i->mouse.buttons[id];
    if (!NK_INBOX(btn->clicked_pos.x, btn->clicked_pos.y, b.x, b.y, b.w, b.h))
      return false;
    return true;
  }
  NK_API bool
  input_has_mouse_click_in_button_rect(const input* i, const buttons id,
                                       rectf b) {
    if (!i)
      return false;
    const mouse_button* btn = &i->mouse.buttons[id];
#ifdef NK_BUTTON_TRIGGER_ON_RELEASE
    if (!NK_INBOX(btn->clicked_pos.x, btn->clicked_pos.y, b.x, b.y, b.w, b.h) || !NK_INBOX(i->mouse.down_pos.x, i->mouse.down_pos.y, b.x, b.y, b.w, b.h))
#else
    if (!NK_INBOX(btn->clicked_pos.x, btn->clicked_pos.y, b.x, b.y, b.w, b.h))
#endif
      return false;
    return true;
  }
  NK_API bool
  input_has_mouse_click_down_in_rect(const input* i, const buttons id,
                                     const rectf b, const bool down) {
    if (!i)
      return false;
    const mouse_button* btn = &i->mouse.buttons[id];
    return input_has_mouse_click_in_rect(i, id, b) && (btn->down == down);
  }
  NK_API bool
  input_is_mouse_click_in_rect(const input* i, const buttons id,
                               const rectf b) {
    if (!i)
      return false;
    const mouse_button* btn = &i->mouse.buttons[id];
    return (input_has_mouse_click_down_in_rect(i, id, b, false) &&
            btn->clicked)
               ? true
               : false;
  }
  NK_API bool
  input_is_mouse_click_down_in_rect(const input* i, const buttons id,
                                    const rectf b, const bool down) {
    if (!i)
      return false;
    const mouse_button* btn = &i->mouse.buttons[id];
    return (input_has_mouse_click_down_in_rect(i, id, b, down) &&
            btn->clicked)
               ? true
               : false;
  }
  NK_API bool
  input_any_mouse_click_in_rect(const input* in, const rectf b) {
    int down = 0;
    for (int i = 0; i < NK_BUTTON_MAX; ++i)
      down = down || input_is_mouse_click_in_rect(in, (buttons) i, b);
    return down;
  }
  NK_API bool
  input_is_mouse_hovering_rect(const input* i, const rectf rect) {
    if (!i)
      return false;
    return NK_INBOX(i->mouse.pos.x, i->mouse.pos.y, rect.x, rect.y, rect.w, rect.h);
  }
  NK_API bool
  input_is_mouse_prev_hovering_rect(const input* i, const rectf rect) {
    if (!i)
      return false;
    return NK_INBOX(i->mouse.prev.x, i->mouse.prev.y, rect.x, rect.y, rect.w, rect.h);
  }
  NK_API bool
  input_mouse_clicked(const input* i, const buttons id, const rectf rect) {
    if (!i)
      return false;
    if (!input_is_mouse_hovering_rect(i, rect))
      return false;
    return input_is_mouse_click_in_rect(i, id, rect);
  }
  NK_API bool
  input_is_mouse_down(const input* i, const buttons id) {
    if (!i)
      return false;
    return i->mouse.buttons[id].down;
  }
  NK_API bool
  input_is_mouse_pressed(const input* i, const buttons id) {
    if (!i)
      return false;
    const mouse_button* b = &i->mouse.buttons[id];
    if (b->down && b->clicked)
      return true;
    return false;
  }
  NK_API bool
  input_is_mouse_released(const input* i, const buttons id) {
    if (!i)
      return false;
    return (!i->mouse.buttons[id].down && i->mouse.buttons[id].clicked);
  }
  NK_API bool
  input_is_key_pressed(const input* i, const keys key) {
    if (!i)
      return false;
    const struct key* k = &i->keyboard.keys[key];
    if ((k->down && k->clicked) || (!k->down && k->clicked >= 2))
      return true;
    return false;
  }
  NK_API bool
  input_is_key_released(const input* i, const keys key) {
    if (!i)
      return false;
    const struct key* k = &i->keyboard.keys[key];
    if ((!k->down && k->clicked) || (k->down && k->clicked >= 2))
      return true;
    return false;
  }
  NK_API bool
  input_is_key_down(const input* i, const keys key) {
    if (!i)
      return false;
    const struct key* k = &i->keyboard.keys[key];
    if (k->down)
      return true;
    return false;
  }
} // namespace nk
