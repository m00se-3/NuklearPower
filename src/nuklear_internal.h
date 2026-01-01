#ifndef INTERNAL_H
#define INTERNAL_H

#ifndef NK_POOL_DEFAULT_CAPACITY
#define NK_POOL_DEFAULT_CAPACITY 16
#endif

#ifndef NK_DEFAULT_COMMAND_BUFFER_SIZE
#define NK_DEFAULT_COMMAND_BUFFER_SIZE (4 * 1024)
#endif

#ifndef NK_BUFFER_DEFAULT_INITIAL_SIZE
#define NK_BUFFER_DEFAULT_INITIAL_SIZE (4 * 1024)
#endif

/* standard library headers */
#ifdef NK_INCLUDE_DEFAULT_ALLOCATOR
#include <stdlib.h> /* malloc, free */
#endif
#ifdef NK_INCLUDE_STANDARD_IO
#include <stdio.h> /* fopen, fclose,... */
#endif
#ifdef NK_INCLUDE_STANDARD_VARARGS
#include <stdarg.h> /* valist, va_start, va_end, ... */
#endif
#ifndef NK_ASSERT
#include <cassert>
#define NK_ASSERT(expr) assert(expr)
#endif

#define NK_DEFAULT (-1)

#ifndef NK_VSNPRINTF
/* If your compiler does support `vsnprintf` I would highly recommend
 * defining this to vsnprintf instead since `vsprintf` is basically
 * unbelievable unsafe and should *NEVER* be used. But I have to support
 * it since C89 only provides this unsafe version. */
#if (defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)) || \
    (defined(__cplusplus) && (__cplusplus >= 201103L)) ||           \
    (defined(_POSIX_C_SOURCE) && (_POSIX_C_SOURCE >= 200112L)) ||   \
    (defined(_XOPEN_SOURCE) && (_XOPEN_SOURCE >= 500)) ||           \
    defined(_ISOC99_SOURCE) || defined(_BSD_SOURCE)
#define NK_VSNPRINTF(s, n, f, a) vsnprintf(s, n, f, a)
#else
#define NK_VSNPRINTF(s, n, f, a) vsprintf(s, f, a)
#endif
#endif

#define NK_SCHAR_MIN (-127)
#define NK_SCHAR_MAX 127
#define NK_UCHAR_MIN 0
#define NK_UCHAR_MAX 256
#define NK_SSHORT_MIN (-32767)
#define NK_SSHORT_MAX 32767
#define NK_USHORT_MIN 0
#define NK_USHORT_MAX 65535
#define NK_SINT_MIN (-2147483647)
#define NK_SINT_MAX 2147483647
#define NK_UINT_MIN 0
#define NK_UINT_MAX 4294967295u

/* Make sure correct type size:
 * This will fire with a negative subscript error if the type sizes
 * are set incorrectly by the compiler, and compile out if not */
NK_STATIC_ASSERT(sizeof(std::size_t) >= sizeof(void*));
NK_STATIC_ASSERT(sizeof(std::uintptr_t) == sizeof(void*));
NK_STATIC_ASSERT(sizeof(nk::flag) >= 4);
NK_STATIC_ASSERT(sizeof(nk::rune) >= 4);
NK_STATIC_ASSERT(sizeof(unsigned short) == 2);
NK_STATIC_ASSERT(sizeof(short) == 2);
NK_STATIC_ASSERT(sizeof(std::uint32_t) == 4);
NK_STATIC_ASSERT(sizeof(std::int32_t) == 4);
NK_STATIC_ASSERT(sizeof(std::byte) == 1);

namespace nk {
  NK_GLOBAL constexpr rectf null_rect = {.x = -8192.0f, .y = -8192.0f, .w = 16384, .h = 16384};
#define NK_FLOAT_PRECISION 0.00000000000001

