#ifndef NK_POWER_WIDGETS_HPP
#define NK_POWER_WIDGETS_HPP

#include <internal/nuklear_internal.hpp>

namespace nk {
  NK_API bool list_view_begin(context*, list_view* out, const char* id, flag, int row_height, int row_count);
  NK_API void list_view_end(list_view*);
  NK_API widget_layout_states widget(rectf*, const context*);
  NK_API widget_layout_states widget_fitting(rectf*, const context*, vec2f);
  NK_API rectf widget_bounds(const context*);
  NK_API vec2f widget_position(const context*);
  NK_API vec2f widget_size(const context*);
  NK_API float widget_width(const context*);
  NK_API float widget_height(const context*);
  NK_API bool widget_is_hovered(const context*);
  NK_API bool widget_is_mouse_clicked(const context*, buttons);
  NK_API bool widget_has_mouse_click_down(const context*, buttons, bool down);
  NK_API void spacing(context*, int cols);
  NK_API void widget_disable_begin(context* ctx);
  NK_API void widget_disable_end(context* ctx);
  NK_API void text_string(context*, const char*, int, flag);
  NK_API void text_colored(context*, const char*, int, flag, color);
  NK_API void text_wrap(context*, const char*, int);
  NK_API void text_wrap_colored(context*, const char*, int, color);
  NK_API void label(context*, const char*, flag align);
  NK_API void label_colored(context*, const char*, flag align, color);
  NK_API void label_wrap(context*, const char*);
  NK_API void label_colored_wrap(context*, const char*, color);
  NK_API void get_image(context*, image);
  NK_API void image_color(context*, image, color);
#ifdef NK_INCLUDE_STANDARD_VARARGS
  NK_API void labelf(struct context*, flags, NK_PRINTF_FORMAT_STRING const char*, ...) NK_PRINTF_VARARG_FUNC(3);
  NK_API void labelf_colored(struct context*, flags, struct color, NK_PRINTF_FORMAT_STRING const char*, ...) NK_PRINTF_VARARG_FUNC(4);
  NK_API void labelf_wrap(struct context*, NK_PRINTF_FORMAT_STRING const char*, ...) NK_PRINTF_VARARG_FUNC(2);
  NK_API void labelf_colored_wrap(struct context*, struct color, NK_PRINTF_FORMAT_STRING const char*, ...) NK_PRINTF_VARARG_FUNC(3);
  NK_API void labelfv(struct context*, flags, NK_PRINTF_FORMAT_STRING const char*, va_list) NK_PRINTF_VALIST_FUNC(3);
  NK_API void labelfv_colored(struct context*, flags, struct color, NK_PRINTF_FORMAT_STRING const char*, va_list) NK_PRINTF_VALIST_FUNC(4);
  NK_API void labelfv_wrap(struct context*, NK_PRINTF_FORMAT_STRING const char*, va_list) NK_PRINTF_VALIST_FUNC(2);
  NK_API void labelfv_colored_wrap(struct context*, struct color, NK_PRINTF_FORMAT_STRING const char*, va_list) NK_PRINTF_VALIST_FUNC(3);
  NK_API void value_bool(struct context*, const char* prefix, int);
  NK_API void value_int(struct context*, const char* prefix, int);
  NK_API void value_uint(struct context*, const char* prefix, unsigned int);
  NK_API void value_float(struct context*, const char* prefix, float);
  NK_API void value_color_byte(struct context*, const char* prefix, struct color);
  NK_API void value_color_float(struct context*, const char* prefix, struct color);
  NK_API void value_color_hex(struct context*, const char* prefix, struct color);
#endif
  /* =============================================================================
   *
   *                                  BUTTON
   *
   * ============================================================================= */
  NK_API bool button_text(context*, const char* title, int len);
  NK_API bool button_label(context*, const char* title);
  NK_API bool button_color(context*, color);
  NK_API bool button_symbol(context*, symbol_type);
  NK_API bool button_image(context*, struct image img);
  NK_API bool button_symbol_label(context*, symbol_type, const char*, flag text_alignment);
  NK_API bool button_symbol_text(context*, symbol_type, const char*, int, flag alignment);
  NK_API bool button_image_label(context*, struct image img, const char*, flag text_alignment);
  NK_API bool button_image_text(context*, struct image img, const char*, int, flag alignment);
  NK_API bool button_text_styled(context*, const style_button*, const char* title, int len);
  NK_API bool button_label_styled(context*, const style_button*, const char* title);
  NK_API bool button_symbol_styled(context*, const style_button*, symbol_type);
  NK_API bool button_image_styled(context*, const style_button*, struct image img);
  NK_API bool button_symbol_text_styled(context*, const style_button*, symbol_type, const char*, int, flag alignment);
  NK_API bool button_symbol_label_styled(context* ctx, const style_button* style, symbol_type symbol, const char* title, flag align);
  NK_API bool button_image_label_styled(context*, const style_button*, struct image img, const char*, flag text_alignment);
  NK_API bool button_image_text_styled(context*, const style_button*, struct image img, const char*, int, flag alignment);
  NK_API void button_set_behavior(context*, nk::btn_behavior);
  NK_API bool button_push_behavior(context*, nk::btn_behavior);
  NK_API bool button_pop_behavior(context*);
  /* =============================================================================
   *
   *                                  CHECKBOX
   *
   * ============================================================================= */
  NK_API bool check_label(context*, const char*, bool active);
  NK_API bool check_text(context*, const char*, int, bool active);
  NK_API bool check_text_align(context*, const char*, int, bool active, flag widget_alignment, flag text_alignment);
  NK_API unsigned check_flags_label(context*, const char*, unsigned int flags, unsigned int value);
  NK_API unsigned check_flags_text(context*, const char*, int, unsigned int flags, unsigned int value);
  NK_API bool checkbox_label(context*, const char*, bool* active);
  NK_API bool checkbox_label_align(context* ctx, const char* label, bool* active, flag widget_alignment, flag text_alignment);
  NK_API bool checkbox_text(context*, const char*, int, bool* active);
  NK_API bool checkbox_text_align(context* ctx, const char* text, int len, bool* active, flag widget_alignment, flag text_alignment);
  NK_API bool checkbox_flags_label(context*, const char*, unsigned int* flags, unsigned int value);
  NK_API bool checkbox_flags_text(context*, const char*, int, unsigned int* flags, unsigned int value);
  /* =============================================================================
   *
   *                                  RADIO BUTTON
   *
   * ============================================================================= */
  NK_API bool radio_label(context*, const char*, bool* active);
  NK_API bool radio_label_align(context* ctx, const char* label, bool* active, flag widget_alignment, flag text_alignment);
  NK_API bool radio_text(context*, const char*, int, bool* active);
  NK_API bool radio_text_align(context* ctx, const char* text, int len, bool* active, flag widget_alignment, flag text_alignment);
  NK_API bool option_label(context*, const char*, bool active);
  NK_API bool option_label_align(context* ctx, const char* label, bool active, flag widget_alignment, flag text_alignment);
  NK_API bool option_text(context*, const char*, int, bool active);
  NK_API bool option_text_align(context* ctx, const char* text, int len, bool is_active, flag widget_alignment, flag text_alignment);
  /* =============================================================================
   *
   *                                  SELECTABLE
   *
   * ============================================================================= */
  NK_API bool selectable_label(context*, const char*, flag align, bool* value);
  NK_API bool selectable_text(context*, const char*, int, flag align, bool* value);
  NK_API bool selectable_image_label(context*, struct image, const char*, flag align, bool* value);
  NK_API bool selectable_image_text(context*, struct image, const char*, int, flag align, bool* value);
  NK_API bool selectable_symbol_label(context*, symbol_type, const char*, flag align, bool* value);
  NK_API bool selectable_symbol_text(context*, symbol_type, const char*, int, flag align, bool* value);

