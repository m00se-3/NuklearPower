#ifndef NK_POWER_WIDGETS_HPP
#define NK_POWER_WIDGETS_HPP

#include <internal/nuklear_internal.hpp>

namespace nk {
  bool list_view_begin(context*, list_view* out, const char* id, flag, int row_height, int row_count);
  void list_view_end(list_view*);
  widget_layout_states widget(rectf*, const context*);
  widget_layout_states widget_fitting(rectf*, const context*, vec2f);
  rectf widget_bounds(const context*);
  vec2f widget_position(const context*);
  vec2f widget_size(const context*);
  float widget_width(const context*);
  float widget_height(const context*);
  bool widget_is_hovered(const context*);
  bool widget_is_mouse_clicked(const context*, buttons);
  bool widget_has_mouse_click_down(const context*, buttons, bool down);
  void spacing(context*, int cols);
  void widget_disable_begin(context* ctx);
  void widget_disable_end(context* ctx);
  void text_string(context*, const char*, int, flag);
  void text_colored(context*, const char*, int, flag, color);
  void text_wrap(context*, const char*, int);
  void text_wrap_colored(context*, const char*, int, color);
  void label(context*, const char*, flag align);
  void label_colored(context*, const char*, flag align, color);
  void label_wrap(context*, const char*);
  void label_colored_wrap(context*, const char*, color);
  void get_image(context*, image);
  void image_color(context*, image, color);
#ifdef NK_INCLUDE_STANDARD_VARARGS
  void labelf(struct context*, flags, NK_PRINTF_FORMAT_STRING const char*, ...) NK_PRINTF_VARARG_FUNC(3);
  void labelf_colored(struct context*, flags, struct color, NK_PRINTF_FORMAT_STRING const char*, ...) NK_PRINTF_VARARG_FUNC(4);
  void labelf_wrap(struct context*, NK_PRINTF_FORMAT_STRING const char*, ...) NK_PRINTF_VARARG_FUNC(2);
  void labelf_colored_wrap(struct context*, struct color, NK_PRINTF_FORMAT_STRING const char*, ...) NK_PRINTF_VARARG_FUNC(3);
  void labelfv(struct context*, flags, NK_PRINTF_FORMAT_STRING const char*, va_list) NK_PRINTF_VALIST_FUNC(3);
  void labelfv_colored(struct context*, flags, struct color, NK_PRINTF_FORMAT_STRING const char*, va_list) NK_PRINTF_VALIST_FUNC(4);
  void labelfv_wrap(struct context*, NK_PRINTF_FORMAT_STRING const char*, va_list) NK_PRINTF_VALIST_FUNC(2);
  void labelfv_colored_wrap(struct context*, struct color, NK_PRINTF_FORMAT_STRING const char*, va_list) NK_PRINTF_VALIST_FUNC(3);
  void value_bool(struct context*, const char* prefix, int);
  void value_int(struct context*, const char* prefix, int);
  void value_uint(struct context*, const char* prefix, unsigned int);
  void value_float(struct context*, const char* prefix, float);
  void value_color_byte(struct context*, const char* prefix, struct color);
  void value_color_float(struct context*, const char* prefix, struct color);
  void value_color_hex(struct context*, const char* prefix, struct color);
#endif
  /* =============================================================================
   *
   *                                  BUTTON
   *
   * ============================================================================= */
  bool button_text(context*, const char* title, int len);
  bool button_label(context*, const char* title);
  bool button_color(context*, color);
  bool button_symbol(context*, symbol_type);
  bool button_image(context*, struct image img);
  bool button_symbol_label(context*, symbol_type, const char*, flag text_alignment);
  bool button_symbol_text(context*, symbol_type, const char*, int, flag alignment);
  bool button_image_label(context*, struct image img, const char*, flag text_alignment);
  bool button_image_text(context*, struct image img, const char*, int, flag alignment);
  bool button_text_styled(context*, const style_button*, const char* title, int len);
  bool button_label_styled(context*, const style_button*, const char* title);
  bool button_symbol_styled(context*, const style_button*, symbol_type);
  bool button_image_styled(context*, const style_button*, struct image img);
  bool button_symbol_text_styled(context*, const style_button*, symbol_type, const char*, int, flag alignment);
  bool button_symbol_label_styled(context* ctx, const style_button* style, symbol_type symbol, const char* title, flag align);
  bool button_image_label_styled(context*, const style_button*, struct image img, const char*, flag text_alignment);
  bool button_image_text_styled(context*, const style_button*, struct image img, const char*, int, flag alignment);
  void button_set_behavior(context*, nk::btn_behavior);
  bool button_push_behavior(context*, nk::btn_behavior);
  bool button_pop_behavior(context*);
  /* =============================================================================
   *
   *                                  CHECKBOX
   *
   * ============================================================================= */
  bool check_label(context*, const char*, bool active);
  bool check_text(context*, const char*, int, bool active);
  bool check_text_align(context*, const char*, int, bool active, flag widget_alignment, flag text_alignment);
  unsigned check_flags_label(context*, const char*, unsigned int flags, unsigned int value);
  unsigned check_flags_text(context*, const char*, int, unsigned int flags, unsigned int value);
  bool checkbox_label(context*, const char*, bool* active);
  bool checkbox_label_align(context* ctx, const char* label, bool* active, flag widget_alignment, flag text_alignment);
  bool checkbox_text(context*, const char*, int, bool* active);
  bool checkbox_text_align(context* ctx, const char* text, int len, bool* active, flag widget_alignment, flag text_alignment);
  bool checkbox_flags_label(context*, const char*, unsigned int* flags, unsigned int value);
  bool checkbox_flags_text(context*, const char*, int, unsigned int* flags, unsigned int value);
  /* =============================================================================
   *
   *                                  RADIO BUTTON
   *
   * ============================================================================= */
  bool radio_label(context*, const char*, bool* active);
  bool radio_label_align(context* ctx, const char* label, bool* active, flag widget_alignment, flag text_alignment);
  bool radio_text(context*, const char*, int, bool* active);
  bool radio_text_align(context* ctx, const char* text, int len, bool* active, flag widget_alignment, flag text_alignment);
  bool option_label(context*, const char*, bool active);
  bool option_label_align(context* ctx, const char* label, bool active, flag widget_alignment, flag text_alignment);
  bool option_text(context*, const char*, int, bool active);
  bool option_text_align(context* ctx, const char* text, int len, bool is_active, flag widget_alignment, flag text_alignment);
  /* =============================================================================
   *
   *                                  SELECTABLE
   *
   * ============================================================================= */
  bool selectable_label(context*, const char*, flag align, bool* value);
  bool selectable_text(context*, const char*, int, flag align, bool* value);
  bool selectable_image_label(context*, struct image, const char*, flag align, bool* value);
  bool selectable_image_text(context*, struct image, const char*, int, flag align, bool* value);
  bool selectable_symbol_label(context*, symbol_type, const char*, flag align, bool* value);
  bool selectable_symbol_text(context*, symbol_type, const char*, int, flag align, bool* value);

