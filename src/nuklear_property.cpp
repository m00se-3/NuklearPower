#include "nuklear.h"
#include "nuklear_internal.h"
#include <cstring>

namespace nk {
  /* ===============================================================
   *
   *                              PROPERTY
   *
   * ===============================================================*/
#ifndef NK_DTOA
#define NK_DTOA dtoa
  NK_LIB char*
  dtoa(char *s, double n)
  {
    int useExp = 0;
    int digit = 0, m = 0, m1 = 0;
    char *c = s;
    int neg = 0;

    NK_ASSERT(s);
    if (!s) return 0;

    if (n == 0.0) {
      s[0] = '0'; s[1] = '\0';
      return s;
    }

    neg = (n < 0);
    if (neg) n = -n;

    /* calculate magnitude */
    m = log10(n);
    useExp = (m >= 14 || (neg && m >= 9) || m <= -9);
    if (neg) *(c++) = '-';

    /* set up for scientific notation */
    if (useExp) {
      if (m < 0)
        m -= 1;
      n = n / (double)pow(10.0, m);
      m1 = m;
      m = 0;
    }
    if (m < 1.0) {
      m = 0;
    }

    /* convert the number */
    while (n > NK_FLOAT_PRECISION || m >= 0) {
      double weight = pow(10.0, m);
      if (weight > 0) {
        double t = (double)n / weight;
        digit = ifloord(t);
        n -= ((double)digit * weight);
        *(c++) = (char)('0' + (char)digit);
      }
      if (m == 0 && n > 0)
        *(c++) = '.';
      m--;
    }

    if (useExp) {
      /* convert the exponent */
      int i, j;
      *(c++) = 'e';
      if (m1 > 0) {
        *(c++) = '+';
      } else {
        *(c++) = '-';
        m1 = -m1;
      }
      m = 0;
      while (m1 > 0) {
        *(c++) = (char)('0' + (char)(m1 % 10));
        m1 /= 10;
        m++;
      }
      c -= m;
      for (i = 0, j = m-1; i<j; i++, j--) {
        /* swap without temporary */
        c[i] ^= c[j];
        c[j] ^= c[i];
        c[i] ^= c[j];
      }
      c += m;
    }
    *(c) = '\0';
    return s;
  }
#endif