  NK_API bool select_label(context*, const char*, flag align, bool value);
  NK_API bool select_text(context*, const char*, int, flag align, bool value);
  NK_API bool select_image_label(context*, struct image, const char*, flag align, bool value);
  NK_API bool select_image_text(context*, struct image, const char*, int, flag align, bool value);
  NK_API bool select_symbol_label(context*, symbol_type, const char*, flag align, bool value);
  NK_API bool select_symbol_text(context*, symbol_type, const char*, int, flag align, bool value);

  /* =============================================================================
   *
   *                                  SLIDER
   *
   * ============================================================================= */
  NK_API float slide_float(context*, float min, float val, float max, float step);
  NK_API int slide_int(context*, int min, int val, int max, int step);
  NK_API bool slider_float(context*, float min, float* val, float max, float step);
  NK_API bool slider_int(context*, int min, int* val, int max, int step);

  /* =============================================================================
   *
   *                                   KNOB
   *
   * ============================================================================= */
  NK_API bool knob_float(context*, float min, float* val, float max, float step, nk::heading zero_direction, float dead_zone_degrees);
  NK_API bool knob_int(context*, int min, int* val, int max, int step, nk::heading zero_direction, float dead_zone_degrees);

  /* =============================================================================
   *
   *                                  PROGRESSBAR
   *
   * ============================================================================= */
  NK_API bool progress(context*, std::size_t* cur, std::size_t max, bool modifyable);
  NK_API std::size_t prog(context*, std::size_t cur, std::size_t max, bool modifyable);