  bool select_label(context*, const char*, flag align, bool value);
  bool select_text(context*, const char*, int, flag align, bool value);
  bool select_image_label(context*, struct image, const char*, flag align, bool value);
  bool select_image_text(context*, struct image, const char*, int, flag align, bool value);
  bool select_symbol_label(context*, symbol_type, const char*, flag align, bool value);
  bool select_symbol_text(context*, symbol_type, const char*, int, flag align, bool value);

  /* =============================================================================
   *
   *                                  SLIDER
   *
   * ============================================================================= */
  float slide_float(context*, float min, float val, float max, float step);
  int slide_int(context*, int min, int val, int max, int step);
  bool slider_float(context*, float min, float* val, float max, float step);
  bool slider_int(context*, int min, int* val, int max, int step);

  /* =============================================================================
   *
   *                                   KNOB
   *
   * ============================================================================= */
  bool knob_float(context*, float min, float* val, float max, float step, nk::heading zero_direction, float dead_zone_degrees);
  bool knob_int(context*, int min, int* val, int max, int step, nk::heading zero_direction, float dead_zone_degrees);

  /* =============================================================================
   *
   *                                  PROGRESSBAR
   *
   * ============================================================================= */
  bool progress(context*, std::size_t* cur, std::size_t max, bool modifyable);
  std::size_t prog(context*, std::size_t cur, std::size_t max, bool modifyable);

