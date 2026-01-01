#include "nuklear.h"
#include "nuklear_internal.h"

namespace nk {
  /* ===============================================================
   *
   *                              STYLE
   *
   * ===============================================================*/
  NK_API void style_default(context* ctx) { style_from_table(ctx, 0); }
#define NK_COLOR_MAP(NK_COLOR)                                                                        \
  NK_COLOR(NK_COLOR_TEXT, 175, 175, 175, 255)                                                         \
  NK_COLOR(NK_COLOR_WINDOW, 45, 45, 45, 255)                                                          \
  NK_COLOR(static_cast<std::size_t>(style_colors::COLOR_HEADER), 40, 40, 40, 255)                     \
  NK_COLOR(static_cast<std::size_t>(style_colors::COLOR_BORDER), 65, 65, 65, 255)                     \
  NK_COLOR(NK_COLOR_BUTTON, 50, 50, 50, 255)                                                          \
  NK_COLOR(NK_COLOR_BUTTON_HOVER, 40, 40, 40, 255)                                                    \
  NK_COLOR(NK_COLOR_BUTTON_ACTIVE, 35, 35, 35, 255)                                                   \
  NK_COLOR(static_cast<std::size_t>(style_colors::COLOR_TOGGLE), 100, 100, 100, 255)                  \
  NK_COLOR(static_cast<std::size_t>(style_colors::COLOR_TOGGLE_HOVER), 120, 120, 120, 255)            \
  NK_COLOR(static_cast<std::size_t>(style_colors::COLOR_TOGGLE_CURSOR), 45, 45, 45, 255)              \
  NK_COLOR(static_cast<std::size_t>(style_colors::COLOR_SELECT), 45, 45, 45, 255)                     \
  NK_COLOR(static_cast<std::size_t>(style_colors::COLOR_SELECT_ACTIVE), 35, 35, 35, 255)              \
  NK_COLOR(static_cast<std::size_t>(style_colors::COLOR_SLIDER), 38, 38, 38, 255)                     \
  NK_COLOR(static_cast<std::size_t>(style_colors::COLOR_SLIDER_CURSOR), 100, 100, 100, 255)           \
  NK_COLOR(static_cast<std::size_t>(style_colors::COLOR_SLIDER_CURSOR_HOVER), 120, 120, 120, 255)     \
  NK_COLOR(static_cast<std::size_t>(style_colors::COLOR_SLIDER_CURSOR_ACTIVE), 150, 150, 150, 255)    \
  NK_COLOR(static_cast<std::size_t>(style_colors::COLOR_PROPERTY), 38, 38, 38, 255)                   \
  NK_COLOR(static_cast<std::size_t>(style_colors::COLOR_EDIT), 38, 38, 38, 255)                       \
  NK_COLOR(static_cast<std::size_t>(style_colors::COLOR_EDIT) _CURSOR, 175, 175, 175, 255)            \
  NK_COLOR(static_cast<std::size_t>(style_colors::COLOR_COMBO), 45, 45, 45, 255)                      \
  NK_COLOR(static_cast<std::size_t>(style_colors::COLOR_CHART), 120, 120, 120, 255)                   \
  NK_COLOR(static_cast<std::size_t>(style_colors::COLOR_CHART_COLOR), 45, 45, 45, 255)                \
  NK_COLOR(static_cast<std::size_t>(style_colors::COLOR_CHART_COLOR_HIGHLIGHT), 255, 0, 0, 255)       \
  NK_COLOR(static_cast<std::size_t>(style_colors::COLOR_SCROLLBAR), 40, 40, 40, 255)                  \
  NK_COLOR(static_cast<std::size_t>(style_colors::COLOR_SCROLLBAR_CURSOR), 100, 100, 100, 255)        \
  NK_COLOR(static_cast<std::size_t>(style_colors::COLOR_SCROLLBAR_CURSOR_HOVER), 120, 120, 120, 255)  \
  NK_COLOR(static_cast<std::size_t>(style_colors::COLOR_SCROLLBAR_CURSOR_ACTIVE), 150, 150, 150, 255) \
  NK_COLOR(static_cast<std::size_t>(style_colors::COLOR_TAB_HEADER), 40, 40, 40, 255)                 \
  NK_COLOR(static_cast<std::size_t>(style_colors::COLOR_KNOB), 38, 38, 38, 255)                       \
  NK_COLOR(static_cast<std::size_t>(style_colors::COLOR_KNOB_CURSOR), 100, 100, 100, 255)             \
  NK_COLOR(static_cast<std::size_t>(style_colors::COLOR_KNOB_CURSOR_HOVER), 120, 120, 120, 255)       \
  NK_COLOR(static_cast<std::size_t>(style_colors::COLOR_KNOB_CURSOR_ACTIVE), 150, 150, 150, 255)

  NK_GLOBAL const color
      default_color_style[static_cast<std::size_t>(style_colors::COLOR_COUNT)] = {
#define NK_COLOR(a, b, c, d, e) {b, c, d, e},
          NK_COLOR_MAP(NK_COLOR)
#undef NK_COLOR
  };
  NK_GLOBAL const char* color_names[static_cast<std::size_t>(style_colors::COLOR_COUNT)] = {
#define NK_COLOR(a, b, c, d, e) #a,
      NK_COLOR_MAP(NK_COLOR)
#undef NK_COLOR
  };