  NK_GLOBAL constexpr color red = {.r = 255, .g = 0, .b = 0, .a = 255};
  NK_GLOBAL constexpr color green = {.r = 0, .g = 255, .b = 0, .a = 255};
  NK_GLOBAL constexpr color blue = {.r = 0, .g = 0, .b = 255, .a = 255};
  NK_GLOBAL constexpr color white = {.r = 255, .g = 255, .b = 255, .a = 255};
  NK_GLOBAL constexpr color black = {.r = 0, .g = 0, .b = 0, .a = 255};
  NK_GLOBAL constexpr color yellow = {.r = 255, .g = 255, .b = 0, .a = 255};

  /* widget */
  constexpr auto widget_state_reset = [](auto* state) {
    *state = NK_WIDGET_STATE_INACTIVE;
    if ((*state & NK_WIDGET_STATE_MODIFIED) == 1u) {
      *state |= NK_WIDGET_STATE_MODIFIED;
    }
  };


/* math */
#ifndef NK_INV_SQRT
  NK_LIB float inv_sqrt(float n);
#endif
#ifndef NK_SIN
  NK_LIB float sin(float x);
#endif
#ifndef NK_COS
  NK_LIB float cos(float x);
#endif
#ifndef NK_ATAN
  NK_LIB float atan(float x);
#endif
#ifndef NK_ATAN2
  NK_LIB float atan2(float y, float x);
#endif
  NK_LIB std::size_t round_up_pow2(std::size_t v);
  NK_LIB rectf shrirect(rectf r, float amount);
  NK_LIB rectf pad_rect(rectf r, vec2f pad);
  NK_LIB void unify(rectf* clip, const rectf* a, float x0, float y0, float x1, float y1);
  NK_LIB double pow(double x, int n);
  NK_LIB int ifloord(double x);
  NK_LIB int ifloorf(float x);
  NK_LIB int iceilf(float x);
  NK_LIB int log10(double n);
  NK_LIB float roundf(float x);

  /* util */
  enum { NK_DO_NOT_STOP_ON_NEW_LINE,
         NK_STOP_ON_NEW_LINE };
  NK_LIB bool is_lower(int c);
  NK_LIB bool is_upper(int c);
  NK_LIB int to_upper(int c);
  NK_LIB int to_lower(int c);

#ifndef NK_MEMCPY
  NK_LIB void* memcopy(void* dst, const void* src, std::size_t n);
#endif
#ifndef NK_MEMSET
  NK_LIB void memset(void* ptr, int c0, std::size_t size);
#endif
  NK_LIB void zero(void* ptr, std::size_t size);
  NK_LIB char* itoa(char* s, long n);
  NK_LIB int string_float_limit(char* string, int prec);
#ifndef NK_DTOA
  NK_LIB char* dtoa(char* s, double n);
#endif
  NK_LIB int text_clamp(const user_font* font, const char* text, int text_len, float space, int* glyphs, float* text_width, rune* sep_list, int sep_count);
  NK_LIB vec2f text_calculate_text_bounds(const user_font* font, const char* begin, int byte_len, float row_height, const char** remaining, vec2f* out_offset, int* glyphs, int op);
#ifdef NK_INCLUDE_STANDARD_VARARGS
  NK_LIB int strfmt(char* buf, int buf_size, const char* fmt, va_list args);
#endif
#ifdef NK_INCLUDE_STANDARD_IO
  NK_LIB char* file_load(const char* path, std::size_t* siz, const struct allocator* alloc);
#endif

/* buffer */
#ifdef NK_INCLUDE_DEFAULT_ALLOCATOR
  NK_LIB void* malloc(handle unused, void* old, std::size_t size);
  NK_LIB void mfree(handle unused, void* ptr);
#endif
  NK_LIB void* buffer_align(void* unaligned, std::size_t align, std::size_t* alignment, buffer_allocation_type type);
  NK_LIB void* buffer_alloc(memory_buffer* b, buffer_allocation_type type, std::size_t size, std::size_t align);
  NK_LIB void* buffer_realloc(memory_buffer* b, std::size_t capacity, std::size_t* size);