  /* =============================================================================
   *
   *                                  COLOR PICKER
   *
   * ============================================================================= */
  colorf color_picker(context*, colorf, nk::color_format);
  bool color_pick(context*, colorf*, nk::color_format);

  flag edit_string(context*, flag, char* buffer, int* len, int max, plugin_filter);
  flag edit_string_zero_terminated(context*, flag, char* buffer, int max, plugin_filter);
  flag edit_buffer(context*, flag, text_edit*, plugin_filter);
  void edit_focus(context*, flag flags);
  void edit_unfocus(context*);
  /* =============================================================================
   *
   *                                  CHART
   *
   * ============================================================================= */
  bool chart_begin(context*, nk::chart_type, int num, float min, float max);
  bool chart_begin_colored(context*, nk::chart_type, color, color active, int num, float min, float max);
  void chart_add_slot(context* ctx, const nk::chart_type, int count, float min_value, float max_value);
  void chart_add_slot_colored(context* ctx, const nk::chart_type, color, color active, int count, float min_value, float max_value);
  flag chart_push(context*, float);
  flag chart_push_slot(context*, float, int);
  void chart_end(context*);
  void plot(context*, nk::chart_type, const float* values, int count, int offset);
  void plot_function(context*, nk::chart_type, void* userdata, float (*value_getter)(void* user, int index), int count, int offset);
  /* =============================================================================
   *
   *                                  POPUP
   *
   * ============================================================================= */
  bool popup_begin(context*, nk::popup_type, const char*, flag, rectf bounds);
  void popup_close(context*);
  void popup_end(context*);
  void popup_get_scroll(const context*, std::uint32_t* offset_x, std::uint32_t* offset_y);
  void popup_set_scroll(context*, std::uint32_t offset_x, std::uint32_t offset_y);
  /* =============================================================================
   *
   *                                  COMBOBOX
   *
   * ============================================================================= */
  int combo(context*, const char* const* items, int count, int selected, int item_height, vec2f size);
  int combo_separator(context*, const char* items_separated_by_separator, int separator, int selected, int count, int item_height, vec2f size);
  int combo_string(context*, const char* items_separated_by_zeros, int selected, int count, int item_height, vec2f size);
  int combo_callback(context*, void (*item_getter)(void*, int, const char**), void* userdata, int selected, int count, int item_height, vec2f size);
  void combobox(context*, const char* const* items, int count, int* selected, int item_height, vec2f size);
  void combobox_string(context*, const char* items_separated_by_zeros, int* selected, int count, int item_height, vec2f size);
  void combobox_separator(context*, const char* items_separated_by_separator, int separator, int* selected, int count, int item_height, vec2f size);
  void combobox_callback(context*, void (*item_getter)(void*, int, const char**), void*, int* selected, int count, int item_height, vec2f size);
  /* =============================================================================
   *
   *                                  ABSTRACT COMBOBOX
   *
   * ============================================================================= */
  bool combo_begin_text(context*, const char* selected, int, vec2f size);
  bool combo_begin_label(context*, const char* selected, vec2f size);
  bool combo_begin_color(context*, color color, vec2f size);
  bool combo_begin_symbol(context*, symbol_type, vec2f size);
  bool combo_begin_symbol_label(context*, const char* selected, symbol_type, vec2f size);
  bool combo_begin_symbol_text(context*, const char* selected, int, symbol_type, vec2f size);
  bool combo_begin_image(context*, struct image img, vec2f size);
  bool combo_begin_image_label(context*, const char* selected, struct image, vec2f size);
  bool combo_begin_image_text(context*, const char* selected, int, struct image, vec2f size);
  bool combo_item_label(context*, const char*, flag alignment);
  bool combo_item_text(context*, const char*, int, flag alignment);
  bool combo_item_image_label(context*, struct image, const char*, flag alignment);
  bool combo_item_image_text(context*, struct image, const char*, int, flag alignment);
  bool combo_item_symbol_label(context*, symbol_type, const char*, flag alignment);
  bool combo_item_symbol_text(context*, symbol_type, const char*, int, flag alignment);
  void combo_close(context*);
  void combo_end(context*);
  /* =============================================================================
   *
   *                                  CONTEXTUAL
   *
   * ============================================================================= */
  bool contextual_begin(context*, flag, vec2f, rectf trigger_bounds);
  bool contextual_item_text(context*, const char*, int, flag align);
  bool contextual_item_label(context*, const char*, flag align);
  bool contextual_item_image_label(context*, struct image, const char*, flag alignment);
  bool contextual_item_image_text(context*, struct image, const char*, int len, flag alignment);
  bool contextual_item_symbol_label(context*, symbol_type, const char*, flag alignment);
  bool contextual_item_symbol_text(context*, symbol_type, const char*, int, flag alignment);
  void contextual_close(context*);
  void contextual_end(context*);
  /* =============================================================================
   *
   *                                  TOOLTIP
   *
   * ============================================================================= */
  void tooltip(context*, const char*);
#ifdef NK_INCLUDE_STANDARD_VARARGS
  void tooltipf(struct context*, NK_PRINTF_FORMAT_STRING const char*, ...) NK_PRINTF_VARARG_FUNC(2);
  void tooltipfv(struct context*, NK_PRINTF_FORMAT_STRING const char*, va_list) NK_PRINTF_VALIST_FUNC(2);
#endif
  bool tooltip_begin(context*, float width);
  void tooltip_end(context*);
  /* =============================================================================
   *
   *                                  MENU
   *
   * ============================================================================= */
  void menubar_begin(context*);
  void menubar_end(context*);
  bool menu_begin_text(context*, const char* title, int title_len, flag align, vec2f size);
  bool menu_begin_label(context*, const char*, flag align, vec2f size);
  bool menu_begin_image(context*, const char*, struct image, vec2f size);
  bool menu_begin_image_text(context*, const char*, int, flag align, struct image, vec2f size);
  bool menu_begin_image_label(context*, const char*, flag align, struct image, vec2f size);
  bool menu_begin_symbol(context*, const char*, symbol_type, vec2f size);
  bool menu_begin_symbol_text(context*, const char*, int, flag align, symbol_type, vec2f size);
  bool menu_begin_symbol_label(context*, const char*, flag align, symbol_type, vec2f size);
  bool menu_item_text(context*, const char*, int, flag align);
  bool menu_item_label(context*, const char*, flag alignment);
  bool menu_item_image_label(context*, struct image, const char*, flag alignment);
  bool menu_item_image_text(context*, struct image, const char*, int len, flag alignment);
  bool menu_item_symbol_text(context*, symbol_type, const char*, int, flag alignment);
  bool menu_item_symbol_label(context*, symbol_type, const char*, flag alignment);
  void menu_close(context*);
  void menu_end(context*);