  /* =============================================================================
   *
   *                                  COLOR PICKER
   *
   * ============================================================================= */
  NK_API colorf color_picker(context*, colorf, nk::color_format);
  NK_API bool color_pick(context*, colorf*, nk::color_format);

  NK_API flag edit_string(context*, flag, char* buffer, int* len, int max, plugin_filter);
  NK_API flag edit_string_zero_terminated(context*, flag, char* buffer, int max, plugin_filter);
  NK_API flag edit_buffer(context*, flag, text_edit*, plugin_filter);
  NK_API void edit_focus(context*, flag flags);
  NK_API void edit_unfocus(context*);
  /* =============================================================================
   *
   *                                  CHART
   *
   * ============================================================================= */
  NK_API bool chart_begin(context*, nk::chart_type, int num, float min, float max);
  NK_API bool chart_begin_colored(context*, nk::chart_type, color, color active, int num, float min, float max);
  NK_API void chart_add_slot(context* ctx, const nk::chart_type, int count, float min_value, float max_value);
  NK_API void chart_add_slot_colored(context* ctx, const nk::chart_type, color, color active, int count, float min_value, float max_value);
  NK_API flag chart_push(context*, float);
  NK_API flag chart_push_slot(context*, float, int);
  NK_API void chart_end(context*);
  NK_API void plot(context*, nk::chart_type, const float* values, int count, int offset);
  NK_API void plot_function(context*, nk::chart_type, void* userdata, float (*value_getter)(void* user, int index), int count, int offset);
  /* =============================================================================
   *
   *                                  POPUP
   *
   * ============================================================================= */
  NK_API bool popup_begin(context*, nk::popup_type, const char*, flag, rectf bounds);
  NK_API void popup_close(context*);
  NK_API void popup_end(context*);
  NK_API void popup_get_scroll(const context*, std::uint32_t* offset_x, std::uint32_t* offset_y);
  NK_API void popup_set_scroll(context*, std::uint32_t offset_x, std::uint32_t offset_y);
  /* =============================================================================
   *
   *                                  COMBOBOX
   *
   * ============================================================================= */
  NK_API int combo(context*, const char* const* items, int count, int selected, int item_height, vec2f size);
  NK_API int combo_separator(context*, const char* items_separated_by_separator, int separator, int selected, int count, int item_height, vec2f size);
  NK_API int combo_string(context*, const char* items_separated_by_zeros, int selected, int count, int item_height, vec2f size);
  NK_API int combo_callback(context*, void (*item_getter)(void*, int, const char**), void* userdata, int selected, int count, int item_height, vec2f size);
  NK_API void combobox(context*, const char* const* items, int count, int* selected, int item_height, vec2f size);
  NK_API void combobox_string(context*, const char* items_separated_by_zeros, int* selected, int count, int item_height, vec2f size);
  NK_API void combobox_separator(context*, const char* items_separated_by_separator, int separator, int* selected, int count, int item_height, vec2f size);
  NK_API void combobox_callback(context*, void (*item_getter)(void*, int, const char**), void*, int* selected, int count, int item_height, vec2f size);
  /* =============================================================================
   *
   *                                  ABSTRACT COMBOBOX
   *
   * ============================================================================= */
  NK_API bool combo_begin_text(context*, const char* selected, int, vec2f size);
  NK_API bool combo_begin_label(context*, const char* selected, vec2f size);
  NK_API bool combo_begin_color(context*, color color, vec2f size);
  NK_API bool combo_begin_symbol(context*, symbol_type, vec2f size);
  NK_API bool combo_begin_symbol_label(context*, const char* selected, symbol_type, vec2f size);
  NK_API bool combo_begin_symbol_text(context*, const char* selected, int, symbol_type, vec2f size);
  NK_API bool combo_begin_image(context*, struct image img, vec2f size);
  NK_API bool combo_begin_image_label(context*, const char* selected, struct image, vec2f size);
  NK_API bool combo_begin_image_text(context*, const char* selected, int, struct image, vec2f size);
  NK_API bool combo_item_label(context*, const char*, flag alignment);
  NK_API bool combo_item_text(context*, const char*, int, flag alignment);
  NK_API bool combo_item_image_label(context*, struct image, const char*, flag alignment);
  NK_API bool combo_item_image_text(context*, struct image, const char*, int, flag alignment);
  NK_API bool combo_item_symbol_label(context*, symbol_type, const char*, flag alignment);
  NK_API bool combo_item_symbol_text(context*, symbol_type, const char*, int, flag alignment);
  NK_API void combo_close(context*);
  NK_API void combo_end(context*);
  /* =============================================================================
   *
   *                                  CONTEXTUAL
   *
   * ============================================================================= */
  NK_API bool contextual_begin(context*, flag, vec2f, rectf trigger_bounds);
  NK_API bool contextual_item_text(context*, const char*, int, flag align);
  NK_API bool contextual_item_label(context*, const char*, flag align);
  NK_API bool contextual_item_image_label(context*, struct image, const char*, flag alignment);
  NK_API bool contextual_item_image_text(context*, struct image, const char*, int len, flag alignment);
  NK_API bool contextual_item_symbol_label(context*, symbol_type, const char*, flag alignment);
  NK_API bool contextual_item_symbol_text(context*, symbol_type, const char*, int, flag alignment);
  NK_API void contextual_close(context*);
  NK_API void contextual_end(context*);
  /* =============================================================================
   *
   *                                  TOOLTIP
   *
   * ============================================================================= */
  NK_API void tooltip(context*, const char*);
#ifdef NK_INCLUDE_STANDARD_VARARGS
  NK_API void tooltipf(struct context*, NK_PRINTF_FORMAT_STRING const char*, ...) NK_PRINTF_VARARG_FUNC(2);
  NK_API void tooltipfv(struct context*, NK_PRINTF_FORMAT_STRING const char*, va_list) NK_PRINTF_VALIST_FUNC(2);
#endif
  NK_API bool tooltip_begin(context*, float width);
  NK_API void tooltip_end(context*);
  /* =============================================================================
   *
   *                                  MENU
   *
   * ============================================================================= */
  NK_API void menubar_begin(context*);
  NK_API void menubar_end(context*);
  NK_API bool menu_begin_text(context*, const char* title, int title_len, flag align, vec2f size);
  NK_API bool menu_begin_label(context*, const char*, flag align, vec2f size);
  NK_API bool menu_begin_image(context*, const char*, struct image, vec2f size);
  NK_API bool menu_begin_image_text(context*, const char*, int, flag align, struct image, vec2f size);
  NK_API bool menu_begin_image_label(context*, const char*, flag align, struct image, vec2f size);
  NK_API bool menu_begin_symbol(context*, const char*, symbol_type, vec2f size);
  NK_API bool menu_begin_symbol_text(context*, const char*, int, flag align, symbol_type, vec2f size);
  NK_API bool menu_begin_symbol_label(context*, const char*, flag align, symbol_type, vec2f size);
  NK_API bool menu_item_text(context*, const char*, int, flag align);
  NK_API bool menu_item_label(context*, const char*, flag alignment);
  NK_API bool menu_item_image_label(context*, struct image, const char*, flag alignment);
  NK_API bool menu_item_image_text(context*, struct image, const char*, int len, flag alignment);
  NK_API bool menu_item_symbol_text(context*, symbol_type, const char*, int, flag alignment);
  NK_API bool menu_item_symbol_label(context*, symbol_type, const char*, flag alignment);
  NK_API void menu_close(context*);
  NK_API void menu_end(context*);

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
  NK_API bool filter_default(const text_edit*, rune unicode);
  NK_API bool filter_ascii(const text_edit*, rune unicode);
  NK_API bool filter_float(const text_edit*, rune unicode);
  NK_API bool filter_decimal(const text_edit*, rune unicode);
  NK_API bool filter_hex(const text_edit*, rune unicode);
  NK_API bool filter_oct(const text_edit*, rune unicode);
  NK_API bool filter_binary(const text_edit*, rune unicode);