  NK_API const char*
  style_get_color_by_name(style_colors c) {
    return color_names[static_cast<std::size_t>(c)];
  }
  NK_API style_item
  style_item_color(const color col) {
    style_item i;
    i.type = style_item_type::STYLE_ITEM_COLOR;
    i.data.color = col;
    return i;
  }
  NK_API style_item
  style_item_image(struct image img) {
    style_item i;
    i.type = style_item_type::STYLE_ITEM_IMAGE;
    i.data.image = img;
    return i;
  }
  NK_API style_item
  style_item_nine_slice(nine_slice slice) {
    style_item i;
    i.type = style_item_type::STYLE_ITEM_NINE_SLICE;
    i.data.slice = slice;
    return i;
  }
  NK_API style_item
  style_item_hide(void) {
    style_item i;
    i.type = style_item_type::STYLE_ITEM_COLOR;
    i.data.color = rgba(0, 0, 0, 0);
    return i;
  }
  NK_API void
  style_from_table(context* ctx, const color* table) {

    NK_ASSERT(ctx);
    if (!ctx)
      return;
    style* style = &ctx->style;
    table = (!table) ? default_color_style : table;

    /* default text */
    style_text* text = &style->text;
    text->color = table[static_cast<std::size_t>(style_colors::COLOR_TEXT)];
    text->padding = vec2_from_floats(0, 0);
    text->color_factor = 1.0f;
    text->disabled_factor = NK_WIDGET_DISABLED_FACTOR;

    /* default button */
    style_button* button = &style->button;
    zero_struct(*button);
    button->normal = style_item_color(table[static_cast<std::size_t>(style_colors::COLOR_BUTTON)]);
    button->hover = style_item_color(table[static_cast<std::size_t>(style_colors::COLOR_BUTTON_HOVER)]);
    button->active = style_item_color(table[static_cast<std::size_t>(style_colors::COLOR_BUTTON_ACTIVE)]);
    button->border_color = table[static_cast<std::size_t>(style_colors::COLOR_BORDER)];
    button->text_background = table[static_cast<std::size_t>(style_colors::COLOR_BUTTON)];
    button->text_normal = table[static_cast<std::size_t>(style_colors::COLOR_TEXT)];
    button->text_hover = table[static_cast<std::size_t>(style_colors::COLOR_TEXT)];
    button->text_active = table[static_cast<std::size_t>(style_colors::COLOR_TEXT)];
    button->padding = vec2_from_floats(2.0f, 2.0f);
    button->image_padding = vec2_from_floats(0.0f, 0.0f);
    button->touch_padding = vec2_from_floats(0.0f, 0.0f);
    button->userdata = handle_ptr(0);
    button->text_alignment = NK_TEXT_CENTERED;
    button->border = 1.0f;
    button->rounding = 4.0f;
    button->color_factor_text = 1.0f;
    button->color_factor_background = 1.0f;
    button->disabled_factor = NK_WIDGET_DISABLED_FACTOR;
    button->draw_begin = 0;
    button->draw_end = 0;

    /* contextual button */
    button = &style->contextual_button;
    zero_struct(*button);
    button->normal = style_item_color(table[static_cast<std::size_t>(style_colors::COLOR_WINDOW)]);
    button->hover = style_item_color(table[static_cast<std::size_t>(style_colors::COLOR_BUTTON_HOVER)]);
    button->active = style_item_color(table[static_cast<std::size_t>(style_colors::COLOR_BUTTON_ACTIVE)]);
    button->border_color = table[static_cast<std::size_t>(style_colors::COLOR_WINDOW)];
    button->text_background = table[static_cast<std::size_t>(style_colors::COLOR_WINDOW)];
    button->text_normal = table[static_cast<std::size_t>(style_colors::COLOR_TEXT)];
    button->text_hover = table[static_cast<std::size_t>(style_colors::COLOR_TEXT)];
    button->text_active = table[static_cast<std::size_t>(style_colors::COLOR_TEXT)];
    button->padding = vec2_from_floats(2.0f, 2.0f);
    button->touch_padding = vec2_from_floats(0.0f, 0.0f);
    button->userdata = handle_ptr(0);
    button->text_alignment = NK_TEXT_CENTERED;
    button->border = 0.0f;
    button->rounding = 0.0f;
    button->color_factor_text = 1.0f;
    button->color_factor_background = 1.0f;
    button->disabled_factor = NK_WIDGET_DISABLED_FACTOR;
    button->draw_begin = 0;
    button->draw_end = 0;

    /* menu button */
    button = &style->menu_button;
    zero_struct(*button);
    button->normal = style_item_color(table[static_cast<std::size_t>(style_colors::COLOR_WINDOW)]);
    button->hover = style_item_color(table[static_cast<std::size_t>(style_colors::COLOR_WINDOW)]);
    button->active = style_item_color(table[static_cast<std::size_t>(style_colors::COLOR_WINDOW)]);
    button->border_color = table[static_cast<std::size_t>(style_colors::COLOR_WINDOW)];
    button->text_background = table[static_cast<std::size_t>(style_colors::COLOR_WINDOW)];
    button->text_normal = table[static_cast<std::size_t>(style_colors::COLOR_TEXT)];
    button->text_hover = table[static_cast<std::size_t>(style_colors::COLOR_TEXT)];
    button->text_active = table[static_cast<std::size_t>(style_colors::COLOR_TEXT)];
    button->padding = vec2_from_floats(2.0f, 2.0f);
    button->touch_padding = vec2_from_floats(0.0f, 0.0f);
    button->userdata = handle_ptr(0);
    button->text_alignment = NK_TEXT_CENTERED;
    button->border = 0.0f;
    button->rounding = 1.0f;
    button->color_factor_text = 1.0f;
    button->color_factor_background = 1.0f;
    button->disabled_factor = NK_WIDGET_DISABLED_FACTOR;
    button->draw_begin = 0;
    button->draw_end = 0;

    /* checkbox toggle */
    style_toggle* toggle = &style->checkbox;
    zero_struct(*toggle);
    toggle->normal = style_item_color(table[static_cast<std::size_t>(style_colors::COLOR_TOGGLE)]);
    toggle->hover = style_item_color(table[static_cast<std::size_t>(style_colors::COLOR_TOGGLE_HOVER)]);
    toggle->active = style_item_color(table[static_cast<std::size_t>(style_colors::COLOR_TOGGLE_HOVER)]);
    toggle->cursor_normal = style_item_color(table[static_cast<std::size_t>(style_colors::COLOR_TOGGLE_CURSOR)]);
    toggle->cursor_hover = style_item_color(table[static_cast<std::size_t>(style_colors::COLOR_TOGGLE_CURSOR)]);
    toggle->userdata = handle_ptr(0);
    toggle->text_background = table[static_cast<std::size_t>(style_colors::COLOR_WINDOW)];
    toggle->text_normal = table[static_cast<std::size_t>(style_colors::COLOR_TEXT)];
    toggle->text_hover = table[static_cast<std::size_t>(style_colors::COLOR_TEXT)];
    toggle->text_active = table[static_cast<std::size_t>(style_colors::COLOR_TEXT)];
    toggle->padding = vec2_from_floats(2.0f, 2.0f);
    toggle->touch_padding = vec2_from_floats(0, 0);
    toggle->border_color = rgba(0, 0, 0, 0);
    toggle->border = 0.0f;
    toggle->spacing = 4;
    toggle->color_factor = 1.0f;
    toggle->disabled_factor = NK_WIDGET_DISABLED_FACTOR;

    /* option toggle */
    toggle = &style->option;
    zero_struct(*toggle);
    toggle->normal = style_item_color(table[static_cast<std::size_t>(style_colors::COLOR_TOGGLE)]);
    toggle->hover = style_item_color(table[static_cast<std::size_t>(style_colors::COLOR_TOGGLE_HOVER)]);
    toggle->active = style_item_color(table[static_cast<std::size_t>(style_colors::COLOR_TOGGLE_HOVER)]);
    toggle->cursor_normal = style_item_color(table[static_cast<std::size_t>(style_colors::COLOR_TOGGLE_CURSOR)]);
    toggle->cursor_hover = style_item_color(table[static_cast<std::size_t>(style_colors::COLOR_TOGGLE_CURSOR)]);
    toggle->userdata = handle_ptr(0);
    toggle->text_background = table[static_cast<std::size_t>(style_colors::COLOR_WINDOW)];
    toggle->text_normal = table[static_cast<std::size_t>(style_colors::COLOR_TEXT)];
    toggle->text_hover = table[static_cast<std::size_t>(style_colors::COLOR_TEXT)];
    toggle->text_active = table[static_cast<std::size_t>(style_colors::COLOR_TEXT)];
    toggle->padding = vec2_from_floats(3.0f, 3.0f);
    toggle->touch_padding = vec2_from_floats(0, 0);
    toggle->border_color = rgba(0, 0, 0, 0);
    toggle->border = 0.0f;
    toggle->spacing = 4;
    toggle->color_factor = 1.0f;
    toggle->disabled_factor = NK_WIDGET_DISABLED_FACTOR;

    /* selectable */
    style_selectable* select = &style->selectable;
    zero_struct(*select);
    select->normal = style_item_color(table[static_cast<std::size_t>(style_colors::COLOR_SELECT)]);
    select->hover = style_item_color(table[static_cast<std::size_t>(style_colors::COLOR_SELECT)]);
    select->pressed = style_item_color(table[static_cast<std::size_t>(style_colors::COLOR_SELECT)]);
    select->normal_active = style_item_color(table[static_cast<std::size_t>(style_colors::COLOR_SELECT_ACTIVE)]);
    select->hover_active = style_item_color(table[static_cast<std::size_t>(style_colors::COLOR_SELECT_ACTIVE)]);
    select->pressed_active = style_item_color(table[static_cast<std::size_t>(style_colors::COLOR_SELECT_ACTIVE)]);
    select->text_normal = table[static_cast<std::size_t>(style_colors::COLOR_TEXT)];
    select->text_hover = table[static_cast<std::size_t>(style_colors::COLOR_TEXT)];
    select->text_pressed = table[static_cast<std::size_t>(style_colors::COLOR_TEXT)];
    select->text_normal_active = table[static_cast<std::size_t>(style_colors::COLOR_TEXT)];
    select->text_hover_active = table[static_cast<std::size_t>(style_colors::COLOR_TEXT)];
    select->text_pressed_active = table[static_cast<std::size_t>(style_colors::COLOR_TEXT)];
    select->padding = vec2_from_floats(2.0f, 2.0f);
    select->image_padding = vec2_from_floats(2.0f, 2.0f);
    select->touch_padding = vec2_from_floats(0, 0);
    select->userdata = handle_ptr(0);
    select->rounding = 0.0f;
    select->color_factor = 1.0f;
    select->disabled_factor = NK_WIDGET_DISABLED_FACTOR;
    select->draw_begin = 0;
    select->draw_end = 0;

    /* slider */
    style_slider* slider = &style->slider;
    zero_struct(*slider);
    slider->normal = style_item_hide();
    slider->hover = style_item_hide();
    slider->active = style_item_hide();
    slider->bar_normal = table[static_cast<std::size_t>(style_colors::COLOR_SLIDER)];
    slider->bar_hover = table[static_cast<std::size_t>(style_colors::COLOR_SLIDER)];
    slider->bar_active = table[static_cast<std::size_t>(style_colors::COLOR_SLIDER)];
    slider->bar_filled = table[static_cast<std::size_t>(style_colors::COLOR_SLIDER_CURSOR)];
    slider->cursor_normal = style_item_color(table[static_cast<std::size_t>(style_colors::COLOR_SLIDER_CURSOR)]);
    slider->cursor_hover = style_item_color(table[static_cast<std::size_t>(style_colors::COLOR_SLIDER_CURSOR_HOVER)]);
    slider->cursor_active = style_item_color(table[static_cast<std::size_t>(style_colors::COLOR_SLIDER_CURSOR_ACTIVE)]);
    slider->inc_symbol = symbol_type::SYMBOL_TRIANGLE_RIGHT;
    slider->dec_symbol = symbol_type::SYMBOL_TRIANGLE_LEFT;
    slider->cursor_size = vec2_from_floats(16, 16);
    slider->padding = vec2_from_floats(2, 2);
    slider->spacing = vec2_from_floats(2, 2);
    slider->userdata = handle_ptr(0);
    slider->show_buttons = false;
    slider->bar_height = 8;
    slider->rounding = 0;
    slider->color_factor = 1.0f;
    slider->disabled_factor = NK_WIDGET_DISABLED_FACTOR;
    slider->draw_begin = 0;
    slider->draw_end = 0;

    /* slider buttons */
    button = &style->slider.inc_button;
    button->normal = style_item_color(rgb(40, 40, 40));
    button->hover = style_item_color(rgb(42, 42, 42));
    button->active = style_item_color(rgb(44, 44, 44));
    button->border_color = rgb(65, 65, 65);
    button->text_background = rgb(40, 40, 40);
    button->text_normal = rgb(175, 175, 175);
    button->text_hover = rgb(175, 175, 175);
    button->text_active = rgb(175, 175, 175);
    button->padding = vec2_from_floats(8.0f, 8.0f);
    button->touch_padding = vec2_from_floats(0.0f, 0.0f);
    button->userdata = handle_ptr(0);
    button->text_alignment = NK_TEXT_CENTERED;
    button->border = 1.0f;
    button->rounding = 0.0f;
    button->color_factor_text = 1.0f;
    button->color_factor_background = 1.0f;
    button->disabled_factor = NK_WIDGET_DISABLED_FACTOR;
    button->draw_begin = 0;
    button->draw_end = 0;
    style->slider.dec_button = style->slider.inc_button;

    /* knob */
    style_knob* knob = &style->knob;
    zero_struct(*knob);
    knob->normal = style_item_hide();
    knob->hover = style_item_hide();
    knob->active = style_item_hide();
    knob->knob_normal = table[static_cast<std::size_t>(style_colors::COLOR_KNOB)];
    knob->knob_hover = table[static_cast<std::size_t>(style_colors::COLOR_KNOB)];
    knob->knob_active = table[static_cast<std::size_t>(style_colors::COLOR_KNOB)];
    knob->cursor_normal = table[static_cast<std::size_t>(style_colors::COLOR_KNOB_CURSOR)];
    knob->cursor_hover = table[static_cast<std::size_t>(style_colors::COLOR_KNOB_CURSOR_HOVER)];
    knob->cursor_active = table[static_cast<std::size_t>(style_colors::COLOR_KNOB_CURSOR_ACTIVE)];

    knob->knob_border_color = table[static_cast<std::size_t>(style_colors::COLOR_BORDER)];
    knob->knob_border = 1.0f;

    knob->padding = vec2_from_floats(2, 2);
    knob->spacing = vec2_from_floats(2, 2);
    knob->cursor_width = 2;
    knob->color_factor = 1.0f;
    knob->disabled_factor = NK_WIDGET_DISABLED_FACTOR;

    knob->userdata = handle_ptr(0);
    knob->draw_begin = 0;
    knob->draw_end = 0;

    /* progressbar */
    style_progress* prog = &style->progress;
    zero_struct(*prog);
    prog->normal = style_item_color(table[static_cast<std::size_t>(style_colors::COLOR_SLIDER)]);
    prog->hover = style_item_color(table[static_cast<std::size_t>(style_colors::COLOR_SLIDER)]);
    prog->active = style_item_color(table[static_cast<std::size_t>(style_colors::COLOR_SLIDER)]);
    prog->cursor_normal = style_item_color(table[static_cast<std::size_t>(style_colors::COLOR_SLIDER_CURSOR)]);
    prog->cursor_hover = style_item_color(table[static_cast<std::size_t>(style_colors::COLOR_SLIDER_CURSOR_HOVER)]);
    prog->cursor_active = style_item_color(table[static_cast<std::size_t>(style_colors::COLOR_SLIDER_CURSOR_ACTIVE)]);
    prog->border_color = rgba(0, 0, 0, 0);
    prog->cursor_border_color = rgba(0, 0, 0, 0);
    prog->userdata = handle_ptr(0);
    prog->padding = vec2_from_floats(4, 4);
    prog->rounding = 0;
    prog->border = 0;
    prog->cursor_rounding = 0;
    prog->cursor_border = 0;
    prog->color_factor = 1.0f;
    prog->disabled_factor = NK_WIDGET_DISABLED_FACTOR;
    prog->draw_begin = 0;
    prog->draw_end = 0;

    /* scrollbars */
    style_scrollbar* scroll = &style->scrollh;
    zero_struct(*scroll);
    scroll->normal = style_item_color(table[static_cast<std::size_t>(style_colors::COLOR_SCROLLBAR)]);
    scroll->hover = style_item_color(table[static_cast<std::size_t>(style_colors::COLOR_SCROLLBAR)]);
    scroll->active = style_item_color(table[static_cast<std::size_t>(style_colors::COLOR_SCROLLBAR)]);
    scroll->cursor_normal = style_item_color(table[static_cast<std::size_t>(style_colors::COLOR_SCROLLBAR_CURSOR)]);
    scroll->cursor_hover = style_item_color(table[static_cast<std::size_t>(style_colors::COLOR_SCROLLBAR_CURSOR_HOVER)]);
    scroll->cursor_active = style_item_color(table[static_cast<std::size_t>(style_colors::COLOR_SCROLLBAR_CURSOR_ACTIVE)]);
    scroll->dec_symbol = symbol_type::SYMBOL_CIRCLE_SOLID;
    scroll->inc_symbol = symbol_type::SYMBOL_CIRCLE_SOLID;
    scroll->userdata = handle_ptr(0);
    scroll->border_color = table[static_cast<std::size_t>(style_colors::COLOR_SCROLLBAR)];
    scroll->cursor_border_color = table[static_cast<std::size_t>(style_colors::COLOR_SCROLLBAR)];
    scroll->padding = vec2_from_floats(0, 0);
    scroll->show_buttons = false;
    scroll->border = 0;
    scroll->rounding = 0;
    scroll->border_cursor = 0;
    scroll->rounding_cursor = 0;
    scroll->color_factor = 1.0f;
    scroll->disabled_factor = NK_WIDGET_DISABLED_FACTOR;
    scroll->draw_begin = 0;
    scroll->draw_end = 0;
    style->scrollv = style->scrollh;

    /* scrollbars buttons */
    button = &style->scrollh.inc_button;
    button->normal = style_item_color(rgb(40, 40, 40));
    button->hover = style_item_color(rgb(42, 42, 42));
    button->active = style_item_color(rgb(44, 44, 44));
    button->border_color = rgb(65, 65, 65);
    button->text_background = rgb(40, 40, 40);
    button->text_normal = rgb(175, 175, 175);
    button->text_hover = rgb(175, 175, 175);
    button->text_active = rgb(175, 175, 175);
    button->padding = vec2_from_floats(4.0f, 4.0f);
    button->touch_padding = vec2_from_floats(0.0f, 0.0f);
    button->userdata = handle_ptr(0);
    button->text_alignment = NK_TEXT_CENTERED;
    button->border = 1.0f;
    button->rounding = 0.0f;
    button->color_factor_text = 1.0f;
    button->color_factor_background = 1.0f;
    button->disabled_factor = NK_WIDGET_DISABLED_FACTOR;
    button->draw_begin = 0;
    button->draw_end = 0;
    style->scrollh.dec_button = style->scrollh.inc_button;
    style->scrollv.inc_button = style->scrollh.inc_button;
    style->scrollv.dec_button = style->scrollh.inc_button;

    /* edit */
    style_edit* edit = &style->edit;
    zero_struct(*edit);
    edit->normal = style_item_color(table[static_cast<std::size_t>(style_colors::COLOR_EDIT)]);
    edit->hover = style_item_color(table[static_cast<std::size_t>(style_colors::COLOR_EDIT)]);
    edit->active = style_item_color(table[static_cast<std::size_t>(style_colors::COLOR_EDIT)]);
    edit->cursor_normal = table[static_cast<std::size_t>(style_colors::COLOR_TEXT)];
    edit->cursor_hover = table[static_cast<std::size_t>(style_colors::COLOR_TEXT)];
    edit->cursor_text_normal = table[static_cast<std::size_t>(style_colors::COLOR_EDIT)];
    edit->cursor_text_hover = table[static_cast<std::size_t>(style_colors::COLOR_EDIT)];
    edit->border_color = table[static_cast<std::size_t>(style_colors::COLOR_BORDER)];
    edit->text_normal = table[static_cast<std::size_t>(style_colors::COLOR_TEXT)];
    edit->text_hover = table[static_cast<std::size_t>(style_colors::COLOR_TEXT)];
    edit->text_active = table[static_cast<std::size_t>(style_colors::COLOR_TEXT)];
    edit->selected_normal = table[static_cast<std::size_t>(style_colors::COLOR_TEXT)];
    edit->selected_hover = table[static_cast<std::size_t>(style_colors::COLOR_TEXT)];
    edit->selected_text_normal = table[static_cast<std::size_t>(style_colors::COLOR_EDIT)];
    edit->selected_text_hover = table[static_cast<std::size_t>(style_colors::COLOR_EDIT)];
    edit->scrollbar_size = vec2_from_floats(10, 10);
    edit->scrollbar = style->scrollv;
    edit->padding = vec2_from_floats(4, 4);
    edit->row_padding = 2;
    edit->cursor_size = 4;
    edit->border = 1;
    edit->rounding = 0;
    edit->color_factor = 1.0f;
    edit->disabled_factor = NK_WIDGET_DISABLED_FACTOR;

    /* property */
    style_property* property = &style->property;
    zero_struct(*property);
    property->normal = style_item_color(table[static_cast<std::size_t>(style_colors::COLOR_PROPERTY)]);
    property->hover = style_item_color(table[static_cast<std::size_t>(style_colors::COLOR_PROPERTY)]);
    property->active = style_item_color(table[static_cast<std::size_t>(style_colors::COLOR_PROPERTY)]);
    property->border_color = table[static_cast<std::size_t>(style_colors::COLOR_BORDER)];
    property->label_normal = table[static_cast<std::size_t>(style_colors::COLOR_TEXT)];
    property->label_hover = table[static_cast<std::size_t>(style_colors::COLOR_TEXT)];
    property->label_active = table[static_cast<std::size_t>(style_colors::COLOR_TEXT)];
    property->sym_left = symbol_type::SYMBOL_TRIANGLE_LEFT;
    property->sym_right = symbol_type::SYMBOL_TRIANGLE_RIGHT;
    property->userdata = handle_ptr(0);
    property->padding = vec2_from_floats(4, 4);
    property->border = 1;
    property->rounding = 10;
    property->draw_begin = 0;
    property->draw_end = 0;
    property->color_factor = 1.0f;
    property->disabled_factor = NK_WIDGET_DISABLED_FACTOR;

    /* property buttons */
    button = &style->property.dec_button;
    zero_struct(*button);
    button->normal = style_item_color(table[static_cast<std::size_t>(style_colors::COLOR_PROPERTY)]);
    button->hover = style_item_color(table[static_cast<std::size_t>(style_colors::COLOR_PROPERTY)]);
    button->active = style_item_color(table[static_cast<std::size_t>(style_colors::COLOR_PROPERTY)]);
    button->border_color = rgba(0, 0, 0, 0);
    button->text_background = table[static_cast<std::size_t>(style_colors::COLOR_PROPERTY)];
    button->text_normal = table[static_cast<std::size_t>(style_colors::COLOR_TEXT)];
    button->text_hover = table[static_cast<std::size_t>(style_colors::COLOR_TEXT)];
    button->text_active = table[static_cast<std::size_t>(style_colors::COLOR_TEXT)];
    button->padding = vec2_from_floats(0.0f, 0.0f);
    button->touch_padding = vec2_from_floats(0.0f, 0.0f);
    button->userdata = handle_ptr(0);
    button->text_alignment = NK_TEXT_CENTERED;
    button->border = 0.0f;
    button->rounding = 0.0f;
    button->color_factor_text = 1.0f;
    button->color_factor_background = 1.0f;
    button->disabled_factor = NK_WIDGET_DISABLED_FACTOR;
    button->draw_begin = 0;
    button->draw_end = 0;
    style->property.inc_button = style->property.dec_button;

    /* property edit */
    edit = &style->property.edit;
    zero_struct(*edit);
    edit->normal = style_item_color(table[static_cast<std::size_t>(style_colors::COLOR_PROPERTY)]);
    edit->hover = style_item_color(table[static_cast<std::size_t>(style_colors::COLOR_PROPERTY)]);
    edit->active = style_item_color(table[static_cast<std::size_t>(style_colors::COLOR_PROPERTY)]);
    edit->border_color = rgba(0, 0, 0, 0);
    edit->cursor_normal = table[static_cast<std::size_t>(style_colors::COLOR_TEXT)];
    edit->cursor_hover = table[static_cast<std::size_t>(style_colors::COLOR_TEXT)];
    edit->cursor_text_normal = table[static_cast<std::size_t>(style_colors::COLOR_EDIT)];
    edit->cursor_text_hover = table[static_cast<std::size_t>(style_colors::COLOR_EDIT)];
    edit->text_normal = table[static_cast<std::size_t>(style_colors::COLOR_TEXT)];
    edit->text_hover = table[static_cast<std::size_t>(style_colors::COLOR_TEXT)];
    edit->text_active = table[static_cast<std::size_t>(style_colors::COLOR_TEXT)];
    edit->selected_normal = table[static_cast<std::size_t>(style_colors::COLOR_TEXT)];
    edit->selected_hover = table[static_cast<std::size_t>(style_colors::COLOR_TEXT)];
    edit->selected_text_normal = table[static_cast<std::size_t>(style_colors::COLOR_EDIT)];
    edit->selected_text_hover = table[static_cast<std::size_t>(style_colors::COLOR_EDIT)];
    edit->padding = vec2_from_floats(0, 0);
    edit->cursor_size = 8;
    edit->border = 0;
    edit->rounding = 0;
    edit->color_factor = 1.0f;
    edit->disabled_factor = NK_WIDGET_DISABLED_FACTOR;

    /* chart */
    style_chart* chart = &style->chart;
    zero_struct(*chart);
    chart->background = style_item_color(table[static_cast<std::size_t>(style_colors::COLOR_CHART)]);
    chart->border_color = table[static_cast<std::size_t>(style_colors::COLOR_BORDER)];
    chart->selected_color = table[static_cast<std::size_t>(style_colors::COLOR_CHART_COLOR_HIGHLIGHT)];
    chart->color = table[static_cast<std::size_t>(style_colors::COLOR_CHART_COLOR)];
    chart->padding = vec2_from_floats(4, 4);
    chart->border = 0;
    chart->rounding = 0;
    chart->color_factor = 1.0f;
    chart->disabled_factor = NK_WIDGET_DISABLED_FACTOR;
    chart->show_markers = true;

    /* combo */
    style_combo* combo = &style->combo;
    combo->normal = style_item_color(table[static_cast<std::size_t>(style_colors::COLOR_COMBO)]);
    combo->hover = style_item_color(table[static_cast<std::size_t>(style_colors::COLOR_COMBO)]);
    combo->active = style_item_color(table[static_cast<std::size_t>(style_colors::COLOR_COMBO)]);
    combo->border_color = table[static_cast<std::size_t>(style_colors::COLOR_BORDER)];
    combo->label_normal = table[static_cast<std::size_t>(style_colors::COLOR_TEXT)];
    combo->label_hover = table[static_cast<std::size_t>(style_colors::COLOR_TEXT)];
    combo->label_active = table[static_cast<std::size_t>(style_colors::COLOR_TEXT)];
    combo->sym_normal = symbol_type::SYMBOL_TRIANGLE_DOWN;
    combo->sym_hover = symbol_type::SYMBOL_TRIANGLE_DOWN;
    combo->sym_active = symbol_type::SYMBOL_TRIANGLE_DOWN;
    combo->content_padding = vec2_from_floats(4, 4);
    combo->button_padding = vec2_from_floats(0, 4);
    combo->spacing = vec2_from_floats(4, 0);
    combo->border = 1;
    combo->rounding = 0;
    combo->color_factor = 1.0f;
    combo->disabled_factor = NK_WIDGET_DISABLED_FACTOR;

    /* combo button */
    button = &style->combo.button;
    zero_struct(*button);
    button->normal = style_item_color(table[static_cast<std::size_t>(style_colors::COLOR_COMBO)]);
    button->hover = style_item_color(table[static_cast<std::size_t>(style_colors::COLOR_COMBO)]);
    button->active = style_item_color(table[static_cast<std::size_t>(style_colors::COLOR_COMBO)]);
    button->border_color = rgba(0, 0, 0, 0);
    button->text_background = table[static_cast<std::size_t>(style_colors::COLOR_COMBO)];
    button->text_normal = table[static_cast<std::size_t>(style_colors::COLOR_TEXT)];
    button->text_hover = table[static_cast<std::size_t>(style_colors::COLOR_TEXT)];
    button->text_active = table[static_cast<std::size_t>(style_colors::COLOR_TEXT)];
    button->padding = vec2_from_floats(2.0f, 2.0f);
    button->touch_padding = vec2_from_floats(0.0f, 0.0f);
    button->userdata = handle_ptr(0);
    button->text_alignment = NK_TEXT_CENTERED;
    button->border = 0.0f;
    button->rounding = 0.0f;
    button->color_factor_text = 1.0f;
    button->color_factor_background = 1.0f;
    button->disabled_factor = NK_WIDGET_DISABLED_FACTOR;
    button->draw_begin = 0;
    button->draw_end = 0;

    /* tab */
    style_tab* tab = &style->tab;
    tab->background = style_item_color(table[static_cast<std::size_t>(style_colors::COLOR_TAB_HEADER)]);
    tab->border_color = table[static_cast<std::size_t>(style_colors::COLOR_BORDER)];
    tab->text = table[static_cast<std::size_t>(style_colors::COLOR_TEXT)];
    tab->sym_minimize = symbol_type::SYMBOL_TRIANGLE_RIGHT;
    tab->sym_maximize = symbol_type::SYMBOL_TRIANGLE_DOWN;
    tab->padding = vec2_from_floats(4, 4);
    tab->spacing = vec2_from_floats(4, 4);
    tab->indent = 10.0f;
    tab->border = 1;
    tab->rounding = 0;
    tab->color_factor = 1.0f;
    tab->disabled_factor = NK_WIDGET_DISABLED_FACTOR;

    /* tab button */
    button = &style->tab.tab_minimize_button;
    zero_struct(*button);
    button->normal = style_item_color(table[static_cast<std::size_t>(style_colors::COLOR_TAB_HEADER)]);
    button->hover = style_item_color(table[static_cast<std::size_t>(style_colors::COLOR_TAB_HEADER)]);
    button->active = style_item_color(table[static_cast<std::size_t>(style_colors::COLOR_TAB_HEADER)]);
    button->border_color = rgba(0, 0, 0, 0);
    button->text_background = table[static_cast<std::size_t>(style_colors::COLOR_TAB_HEADER)];
    button->text_normal = table[static_cast<std::size_t>(style_colors::COLOR_TEXT)];
    button->text_hover = table[static_cast<std::size_t>(style_colors::COLOR_TEXT)];
    button->text_active = table[static_cast<std::size_t>(style_colors::COLOR_TEXT)];
    button->padding = vec2_from_floats(2.0f, 2.0f);
    button->touch_padding = vec2_from_floats(0.0f, 0.0f);
    button->userdata = handle_ptr(0);
    button->text_alignment = NK_TEXT_CENTERED;
    button->border = 0.0f;
    button->rounding = 0.0f;
    button->color_factor_text = 1.0f;
    button->color_factor_background = 1.0f;
    button->disabled_factor = NK_WIDGET_DISABLED_FACTOR;
    button->draw_begin = 0;
    button->draw_end = 0;
    style->tab.tab_maximize_button = *button;

    /* node button */
    button = &style->tab.node_minimize_button;
    zero_struct(*button);
    button->normal = style_item_color(table[static_cast<std::size_t>(style_colors::COLOR_WINDOW)]);
    button->hover = style_item_color(table[static_cast<std::size_t>(style_colors::COLOR_WINDOW)]);
    button->active = style_item_color(table[static_cast<std::size_t>(style_colors::COLOR_WINDOW)]);
    button->border_color = rgba(0, 0, 0, 0);
    button->text_background = table[static_cast<std::size_t>(style_colors::COLOR_TAB_HEADER)];
    button->text_normal = table[static_cast<std::size_t>(style_colors::COLOR_TEXT)];
    button->text_hover = table[static_cast<std::size_t>(style_colors::COLOR_TEXT)];
    button->text_active = table[static_cast<std::size_t>(style_colors::COLOR_TEXT)];
    button->padding = vec2_from_floats(2.0f, 2.0f);
    button->touch_padding = vec2_from_floats(0.0f, 0.0f);
    button->userdata = handle_ptr(0);
    button->text_alignment = NK_TEXT_CENTERED;
    button->border = 0.0f;
    button->rounding = 0.0f;
    button->color_factor_text = 1.0f;
    button->color_factor_background = 1.0f;
    button->disabled_factor = NK_WIDGET_DISABLED_FACTOR;
    button->draw_begin = 0;
    button->draw_end = 0;
    style->tab.node_maximize_button = *button;

    /* window header */
    style_window* win = &style->window;
    win->header.align = style_header_align::HEADER_RIGHT;
    win->header.close_symbol = symbol_type::SYMBOL_X;
    win->header.minimize_symbol = symbol_type::SYMBOL_MINUS;
    win->header.maximize_symbol = symbol_type::SYMBOL_PLUS;
    win->header.normal = style_item_color(table[static_cast<std::size_t>(style_colors::COLOR_HEADER)]);
    win->header.hover = style_item_color(table[static_cast<std::size_t>(style_colors::COLOR_HEADER)]);
    win->header.active = style_item_color(table[static_cast<std::size_t>(style_colors::COLOR_HEADER)]);
    win->header.label_normal = table[static_cast<std::size_t>(style_colors::COLOR_TEXT)];
    win->header.label_hover = table[static_cast<std::size_t>(style_colors::COLOR_TEXT)];
    win->header.label_active = table[static_cast<std::size_t>(style_colors::COLOR_TEXT)];
    win->header.label_padding = vec2_from_floats(4, 4);
    win->header.padding = vec2_from_floats(4, 4);
    win->header.spacing = vec2_from_floats(0, 0);

    /* window header close button */
    button = &style->window.header.close_button;
    zero_struct(*button);
    button->normal = style_item_color(table[static_cast<std::size_t>(style_colors::COLOR_HEADER)]);
    button->hover = style_item_color(table[static_cast<std::size_t>(style_colors::COLOR_HEADER)]);
    button->active = style_item_color(table[static_cast<std::size_t>(style_colors::COLOR_HEADER)]);
    button->border_color = rgba(0, 0, 0, 0);
    button->text_background = table[static_cast<std::size_t>(style_colors::COLOR_HEADER)];
    button->text_normal = table[static_cast<std::size_t>(style_colors::COLOR_TEXT)];
    button->text_hover = table[static_cast<std::size_t>(style_colors::COLOR_TEXT)];
    button->text_active = table[static_cast<std::size_t>(style_colors::COLOR_TEXT)];
    button->padding = vec2_from_floats(0.0f, 0.0f);
    button->touch_padding = vec2_from_floats(0.0f, 0.0f);
    button->userdata = handle_ptr(0);
    button->text_alignment = NK_TEXT_CENTERED;
    button->border = 0.0f;
    button->rounding = 0.0f;
    button->color_factor_text = 1.0f;
    button->color_factor_background = 1.0f;
    button->disabled_factor = NK_WIDGET_DISABLED_FACTOR;
    button->draw_begin = 0;
    button->draw_end = 0;

    /* window header minimize button */
    button = &style->window.header.minimize_button;
    zero_struct(*button);
    button->normal = style_item_color(table[static_cast<std::size_t>(style_colors::COLOR_HEADER)]);
    button->hover = style_item_color(table[static_cast<std::size_t>(style_colors::COLOR_HEADER)]);
    button->active = style_item_color(table[static_cast<std::size_t>(style_colors::COLOR_HEADER)]);
    button->border_color = rgba(0, 0, 0, 0);
    button->text_background = table[static_cast<std::size_t>(style_colors::COLOR_HEADER)];
    button->text_normal = table[static_cast<std::size_t>(style_colors::COLOR_TEXT)];
    button->text_hover = table[static_cast<std::size_t>(style_colors::COLOR_TEXT)];
    button->text_active = table[static_cast<std::size_t>(style_colors::COLOR_TEXT)];
    button->padding = vec2_from_floats(0.0f, 0.0f);
    button->touch_padding = vec2_from_floats(0.0f, 0.0f);
    button->userdata = handle_ptr(0);
    button->text_alignment = NK_TEXT_CENTERED;
    button->border = 0.0f;
    button->rounding = 0.0f;
    button->color_factor_text = 1.0f;
    button->color_factor_background = 1.0f;
    button->disabled_factor = NK_WIDGET_DISABLED_FACTOR;
    button->draw_begin = 0;
    button->draw_end = 0;

    /* window */
    win->background = table[static_cast<std::size_t>(style_colors::COLOR_WINDOW)];
    win->fixed_background = style_item_color(table[static_cast<std::size_t>(style_colors::COLOR_WINDOW)]);
    win->border_color = table[static_cast<std::size_t>(style_colors::COLOR_BORDER)];
    win->popup_border_color = table[static_cast<std::size_t>(style_colors::COLOR_BORDER)];
    win->combo_border_color = table[static_cast<std::size_t>(style_colors::COLOR_BORDER)];
    win->contextual_border_color = table[static_cast<std::size_t>(style_colors::COLOR_BORDER)];
    win->menu_border_color = table[static_cast<std::size_t>(style_colors::COLOR_BORDER)];
    win->group_border_color = table[static_cast<std::size_t>(style_colors::COLOR_BORDER)];
    win->tooltip_border_color = table[static_cast<std::size_t>(style_colors::COLOR_BORDER)];
    win->scaler = style_item_color(table[static_cast<std::size_t>(style_colors::COLOR_TEXT)]);

    win->rounding = 0.0f;
    win->spacing = vec2_from_floats(4, 4);
    win->scrollbar_size = vec2_from_floats(10, 10);
    win->min_size = vec2_from_floats(64, 64);

    win->combo_border = 1.0f;
    win->contextual_border = 1.0f;
    win->menu_border = 1.0f;
    win->group_border = 1.0f;
    win->tooltip_border = 1.0f;
    win->popup_border = 1.0f;
    win->border = 2.0f;
    win->min_row_height_padding = 8;

    win->padding = vec2_from_floats(4, 4);
    win->group_padding = vec2_from_floats(4, 4);
    win->popup_padding = vec2_from_floats(4, 4);
    win->combo_padding = vec2_from_floats(4, 4);
    win->contextual_padding = vec2_from_floats(4, 4);
    win->menu_padding = vec2_from_floats(4, 4);
    win->tooltip_padding = vec2_from_floats(4, 4);
  }