  /**===============================================================
   *
   *                      TEXT EDITOR
   *
   * ===============================================================*/
  /**
   * \page Text Editor
   * Editing text in this library is handled by either `edit_string` or
   * `edit_buffer`. But like almost everything in this library there are multiple
   * ways of doing it and a balance between control and ease of use with memory
   * as well as functionality controlled by flag.
   *
   * This library generally allows three different levels of memory control:
   * First of is the most basic way of just providing a simple char array with
   * string length. This method is probably the easiest way of handling simple
   * user text input. Main upside is complete control over memory while the biggest
   * downside in comparison with the other two approaches is missing undo/redo.
   *
   * For UIs that require undo/redo the second way was created. It is based on
   * a fixed size text_edit struct, which has an internal undo/redo stack.
   * This is mainly useful if you want something more like a text editor but don't want
   * to have a dynamically growing buffer.
   *
   * The final way is using a dynamically growing text_edit struct, which
   * has both a default version if you don't care where memory comes from and an
   * allocator version if you do. While the text editor is quite powerful for its
   * complexity I would not recommend editing gigabytes of data with it.
   * It is rather designed for uses cases which make sense for a GUI library not for
   * an full blown text editor.
   */

  /** filter function */
  bool filter_default(const text_edit*, rune unicode);
  bool filter_ascii(const text_edit*, rune unicode);
  bool filter_float(const text_edit*, rune unicode);
  bool filter_decimal(const text_edit*, rune unicode);
  bool filter_hex(const text_edit*, rune unicode);
  bool filter_oct(const text_edit*, rune unicode);
  bool filter_binary(const text_edit*, rune unicode);