  /* draw */
  NK_LIB void command_buffer_init(command_buffer* cb, memory_buffer* b, command_clipping clip);
  NK_LIB void command_buffer_reset(command_buffer* b);
  NK_LIB void* command_buffer_push(command_buffer* b, command_type t, std::size_t size);
  NK_LIB void draw_symbol(command_buffer* out, symbol_type type, rectf content, color background, color foreground, float border_width, const user_font* font);

  /* buffering */
  NK_LIB void start_buffer(context* ctx, command_buffer* b);
  NK_LIB void start(context* ctx, window* win);
  NK_LIB void start_popup(context* ctx, window* win);
  NK_LIB void finish_popup(context* ctx, window*);
  NK_LIB void finish_buffer(context* ctx, command_buffer* b);
  NK_LIB void finish(context* ctx, window* w);
  NK_LIB void build(context* ctx);

  /* text editor */
  NK_LIB void textedit_clear_state(text_edit* state, text_edit_type type, plugin_filter filter);
  NK_LIB void textedit_click(text_edit* state, float x, float y, const user_font* font, float row_height);
  NK_LIB void textedit_drag(text_edit* state, float x, float y, const user_font* font, float row_height);
  NK_LIB void textedit_key(text_edit* state, keys key, int shift_mod, const user_font* font, float row_height);

  /* window */
  enum window_insert_location {
    NK_INSERT_BACK, /* inserts window into the back of list (front of screen) */
    NK_INSERT_FRONT /* inserts window into the front of list (back of screen) */
  };
  NK_LIB void* create_window(context* ctx);
  NK_LIB void remove_window(context*, window*);
  NK_LIB void free_window(context* ctx, window* win);
  NK_LIB window* find_window(const context* ctx, hash hash, const char* name);
  NK_LIB void insert_window(context* ctx, window* win, window_insert_location loc);

  /* pool */
  NK_LIB void pool_init(pool* pool, const allocator* alloc, unsigned int capacity);
  NK_LIB void pool_free(pool* pool);
  NK_LIB void pool_init_fixed(pool* pool, void* memory, std::size_t size);
  NK_LIB page_element* pool_alloc(pool* pool);

  /* page-element */
  NK_LIB page_element* create_page_element(context* ctx);
  NK_LIB void lipage_element_into_freelist(context* ctx, page_element* elem);
  NK_LIB void free_page_element(context* ctx, page_element* elem);

  /* table */
  NK_LIB table* create_table(context* ctx);
  NK_LIB void remove_table(window* win, table* tbl);
  NK_LIB void free_table(context* ctx, table* tbl);
  NK_LIB void push_table(window* win, table* tbl);
  NK_LIB std::uint32_t* add_value(context* ctx, window* win, hash name, std::uint32_t value);
  NK_LIB std::uint32_t* find_value(const window* win, hash name);

  /* panel */
  NK_LIB void* create_panel(context* ctx);
  NK_LIB void free_panel(context*, panel* pan);
  NK_LIB bool panel_has_header(flag flags, const char* title);
  NK_LIB vec2f panel_get_padding(const style* style, panel_type::value_type type);
  NK_LIB float panel_get_border(const style* style, flag flags, panel_type::value_type type);
  NK_LIB color panel_get_border_color(const style* style, panel_type::value_type type);
  NK_LIB bool panel_is_sub(panel_type::value_type type);
  NK_LIB bool panel_is_nonblock(panel_type::value_type type);
  NK_LIB bool panel_begin(context* ctx, const char* title, panel_type::value_type panel_type);
  NK_LIB void panel_end(context* ctx);

  /* layout */
  NK_LIB float layout_row_calculate_usable_space(const style* style, panel_type::value_type type, float total_space, int columns);
  NK_LIB void panel_layout(const context* ctx, window* win, float height, int cols);
  NK_LIB void row_layout(context* ctx, layout_format fmt, float height, int cols, int width);
  NK_LIB void panel_alloc_row(const context* ctx, window* win);
  NK_LIB void layout_widget_space(rectf* bounds, const context* ctx, window* win, int modify);
  NK_LIB void panel_alloc_space(rectf* bounds, const context* ctx);
  NK_LIB void layout_peek(rectf* bounds, const context* ctx);