  /** text editor */
#ifdef NK_INCLUDE_DEFAULT_ALLOCATOR
  NK_API void textedit_init_default(struct text_edit*);
#endif
  NK_API void textedit_init(text_edit*, const allocator*, std::size_t size);
  NK_API void textedit_init_fixed(text_edit*, void* memory, std::size_t size);
  NK_API void textedit_free(text_edit*);
  NK_API void textedit_text(text_edit*, const char*, int total_len);
  NK_API void textedit_delete(text_edit*, int where, int len);
  NK_API void textedit_delete_selection(text_edit*);
  NK_API void textedit_select_all(text_edit*);
  NK_API bool textedit_cut(text_edit*);
  NK_API bool textedit_paste(text_edit*, char const*, int len);
  NK_API void textedit_undo(text_edit*);
  NK_API void textedit_redo(text_edit*);

  /* popup */
  NK_LIB bool nonblock_begin(context* ctx, flag flags, rectf body, rectf header, panel_type::value_type panel_type);

  NK_LIB void widget_text(command_buffer* o, rectf b, const char* string, int len, const text* t, flag a, const user_font* f);
  NK_LIB void widget_text_wrap(command_buffer* o, rectf b, const char* string, int len, const text* t, const user_font* f);

  /* button */
  NK_LIB bool button_behavior(flag* state, rectf r, const input* i, btn_behavior behavior);
  NK_LIB const style_item* draw_button(command_buffer* out, const rectf* bounds, flag state, const style_button* style);
  NK_LIB bool do_button(flag* state, command_buffer* out, rectf r, const style_button* style, const input* in, btn_behavior behavior, rectf* content);
  NK_LIB void draw_button_text(command_buffer* out, const rectf* bounds, const rectf* content, flag state, const style_button* style, const char* txt, int len, flag text_alignment, const user_font* font);
  NK_LIB bool do_button_text(flag* state, command_buffer* out, rectf bounds, const char* string, int len, flag align, btn_behavior behavior, const style_button* style, const input* in, const user_font* font);
  NK_LIB void draw_button_symbol(command_buffer* out, const rectf* bounds, const rectf* content, flag state, const style_button* style, symbol_type type, const user_font* font);
  NK_LIB bool do_button_symbol(flag* state, command_buffer* out, rectf bounds, symbol_type symbol, btn_behavior behavior, const style_button* style, const input* in, const user_font* font);
  NK_LIB void draw_button_image(command_buffer* out, const rectf* bounds, const rectf* content, flag state, const style_button* style, const struct image* img);
  NK_LIB bool do_button_image(flag* state, command_buffer* out, rectf bounds, struct image img, btn_behavior b, const style_button* style, const input* in);
  NK_LIB void draw_button_text_symbol(command_buffer* out, const rectf* bounds, const rectf* label, const rectf* symbol, flag state, const style_button* style, const char* str, int len, symbol_type type, const user_font* font);
  NK_LIB bool do_button_text_symbol(flag* state, command_buffer* out, rectf bounds, symbol_type symbol, const char* str, int len, flag align, btn_behavior behavior, const style_button* style, const user_font* font, const input* in);
  NK_LIB void draw_button_text_image(command_buffer* out, const rectf* bounds, const rectf* label, const rectf* image, flag state, const style_button* style, const char* str, int len, const user_font* font, const struct image* img);
  NK_LIB bool do_button_text_image(flag* state, command_buffer* out, rectf bounds, struct image img, const char* str, int len, flag align, btn_behavior behavior, const style_button* style, const user_font* font, const input* in);