  /** text editor */
#ifdef NK_INCLUDE_DEFAULT_ALLOCATOR
  void textedit_init_default(struct text_edit*);
#endif
  void textedit_init(text_edit*, const allocator*, std::size_t size);
  void textedit_init_fixed(text_edit*, void* memory, std::size_t size);
  void textedit_free(text_edit*);
  void textedit_text(text_edit*, const char*, int total_len);
  void textedit_delete(text_edit*, int where, int len);
  void textedit_delete_selection(text_edit*);
  void textedit_select_all(text_edit*);
  bool textedit_cut(text_edit*);
  bool textedit_paste(text_edit*, char const*, int len);
  void textedit_undo(text_edit*);
  void textedit_redo(text_edit*);

  /* popup */
  bool nonblock_begin(context* ctx, flag flags, rectf body, rectf header, panel_type::value_type panel_type);

  void widget_text(command_buffer* o, rectf b, const char* string, int len, const text* t, flag a, const user_font* f);
  void widget_text_wrap(command_buffer* o, rectf b, const char* string, int len, const text* t, const user_font* f);

  /* button */
  bool button_behavior(flag* state, rectf r, const input* i, btn_behavior behavior);
  const style_item* draw_button(command_buffer* out, const rectf* bounds, flag state, const style_button* style);
  bool do_button(flag* state, command_buffer* out, rectf r, const style_button* style, const input* in, btn_behavior behavior, rectf* content);
  void draw_button_text(command_buffer* out, const rectf* bounds, const rectf* content, flag state, const style_button* style, const char* txt, int len, flag text_alignment, const user_font* font);
  bool do_button_text(flag* state, command_buffer* out, rectf bounds, const char* string, int len, flag align, btn_behavior behavior, const style_button* style, const input* in, const user_font* font);
  void draw_button_symbol(command_buffer* out, const rectf* bounds, const rectf* content, flag state, const style_button* style, symbol_type type, const user_font* font);
  bool do_button_symbol(flag* state, command_buffer* out, rectf bounds, symbol_type symbol, btn_behavior behavior, const style_button* style, const input* in, const user_font* font);
  void draw_button_image(command_buffer* out, const rectf* bounds, const rectf* content, flag state, const style_button* style, const struct image* img);
  bool do_button_image(flag* state, command_buffer* out, rectf bounds, struct image img, btn_behavior b, const style_button* style, const input* in);
  void draw_button_text_symbol(command_buffer* out, const rectf* bounds, const rectf* label, const rectf* symbol, flag state, const style_button* style, const char* str, int len, symbol_type type, const user_font* font);
  bool do_button_text_symbol(flag* state, command_buffer* out, rectf bounds, symbol_type symbol, const char* str, int len, flag align, btn_behavior behavior, const style_button* style, const user_font* font, const input* in);
  void draw_button_text_image(command_buffer* out, const rectf* bounds, const rectf* label, const rectf* image, flag state, const style_button* style, const char* str, int len, const user_font* font, const struct image* img);
  bool do_button_text_image(flag* state, command_buffer* out, rectf bounds, struct image img, const char* str, int len, flag align, btn_behavior behavior, const style_button* style, const user_font* font, const input* in);