  /* popup */
  NK_LIB bool nonblock_begin(context* ctx, flag flags, rectf body, rectf header, panel_type::value_type panel_type);

  /* text */
  struct text {
    vec2f padding;
    color background;
    color txt;
  };
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

  /* toggle */
  enum toggle_type {
    NK_TOGGLE_CHECK,
    NK_TOGGLE_OPTION
  };
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

  /* property */
  enum property_status {
    NK_PROPERTY_DEFAULT,
    NK_PROPERTY_EDIT,
    NK_PROPERTY_DRAG
  };
  enum property_filter {
    NK_FILTER_INT,
    NK_FILTER_FLOAT
  };
  enum property_kind {
    NK_PROPERTY_INT,
    NK_PROPERTY_FLOAT,
    NK_PROPERTY_DOUBLE
  };
  union property {
    int i;
    float f;
    double d;
  };
  struct property_variant {
    property_kind kind;
    property value;
    property min_value;
    property max_value;
    property step;
  };
  NK_LIB property_variant property_variant_int(int value, int min_value, int max_value, int step);
  NK_LIB property_variant property_variant_float(float value, float min_value, float max_value, float step);
  NK_LIB property_variant property_variant_double(double value, double min_value, double max_value, double step);

  NK_LIB void drag_behavior(flag* state, const input* in, rectf drag, property_variant* variant, float inc_per_pixel);
  NK_LIB void property_behavior(flag* ws, const input* in, rectf property, rectf label, rectf edit, rectf empty, int* state, property_variant* variant, float inc_per_pixel);
  NK_LIB void draw_property(command_buffer* out, const style_property* style, const rectf* bounds, const rectf* label, flag state, const char* name, int len, const user_font* font);
  NK_LIB void do_property(flag* ws, command_buffer* out, rectf property, const char* name, property_variant* variant, float inc_per_pixel, char* buffer, int* len, int* state, int* cursor, int* select_begin, int* select_end, const style_property* style, property_filter filter, input* in, const user_font* font, text_edit* text_edit, btn_behavior behavior);
  NK_LIB void property(context* ctx, const char* name, property_variant* variant, float inc_per_pixel, const property_filter filter);

#ifdef NK_INCLUDE_FONT_BAKING

/**
 * @def NK_NO_STB_RECT_PACK_IMPLEMENTATION
 *
 * When defined, will avoid enabling STB_RECT_PACK_IMPLEMENTATION for when stb_rect_pack.h is already implemented elsewhere.
 */
#ifndef NK_NO_STB_RECT_PACK_IMPLEMENTATION
#define STB_RECT_PACK_IMPLEMENTATION
#endif /* NK_NO_STB_RECT_PACK_IMPLEMENTATION */

/**
 * @def NK_NO_STB_TRUETYPE_IMPLEMENTATION
 *
 * When defined, will avoid enabling STB_TRUETYPE_IMPLEMENTATION for when stb_truetype.h is already implemented elsewhere.
 */
#ifndef NK_NO_STB_TRUETYPE_IMPLEMENTATION
#define STB_TRUETYPE_IMPLEMENTATION
#endif /* NK_NO_STB_TRUETYPE_IMPLEMENTATION */

/* Allow consumer to define own STBTT_malloc/STBTT_free, and use the font atlas' allocator otherwise */
#ifndef STBTT_malloc
  static void*
  stbtt_malloc(std::size_t size, void* user_data) {
    struct allocator* alloc = (struct allocator*) user_data;
    return alloc->alloc(alloc->userdata, 0, size);
  }

  static void
  stbtt_free(void* ptr, void* user_data) {
    struct allocator* alloc = (struct allocator*) user_data;
    alloc->free(alloc->userdata, ptr);
  }

#define STBTT_malloc(x, u) stbtt_malloc(x, u)
#define STBTT_free(x, u) stbtt_free(x, u)

#endif /* STBTT_malloc */

#endif /* NK_INCLUDE_FONT_BAKING */
} // namespace nk
#endif