  NK_API void
  style_set_font(context* ctx, const user_font* font) {
    NK_ASSERT(ctx);

    if (!ctx)
      return;
    style* style = &ctx->style;
    style->font = font;
    ctx->stacks.fonts.head = 0;
    if (ctx->current)
      layout_reset_min_row_height(ctx);
  }

  NK_API bool
  style_push_font(context* ctx, const user_font* font) {

    NK_ASSERT(ctx);
    if (!ctx)
      return 0;

    config_stack_user_font* font_stack = &ctx->stacks.fonts;
    NK_ASSERT(font_stack->head < (int) NK_LEN(font_stack->elements));
    if (font_stack->head >= (int) NK_LEN(font_stack->elements))
      return 0;

    config_stack_user_font_element* element = &font_stack->elements[static_cast<std::size_t>(font_stack->head++)];
    element->address = &ctx->style.font;
    element->old_value = ctx->style.font;
    ctx->style.font = font;
    return 1;
  }

  NK_API bool
  style_pop_font(context* ctx) {

    NK_ASSERT(ctx);
    if (!ctx)
      return 0;

    config_stack_user_font* font_stack = &ctx->stacks.fonts;
    NK_ASSERT(font_stack->head > 0);
    if (font_stack->head < 1)
      return 0;

    const config_stack_user_font_element* element = &font_stack->elements[static_cast<std::size_t>(--font_stack->head)];
    *element->address = element->old_value;
    return 1;
  }

#define NK_STYLE_PUSH_IMPLEMENATION(prefix, type, stack)                                    \
  style_push_##type(struct context* ctx, prefix##_##type* address, prefix##_##type value) { \
    struct config_stack_##type* type_stack;                                                 \
    struct config_stack_##type##_element* element;                                          \
    NK_ASSERT(ctx);                                                                         \
    if (!ctx)                                                                               \
      return 0;                                                                             \
    type_stack = &ctx->stacks.stack;                                                        \
    NK_ASSERT(type_stack->head < (int) NK_LEN(type_stack->elements));                       \
    if (type_stack->head >= (int) NK_LEN(type_stack->elements))                             \
      return 0;                                                                             \
    element = &type_stack->elements[type_stack->head++];                                    \
    element->address = address;                                                             \
    element->old_value = *address;                                                          \
    *address = value;                                                                       \
    return 1;                                                                               \
  }