  bool toggle_behavior(const input* in, rectf select, flag* state, bool active);
  void draw_checkbox(command_buffer* out, flag state, const style_toggle* style, bool active, const rectf* label, const rectf* selector, const rectf* cursors, const char* string, int len, const user_font* font, flag text_alignment);
  void draw_option(command_buffer* out, flag state, const style_toggle* style, bool active, const rectf* label, const rectf* selector, const rectf* cursors, const char* string, int len, const user_font* font, flag text_alignment);
  bool do_toggle(flag* state, command_buffer* out, rectf r, bool* active, const char* str, int len, toggle_type type, const style_toggle* style, const input* in, const user_font* font, flag widget_alignment, flag text_alignment);

  /* progress */
  std::size_t progress_behavior(flag* state, input* in, rectf r, rectf cursor, std::size_t max, std::size_t value, bool modifiable);
  void draw_progress(command_buffer* out, flag state, const style_progress* style, const rectf* bounds, const rectf* scursor, std::size_t value, std::size_t max);
  std::size_t do_progress(flag* state, command_buffer* out, rectf bounds, std::size_t value, std::size_t max, bool modifiable, const style_progress* style, input* in);

  /* slider */
  float slider_behavior(flag* state, rectf* logical_cursor, rectf* visual_cursor, input* in, rectf bounds, float slider_min, float slider_max, float slider_value, float slider_step, float slider_steps);
  void draw_slider(command_buffer* out, flag state, const style_slider* style, const rectf* bounds, const rectf* visual_cursor, float min, float value, float max);
  float do_slider(flag* state, command_buffer* out, rectf bounds, float min, float val, float max, float step, const style_slider* style, input* in, const user_font* font);

  /* scrollbar */
  float scrollbar_behavior(flag* state, input* in, int has_scrolling, const rectf* scroll, const rectf* cursor, const rectf* empty0, const rectf* empty1, float scroll_offset, float target, float scroll_step, orientation o);
  void draw_scrollbar(command_buffer* out, flag state, const style_scrollbar* style, const rectf* bounds, const rectf* scroll);
  float do_scrollbarv(flag* state, command_buffer* out, rectf scroll, int has_scrolling, float offset, float target, float step, float button_pixel_inc, const style_scrollbar* style, input* in, const user_font* font);
  float do_scrollbarh(flag* state, command_buffer* out, rectf scroll, int has_scrolling, float offset, float target, float step, float button_pixel_inc, const style_scrollbar* style, input* in, const user_font* font);

  /* selectable */
  void draw_selectable(command_buffer* out, flag state, const style_selectable* style, bool active, const rectf* bounds, const rectf* icon, const struct image* img, symbol_type sym, const char* string, int len, flag align, const user_font* font);
  bool do_selectable(flag* state, command_buffer* out, rectf bounds, const char* str, int len, flag align, bool* value, const style_selectable* style, const input* in, const user_font* font);
  bool do_selectable_image(flag* state, command_buffer* out, rectf bounds, const char* str, int len, flag align, bool* value, const struct image* img, const style_selectable* style, const input* in, const user_font* font);

  /* edit */
  void edit_draw_text(command_buffer* out, const style_edit* style, float pos_x, float pos_y, float x_offset, const char* text, int byte_len, float row_height, const user_font* font, color background, color foreground, bool is_selected);
  flag do_edit(flag* state, command_buffer* out, rectf bounds, flag flags, plugin_filter filter, text_edit* edit, const style_edit* style, input* in, const user_font* font);

  /* color-picker */
  bool color_picker_behavior(flag* state, const rectf* bounds, const rectf* matrix, const rectf* hue_bar, const rectf* alpha_bar, colorf* color, const input* in);
  void draw_color_picker(command_buffer* o, const rectf* matrix, const rectf* hue_bar, const rectf* alpha_bar, colorf col);
  bool do_color_picker(flag* state, command_buffer* out, colorf* col, color_format fmt, rectf bounds, vec2f padding, const input* in, const user_font* font);

}

#endif