  NK_LIB bool toggle_behavior(const input* in, rectf select, flag* state, bool active);
  NK_LIB void draw_checkbox(command_buffer* out, flag state, const style_toggle* style, bool active, const rectf* label, const rectf* selector, const rectf* cursors, const char* string, int len, const user_font* font, flag text_alignment);
  NK_LIB void draw_option(command_buffer* out, flag state, const style_toggle* style, bool active, const rectf* label, const rectf* selector, const rectf* cursors, const char* string, int len, const user_font* font, flag text_alignment);
  NK_LIB bool do_toggle(flag* state, command_buffer* out, rectf r, bool* active, const char* str, int len, toggle_type type, const style_toggle* style, const input* in, const user_font* font, flag widget_alignment, flag text_alignment);

  /* progress */
  NK_LIB std::size_t progress_behavior(flag* state, input* in, rectf r, rectf cursor, std::size_t max, std::size_t value, bool modifiable);
  NK_LIB void draw_progress(command_buffer* out, flag state, const style_progress* style, const rectf* bounds, const rectf* scursor, std::size_t value, std::size_t max);
  NK_LIB std::size_t do_progress(flag* state, command_buffer* out, rectf bounds, std::size_t value, std::size_t max, bool modifiable, const style_progress* style, input* in);

  /* slider */
  NK_LIB float slider_behavior(flag* state, rectf* logical_cursor, rectf* visual_cursor, input* in, rectf bounds, float slider_min, float slider_max, float slider_value, float slider_step, float slider_steps);
  NK_LIB void draw_slider(command_buffer* out, flag state, const style_slider* style, const rectf* bounds, const rectf* visual_cursor, float min, float value, float max);
  NK_LIB float do_slider(flag* state, command_buffer* out, rectf bounds, float min, float val, float max, float step, const style_slider* style, input* in, const user_font* font);