#define NK_STYLE_POP_IMPLEMENATION(type, stack)                                    \
  style_pop_##type(struct context* ctx) {                                          \
    struct config_stack_##type* type_stack;                                        \
    struct config_stack_##type##_element* element;                                 \
    NK_ASSERT(ctx);                                                                \
    if (!ctx)                                                                      \
      return 0;                                                                    \
    type_stack = &ctx->stacks.stack;                                               \
    NK_ASSERT(type_stack->head > 0);                                               \
    if (type_stack->head < 1)                                                      \
      return 0;                                                                    \
    element = &type_stack->elements[static_cast<std::size_t>(--type_stack->head)]; \
    *element->address = element->old_value;                                        \
    return 1;                                                                      \
  }

  NK_API bool style_push_style_item(context* ctx, style_item* address, style_item value) {
    NK_ASSERT(ctx);
    if (!ctx)
      return 0;
    config_stack_style_item* type_stack = &ctx->stacks.style_items;
    NK_ASSERT(type_stack->head < (int) NK_LEN(type_stack->elements));
    if (type_stack->head >= (int) NK_LEN(type_stack->elements))
      return 0;
    config_stack_style_item_element* element = &type_stack->elements[static_cast<std::size_t>(type_stack->head++)];
    element->address = address;
    element->old_value = *address;
    *address = value;
    return 1;
  }

  NK_API bool style_push_float(context* ctx, float* address, float value) {
    NK_ASSERT(ctx);
    if (!ctx)
      return 0;
    config_stack_float* type_stack = &ctx->stacks.floats;
    NK_ASSERT(type_stack->head < (int) NK_LEN(type_stack->elements));
    if (type_stack->head >= (int) NK_LEN(type_stack->elements))
      return 0;
    config_stack_float_element* element = &type_stack->elements[static_cast<std::size_t>(type_stack->head++)];
    element->address = address;
    element->old_value = *address;
    *address = value;
    return 1;
  }

  NK_API bool style_push_vec2(context* ctx, vec2f* address, const vec2f value) {
    NK_ASSERT(ctx);
    if (!ctx)
      return 0;
    config_stack_vec2* type_stack = &ctx->stacks.vectors;
    NK_ASSERT(type_stack->head < (int) NK_LEN(type_stack->elements));
    if (type_stack->head >= (int) NK_LEN(type_stack->elements))
      return 0;
    config_stack_vec2_element* element = &type_stack->elements[static_cast<std::size_t>(type_stack->head++)];
    element->address = address;
    element->old_value = *address;
    *address = value;
    return 1;
  }

  NK_API bool style_push_flags(context* ctx, flag* address, const flag value) {
    NK_ASSERT(ctx);
    if (!ctx)
      return 0;
    config_stack_flags* type_stack = &ctx->stacks.flags;
    NK_ASSERT(type_stack->head < (int) NK_LEN(type_stack->elements));
    if (type_stack->head >= (int) NK_LEN(type_stack->elements))
      return 0;
    config_stack_flags_element* element = &type_stack->elements[static_cast<std::size_t>(type_stack->head++)];
    element->address = address;
    element->old_value = *address;
    *address = value;
    return 1;
  }

  NK_API bool style_push_color(context* ctx, color* address, const color value) {
    NK_ASSERT(ctx);
    if (!ctx)
      return 0;
    config_stack_color* type_stack = &ctx->stacks.colors;
    NK_ASSERT(type_stack->head < (int) NK_LEN(type_stack->elements));
    if (type_stack->head >= (int) NK_LEN(type_stack->elements))
      return 0;
    config_stack_color_element* element = &type_stack->elements[static_cast<std::size_t>(type_stack->head++)];
    element->address = address;
    element->old_value = *address;
    *address = value;
    return 1;
  }

  NK_API void
  style_load_cursor(context* ctx, style_cursor cur,
                    const cursor* c) {
    NK_ASSERT(ctx);
    if (!ctx)
      return;
    style* style = &ctx->style;
    style->cursors[static_cast<std::size_t>(cur)] = c;
  }

  NK_API void
  style_load_all_cursors(context* ctx, const cursor* cursors) {
    int i = 0;
    NK_ASSERT(ctx);
    if (!ctx)
      return;
    style* style = &ctx->style;
    for (i = 0; i < std::to_underlying(style_cursor::CURSOR_COUNT); ++i)
      style->cursors[i] = &cursors[i];
    style->cursor_visible = true;
  }

  NK_API bool
  style_set_cursor(context* ctx, style_cursor c) {
    NK_ASSERT(ctx);
    if (!ctx)
      return 0;
    style* style = &ctx->style;
    if (style->cursors[static_cast<std::size_t>(c)]) {
      style->cursor_active = style->cursors[static_cast<std::size_t>(c)];
      return 1;
    }
    return 0;
  }

  NK_API void
  style_show_cursor(context* ctx) {
    ctx->style.cursor_visible = true;
  }

  NK_API void
  style_hide_cursor(context* ctx) {
    ctx->style.cursor_visible = false;
  }

  NK_API bool style_pop_float(context* ctx) {
    NK_ASSERT(ctx);
    if (!ctx)
      return 0;
    config_stack_float* type_stack = &ctx->stacks.floats;
    NK_ASSERT(type_stack->head > 0);
    if (type_stack->head < 1)
      return 0;
    const config_stack_float_element* element = &type_stack->elements[static_cast<std::size_t>(--type_stack->head)];
    *element->address = element->old_value;
    return 1;
  }

  NK_API bool style_pop_vec2(context* ctx) {
    NK_ASSERT(ctx);
    if (!ctx)
      return 0;
    config_stack_vec2* type_stack = &ctx->stacks.vectors;
    NK_ASSERT(type_stack->head > 0);
    if (type_stack->head < 1)
      return 0;
    const config_stack_vec2_element* element = &type_stack->elements[static_cast<std::size_t>(--type_stack->head)];
    *element->address = element->old_value;
    return 1;
  }

  NK_API bool style_pop_style_item(context* ctx) {
    NK_ASSERT(ctx);
    if (!ctx)
      return 0;
    config_stack_style_item* type_stack = &ctx->stacks.style_items;
    NK_ASSERT(type_stack->head > 0);
    if (type_stack->head < 1)
      return 0;
    const config_stack_style_item_element* element = &type_stack->elements[static_cast<std::size_t>(--type_stack->head)];
    *element->address = element->old_value;
    return 1;
  }

  NK_API bool style_pop_flags(context* ctx) {
    NK_ASSERT(ctx);
    if (!ctx)
      return 0;
    config_stack_flags* type_stack = &ctx->stacks.flags;
    NK_ASSERT(type_stack->head > 0);
    if (type_stack->head < 1)
      return 0;
    const config_stack_flags_element* element = &type_stack->elements[static_cast<std::size_t>(--type_stack->head)];
    *element->address = element->old_value;
    return 1;
  }

  NK_API bool style_pop_color(context* ctx) {
    NK_ASSERT(ctx);
    if (!ctx)
      return 0;
    config_stack_color* type_stack = &ctx->stacks.colors;
    NK_ASSERT(type_stack->head > 0);
    if (type_stack->head < 1)
      return 0;
    const config_stack_color_element* element = &type_stack->elements[static_cast<std::size_t>(--type_stack->head)];
    *element->address = element->old_value;
    return 1;
  }
} // namespace nk