  NK_LIB void
  drag_behavior(flag *state, const struct input *in,
      rectf drag, struct property_variant *variant,
      float inc_per_pixel)
  {
    int left_mouse_down = in && in->mouse.buttons[NK_BUTTON_LEFT].down;
    int left_mouse_click_in_cursor = in &&
        input_has_mouse_click_down_in_rect(in, NK_BUTTON_LEFT, drag, true);

    nk::widget_state_reset(state);
    if (input_is_mouse_hovering_rect(in, drag))
      *state = NK_WIDGET_STATE_HOVERED;

    if (left_mouse_down && left_mouse_click_in_cursor) {
      float delta, pixels;
      pixels = in->mouse.delta.x;
      delta = pixels * inc_per_pixel;
      switch (variant->kind) {
        default: break;
        case NK_PROPERTY_INT:
          variant->value.i = variant->value.i + (int)delta;
          variant->value.i = NK_CLAMP(variant->min_value.i, variant->value.i, variant->max_value.i);
          break;
        case NK_PROPERTY_FLOAT:
          variant->value.f = variant->value.f + (float)delta;
          variant->value.f = NK_CLAMP(variant->min_value.f, variant->value.f, variant->max_value.f);
          break;
        case NK_PROPERTY_DOUBLE:
          variant->value.d = variant->value.d + (double)delta;
          variant->value.d = NK_CLAMP(variant->min_value.d, variant->value.d, variant->max_value.d);
          break;
      }
      *state = NK_WIDGET_STATE_ACTIVE;
    }
    if (*state & NK_WIDGET_STATE_HOVER && !input_is_mouse_prev_hovering_rect(in, drag))
      *state |= NK_WIDGET_STATE_ENTERED;
    else if (input_is_mouse_prev_hovering_rect(in, drag))
      *state |= NK_WIDGET_STATE_LEFT;
  }
  NK_LIB void
  property_behavior(flag *ws, const struct input *in,
      rectf property,  rectf label, rectf edit,
      rectf empty, int *state, struct property_variant *variant,
      float inc_per_pixel)
  {
    nk::widget_state_reset(ws);
    if (in && *state == NK_PROPERTY_DEFAULT) {
      if (button_behavior(ws, edit, in, btn_behavior::BUTTON_DEFAULT))
        *state = NK_PROPERTY_EDIT;
      else if (input_is_mouse_click_down_in_rect(in, NK_BUTTON_LEFT, label, true))
        *state = NK_PROPERTY_DRAG;
      else if (input_is_mouse_click_down_in_rect(in, NK_BUTTON_LEFT, empty, true))
        *state = NK_PROPERTY_DRAG;
    }
    if (*state == NK_PROPERTY_DRAG) {
      drag_behavior(ws, in, property, variant, inc_per_pixel);
      if (!(*ws & NK_WIDGET_STATE_ACTIVED)) *state = NK_PROPERTY_DEFAULT;
    }
  }
  NK_LIB void
  draw_property(struct command_buffer *out, const struct style_property *style,
      const rectf *bounds, const rectf *label, flag state,
      const char *name, int len, const struct user_font *font)
  {
    struct text text;
    const struct style_item *background;

    /* select correct background and text color */
    if (state & NK_WIDGET_STATE_ACTIVED) {
      background = &style->active;
      text.txt = style->label_active;
    } else if (state & NK_WIDGET_STATE_HOVER) {
      background = &style->hover;
      text.txt = style->label_hover;
    } else {
      background = &style->normal;
      text.txt = style->label_normal;
    }

    text.txt = rgb_factor(text.txt, style->color_factor);

    /* draw background */
    switch(background->type) {
      case style_item_type::STYLE_ITEM_IMAGE:
        text.background = rgba(0, 0, 0, 0);
        draw_image(out, *bounds, &background->data.image, rgb_factor(white, style->color_factor));
        break;
      case style_item_type::STYLE_ITEM_NINE_SLICE:
        text.background = rgba(0, 0, 0, 0);
        draw_nine_slice(out, *bounds, &background->data.slice, rgb_factor(white, style->color_factor));
        break;
      case style_item_type::STYLE_ITEM_COLOR:
        text.background = background->data.color;
        fill_rect(out, *bounds, style->rounding, rgb_factor(background->data.color, style->color_factor));
        stroke_rect(out, *bounds, style->rounding, style->border, rgb_factor(style->border_color, style->color_factor));
        break;
    }

    /* draw label */
    text.padding = vec2_from_floats(0,0);
    if (name && name[0] != '#') {
      widget_text(out, *label, name, len, &text, NK_TEXT_CENTERED, font);
    }
  }
  NK_LIB void
  do_property(flag *ws,
      struct command_buffer *out, rectf property,
      const char *name, struct property_variant *variant,
      float inc_per_pixel, char *buffer, int *len,
      int *state, int *cursor, int *select_begin, int *select_end,
      const struct style_property *style,
      enum property_filter filter, struct input *in,
      const struct user_font *font, struct text_edit *text_edit,
      btn_behavior behavior)
  {
    const plugin_filter filters[] = {
      filter_decimal,
      filter_float
  };
    bool active, old;
    int num_len = 0, name_len = 0;
    char string[NK_MAX_NUMBER_BUFFER];
    float size;

    char *dst = 0;
    int *length;

    rectf left;
    rectf right;
    rectf label;
    rectf edit;
    rectf empty;

    /* left decrement button */
    left.h = font->height/2;
    left.w = left.h;
    left.x = property.x + style->border + style->padding.x;
    left.y = property.y + style->border + property.h/2.0f - left.h/2;

    /* text label */
    if (name && name[0] != '#') {
      name_len = strlen(name);
    }
    size = font->width(font->userdata, font->height, name, name_len);
    label.x = left.x + left.w + style->padding.x;
    label.w = (float)size + 2 * style->padding.x;
    label.y = property.y + style->border + style->padding.y;
    label.h = property.h - (2 * style->border + 2 * style->padding.y);

    /* right increment button */
    right.y = left.y;
    right.w = left.w;
    right.h = left.h;
    right.x = property.x + property.w - (right.w + style->padding.x);

    /* edit */
    if (*state == NK_PROPERTY_EDIT) {
      size = font->width(font->userdata, font->height, buffer, *len);
      size += style->edit.cursor_size;
      length = len;
      dst = buffer;
    } else {
      switch (variant->kind) {
        default: break;
        case NK_PROPERTY_INT:
          itoa(string, variant->value.i);
          num_len = strlen(string);
          break;
        case NK_PROPERTY_FLOAT:
          NK_DTOA(string, (double)variant->value.f);
          num_len = string_float_limit(string, NK_MAX_FLOAT_PRECISION);
          break;
        case NK_PROPERTY_DOUBLE:
          NK_DTOA(string, variant->value.d);
          num_len = string_float_limit(string, NK_MAX_FLOAT_PRECISION);
          break;
      }
      size = font->width(font->userdata, font->height, string, num_len);
      dst = string;
      length = &num_len;
    }

    edit.w =  (float)size + 2 * style->padding.x;
    edit.w = NK_MIN(edit.w, right.x - (label.x + label.w));
    edit.x = right.x - (edit.w + style->padding.x);
    edit.y = property.y + style->border;
    edit.h = property.h - (2 * style->border);

    /* empty left space activator */
    empty.w = edit.x - (label.x + label.w);
    empty.x = label.x + label.w;
    empty.y = property.y;
    empty.h = property.h;

    /* update property */
    old = (*state == NK_PROPERTY_EDIT);
    property_behavior(ws, in, property, label, edit, empty, state, variant, inc_per_pixel);

    /* draw property */
    if (style->draw_begin) style->draw_begin(out, style->userdata);
    draw_property(out, style, &property, &label, *ws, name, name_len, font);
    if (style->draw_end) style->draw_end(out, style->userdata);

    /* execute right button  */
    if (do_button_symbol(ws, out, left, style->sym_left, behavior, &style->dec_button, in, font)) {
      switch (variant->kind) {
        default: break;
        case NK_PROPERTY_INT:
          variant->value.i = NK_CLAMP(variant->min_value.i, variant->value.i - variant->step.i, variant->max_value.i); break;
        case NK_PROPERTY_FLOAT:
          variant->value.f = NK_CLAMP(variant->min_value.f, variant->value.f - variant->step.f, variant->max_value.f); break;
        case NK_PROPERTY_DOUBLE:
          variant->value.d = NK_CLAMP(variant->min_value.d, variant->value.d - variant->step.d, variant->max_value.d); break;
      }
    }
    /* execute left button  */
    if (do_button_symbol(ws, out, right, style->sym_right, behavior, &style->inc_button, in, font)) {
      switch (variant->kind) {
        default: break;
        case NK_PROPERTY_INT:
          variant->value.i = NK_CLAMP(variant->min_value.i, variant->value.i + variant->step.i, variant->max_value.i); break;
        case NK_PROPERTY_FLOAT:
          variant->value.f = NK_CLAMP(variant->min_value.f, variant->value.f + variant->step.f, variant->max_value.f); break;
        case NK_PROPERTY_DOUBLE:
          variant->value.d = NK_CLAMP(variant->min_value.d, variant->value.d + variant->step.d, variant->max_value.d); break;
      }
    }
    if (old != NK_PROPERTY_EDIT && (*state == NK_PROPERTY_EDIT)) {
      /* property has been activated so setup buffer */
      std::memcpy(buffer, dst, (std::size_t)*length);
      *cursor = utf_len(buffer, *length);
      *len = *length;
      length = len;
      dst = buffer;
      active = 0;
    } else active = (*state == NK_PROPERTY_EDIT);

    /* execute and run text edit field */
    textedit_clear_state(text_edit, text_edit_type::TEXT_EDIT_SINGLE_LINE, filters[filter]);
    text_edit->active = (unsigned char)active;
    text_edit->string.len = *length;
    text_edit->cursor = NK_CLAMP(0, *cursor, *length);
    text_edit->select_start = NK_CLAMP(0,*select_begin, *length);
    text_edit->select_end = NK_CLAMP(0,*select_end, *length);
    text_edit->string.buffer.allocated = (std::size_t)*length;
    text_edit->string.buffer.memory.size = NK_MAX_NUMBER_BUFFER;
    text_edit->string.buffer.memory.ptr = dst;
    text_edit->string.buffer.size = NK_MAX_NUMBER_BUFFER;
    text_edit->mode = static_cast<unsigned char>(text_edit_mode::TEXT_EDIT_MODE_INSERT);
    do_edit(ws, out, edit, static_cast<int>(edit_types::EDIT_FIELD) | static_cast<int>(edit_flags::EDIT_AUTO_SELECT),
        filters[filter], text_edit, &style->edit, (*state == NK_PROPERTY_EDIT) ? in: 0, font);

    *length = text_edit->string.len;
    *cursor = text_edit->cursor;
    *select_begin = text_edit->select_start;
    *select_end = text_edit->select_end;
    if (text_edit->active && input_is_key_pressed(in, NK_KEY_ENTER))
      text_edit->active = false;

    if (active && !text_edit->active) {
      /* property is now not active so convert edit text to value*/
      *state = NK_PROPERTY_DEFAULT;
      buffer[*len] = '\0';
      switch (variant->kind) {
        default: break;
        case NK_PROPERTY_INT:
          variant->value.i = strtoi(buffer, 0);
          variant->value.i = NK_CLAMP(variant->min_value.i, variant->value.i, variant->max_value.i);
          break;
        case NK_PROPERTY_FLOAT:
          string_float_limit(buffer, NK_MAX_FLOAT_PRECISION);
          variant->value.f = strtof(buffer, 0);
          variant->value.f = NK_CLAMP(variant->min_value.f, variant->value.f, variant->max_value.f);
          break;
        case NK_PROPERTY_DOUBLE:
          string_float_limit(buffer, NK_MAX_FLOAT_PRECISION);
          variant->value.d = strtod(buffer, 0);
          variant->value.d = NK_CLAMP(variant->min_value.d, variant->value.d, variant->max_value.d);
          break;
      }
    }
  }
  NK_LIB struct property_variant
  property_variant_int(int value, int min_value, int max_value, int step)
  {
    struct property_variant result;
    result.kind = NK_PROPERTY_INT;
    result.value.i = value;
    result.min_value.i = min_value;
    result.max_value.i = max_value;
    result.step.i = step;
    return result;
  }
  NK_LIB struct property_variant
  property_variant_float(float value, float min_value, float max_value, float step)
  {
    struct property_variant result;
    result.kind = NK_PROPERTY_FLOAT;
    result.value.f = value;
    result.min_value.f = min_value;
    result.max_value.f = max_value;
    result.step.f = step;
    return result;
  }
  NK_LIB struct property_variant
  property_variant_double(double value, double min_value, double max_value,
      double step)
  {
    struct property_variant result;
    result.kind = NK_PROPERTY_DOUBLE;
    result.value.d = value;
    result.min_value.d = min_value;
    result.max_value.d = max_value;
    result.step.d = step;
    return result;
  }
  NK_LIB void
  property(struct context *ctx, const char *name, struct property_variant *variant,
      float inc_per_pixel, const enum property_filter filter)
  {
    struct window *win;
    struct panel *layout;
    struct input *in;
    const struct style *style;

    rectf bounds;
    enum widget_layout_states s;

    int *state = 0;
    hash hash = 0;
    char *buffer = 0;
    int *len = 0;
    int *cursor = 0;
    int *select_begin = 0;
    int *select_end = 0;
    int old_state;

    char dummy_buffer[NK_MAX_NUMBER_BUFFER];
    int dummy_state = NK_PROPERTY_DEFAULT;
    int dummy_length = 0;
    int dummy_cursor = 0;
    int dummy_select_begin = 0;
    int dummy_select_end = 0;

    NK_ASSERT(ctx);
    NK_ASSERT(ctx->current);
    NK_ASSERT(ctx->current->layout);
    if (!ctx || !ctx->current || !ctx->current->layout)
      return;

    win = ctx->current;
    layout = win->layout;
    style = &ctx->style;
    s = widget(&bounds, ctx);
    if (!s) return;

    /* calculate hash from name */
    if (name[0] == '#') {
      hash = murmur_hash(name, (int)strlen(name), win->property.seq++);
      name++; /* special number hash */
    } else hash = murmur_hash(name, (int)strlen(name), 42);

    /* check if property is currently hot item */
    if (win->property.active && hash == win->property.name) {
      buffer = win->property.buffer;
      len = &win->property.length;
      cursor = &win->property.cursor;
      state = &win->property.state;
      select_begin = &win->property.select_start;
      select_end = &win->property.select_end;
    } else {
      buffer = dummy_buffer;
      len = &dummy_length;
      cursor = &dummy_cursor;
      state = &dummy_state;
      select_begin =  &dummy_select_begin;
      select_end = &dummy_select_end;
    }

    /* execute property widget */
    old_state = *state;
    ctx->text_edit.clip = ctx->clip;
    in = ((s == NK_WIDGET_ROM && !win->property.active) ||
        layout->flags & window_flags::WINDOW_ROM || s == NK_WIDGET_DISABLED) ? 0 : &ctx->input;
    do_property(&ctx->last_widget_state, &win->buffer, bounds, name,
        variant, inc_per_pixel, buffer, len, state, cursor, select_begin,
        select_end, &style->property, filter, in, style->font, &ctx->text_edit,
        ctx->button_behavior);

    if (in && *state != NK_PROPERTY_DEFAULT && !win->property.active) {
      /* current property is now hot */
      win->property.active = 1;
      std::memcpy(win->property.buffer, buffer, (std::size_t)*len);
      win->property.length = *len;
      win->property.cursor = *cursor;
      win->property.state = *state;
      win->property.name = hash;
      win->property.select_start = *select_begin;
      win->property.select_end = *select_end;
      if (*state == NK_PROPERTY_DRAG) {
        ctx->input.mouse.grab = true;
        ctx->input.mouse.grabbed = true;
      }
    }
    /* check if previously active property is now inactive */
    if (*state == NK_PROPERTY_DEFAULT && old_state != NK_PROPERTY_DEFAULT) {
      if (old_state == NK_PROPERTY_DRAG) {
        ctx->input.mouse.grab = false;
        ctx->input.mouse.grabbed = false;
        ctx->input.mouse.ungrab = true;
      }
      win->property.select_start = 0;
      win->property.select_end = 0;
      win->property.active = 0;
    }
  }
  NK_API void
  property_int(struct context *ctx, const char *name,
      int min, int *val, int max, int step, float inc_per_pixel)
  {
    struct property_variant variant;
    NK_ASSERT(ctx);
    NK_ASSERT(name);
    NK_ASSERT(val);

    if (!ctx || !ctx->current || !name || !val) return;
    variant = property_variant_int(*val, min, max, step);
    property(ctx, name, &variant, inc_per_pixel, NK_FILTER_INT);
    *val = variant.value.i;
  }
  NK_API void
  property_float(struct context *ctx, const char *name,
      float min, float *val, float max, float step, float inc_per_pixel)
  {
    struct property_variant variant;
    NK_ASSERT(ctx);
    NK_ASSERT(name);
    NK_ASSERT(val);

    if (!ctx || !ctx->current || !name || !val) return;
    variant = property_variant_float(*val, min, max, step);
    property(ctx, name, &variant, inc_per_pixel, NK_FILTER_FLOAT);
    *val = variant.value.f;
  }
  NK_API void
  property_double(struct context *ctx, const char *name,
      double min, double *val, double max, double step, float inc_per_pixel)
  {
    struct property_variant variant;
    NK_ASSERT(ctx);
    NK_ASSERT(name);
    NK_ASSERT(val);

    if (!ctx || !ctx->current || !name || !val) return;
    variant = property_variant_double(*val, min, max, step);
    property(ctx, name, &variant, inc_per_pixel, NK_FILTER_FLOAT);
    *val = variant.value.d;
  }
  NK_API int
  propertyi(struct context *ctx, const char *name, int min, int val,
      int max, int step, float inc_per_pixel)
  {
    struct property_variant variant;
    NK_ASSERT(ctx);
    NK_ASSERT(name);

    if (!ctx || !ctx->current || !name) return val;
    variant = property_variant_int(val, min, max, step);
    property(ctx, name, &variant, inc_per_pixel, NK_FILTER_INT);
    val = variant.value.i;
    return val;
  }
  NK_API float
  propertyf(struct context *ctx, const char *name, float min,
      float val, float max, float step, float inc_per_pixel)
  {
    struct property_variant variant;
    NK_ASSERT(ctx);
    NK_ASSERT(name);

    if (!ctx || !ctx->current || !name) return val;
    variant = property_variant_float(val, min, max, step);
    property(ctx, name, &variant, inc_per_pixel, NK_FILTER_FLOAT);
    val = variant.value.f;
    return val;
  }
  NK_API double
  propertyd(struct context *ctx, const char *name, double min,
      double val, double max, double step, float inc_per_pixel)
  {
    struct property_variant variant;
    NK_ASSERT(ctx);
    NK_ASSERT(name);

    if (!ctx || !ctx->current || !name) return val;
    variant = property_variant_double(val, min, max, step);
    property(ctx, name, &variant, inc_per_pixel, NK_FILTER_FLOAT);
    val = variant.value.d;
    return val;
  }
}