  /* scrollbar */
  NK_LIB float scrollbar_behavior(flag* state, input* in, int has_scrolling, const rectf* scroll, const rectf* cursor, const rectf* empty0, const rectf* empty1, float scroll_offset, float target, float scroll_step, orientation o);
  NK_LIB void draw_scrollbar(command_buffer* out, flag state, const style_scrollbar* style, const rectf* bounds, const rectf* scroll);
  NK_LIB float do_scrollbarv(flag* state, command_buffer* out, rectf scroll, int has_scrolling, float offset, float target, float step, float button_pixel_inc, const style_scrollbar* style, input* in, const user_font* font);
  NK_LIB float do_scrollbarh(flag* state, command_buffer* out, rectf scroll, int has_scrolling, float offset, float target, float step, float button_pixel_inc, const style_scrollbar* style, input* in, const user_font* font);

  /* selectable */
  NK_LIB void draw_selectable(command_buffer* out, flag state, const style_selectable* style, bool active, const rectf* bounds, const rectf* icon, const struct image* img, symbol_type sym, const char* string, int len, flag align, const user_font* font);
  NK_LIB bool do_selectable(flag* state, command_buffer* out, rectf bounds, const char* str, int len, flag align, bool* value, const style_selectable* style, const input* in, const user_font* font);
  NK_LIB bool do_selectable_image(flag* state, command_buffer* out, rectf bounds, const char* str, int len, flag align, bool* value, const struct image* img, const style_selectable* style, const input* in, const user_font* font);

  /* edit */
  NK_LIB void edit_draw_text(command_buffer* out, const style_edit* style, float pos_x, float pos_y, float x_offset, const char* text, int byte_len, float row_height, const user_font* font, color background, color foreground, bool is_selected);
  NK_LIB flag do_edit(flag* state, command_buffer* out, rectf bounds, flag flags, plugin_filter filter, text_edit* edit, const style_edit* style, input* in, const user_font* font);

  /* color-picker */
  NK_LIB bool color_picker_behavior(flag* state, const rectf* bounds, const rectf* matrix, const rectf* hue_bar, const rectf* alpha_bar, colorf* color, const input* in);
  NK_LIB void draw_color_picker(command_buffer* o, const rectf* matrix, const rectf* hue_bar, const rectf* alpha_bar, colorf col);
  NK_LIB bool do_color_picker(flag* state, command_buffer* out, colorf* col, color_format fmt, rectf bounds, vec2f padding, const input* in, const user_font* font);

}

#endif