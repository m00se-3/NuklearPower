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
  NK_GLOBAL constexpr struct rectf null_rect = {.x = -8192.0f, .y = -8192.0f, .w = 16384, .h = 16384};
#define NK_FLOAT_PRECISION 0.00000000000001

  NK_GLOBAL constexpr color red = {.r = 255, .g = 0, .b = 0, .a = 255};
  NK_GLOBAL constexpr color green = {.r = 0, .g = 255, .b = 0, .a = 255};
  NK_GLOBAL constexpr color blue = {.r = 0, .g = 0, .b = 255, .a = 255};
  NK_GLOBAL constexpr color white = {.r = 255, .g = 255, .b = 255, .a = 255};
  NK_GLOBAL constexpr color black = {.r = 0, .g = 0, .b = 0, .a = 255};
  NK_GLOBAL constexpr color yellow = {.r = 255, .g = 255, .b = 0, .a = 255};

  /* widget */
  constexpr auto widget_state_reset = [](auto* state) {
    *state = NK_WIDGET_STATE_INACTIVE | (*state & NK_WIDGET_STATE_MODIFIED) ? NK_WIDGET_STATE_MODIFIED : 0;
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
  NK_LIB std::uint32_t round_up_pow2(std::uint32_t v);
  NK_LIB struct rectf shrirect(struct rectf r, float amount);
  NK_LIB struct rectf pad_rect(struct rectf r, struct vec2f pad);
  NK_LIB void unify(struct rectf* clip, const struct rectf* a, float x0, float y0, float x1, float y1);
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
  NK_LIB int text_clamp(const struct user_font* font, const char* text, int text_len, float space, int* glyphs, float* text_width, rune* sep_list, int sep_count);
  NK_LIB struct vec2f text_calculate_text_bounds(const struct user_font* font, const char* begin, int byte_len, float row_height, const char** remaining, struct vec2f* out_offset, int* glyphs, int op);
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
  NK_LIB void* buffer_align(void* unaligned, std::size_t align, std::size_t* alignment, enum buffer_allocation_type type);
  NK_LIB void* buffer_alloc(struct memory_buffer* b, enum buffer_allocation_type type, std::size_t size, std::size_t align);
  NK_LIB void* buffer_realloc(struct memory_buffer* b, std::size_t capacity, std::size_t* size);

  /* draw */
  NK_LIB void command_buffer_init(struct command_buffer* cb, struct memory_buffer* b, enum command_clipping clip);
  NK_LIB void command_buffer_reset(struct command_buffer* b);
  NK_LIB void* command_buffer_push(struct command_buffer* b, enum command_type t, std::size_t size);
  NK_LIB void draw_symbol(struct command_buffer* out, enum symbol_type type, struct rectf content, struct color background, struct color foreground, float border_width, const struct user_font* font);

  /* buffering */
  NK_LIB void start_buffer(struct context* ctx, struct command_buffer* b);
  NK_LIB void start(struct context* ctx, struct window* win);
  NK_LIB void start_popup(struct context* ctx, struct window* win);
  NK_LIB void finish_popup(struct context* ctx, struct window*);
  NK_LIB void finish_buffer(struct context* ctx, struct command_buffer* b);
  NK_LIB void finish(struct context* ctx, struct window* w);
  NK_LIB void build(struct context* ctx);

  /* text editor */
  NK_LIB void textedit_clear_state(struct text_edit* state, enum text_edit_type type, plugin_filter filter);
  NK_LIB void textedit_click(struct text_edit* state, float x, float y, const struct user_font* font, float row_height);
  NK_LIB void textedit_drag(struct text_edit* state, float x, float y, const struct user_font* font, float row_height);
  NK_LIB void textedit_key(struct text_edit* state, enum keys key, int shift_mod, const struct user_font* font, float row_height);

  /* window */
  enum window_insert_location {
    NK_INSERT_BACK, /* inserts window into the back of list (front of screen) */
    NK_INSERT_FRONT /* inserts window into the front of list (back of screen) */
  };
  NK_LIB void* create_window(struct context* ctx);
  NK_LIB void remove_window(struct context*, struct window*);
  NK_LIB void free_window(struct context* ctx, struct window* win);
  NK_LIB struct window* find_window(const struct context* ctx, hash hash, const char* name);
  NK_LIB void insert_window(struct context* ctx, struct window* win, enum window_insert_location loc);

  /* pool */
  NK_LIB void pool_init(struct pool* pool, const struct allocator* alloc, unsigned int capacity);
  NK_LIB void pool_free(struct pool* pool);
  NK_LIB void pool_init_fixed(struct pool* pool, void* memory, std::size_t size);
  NK_LIB struct page_element* pool_alloc(struct pool* pool);

  /* page-element */
  NK_LIB struct page_element* create_page_element(struct context* ctx);
  NK_LIB void lipage_element_into_freelist(struct context* ctx, struct page_element* elem);
  NK_LIB void free_page_element(struct context* ctx, struct page_element* elem);

  /* table */
  NK_LIB struct table* create_table(struct context* ctx);
  NK_LIB void remove_table(struct window* win, struct table* tbl);
  NK_LIB void free_table(struct context* ctx, struct table* tbl);
  NK_LIB void push_table(struct window* win, struct table* tbl);
  NK_LIB std::uint32_t* add_value(struct context* ctx, struct window* win, hash name, std::uint32_t value);
  NK_LIB std::uint32_t* find_value(const struct window* win, hash name);

  /* panel */
  NK_LIB void* create_panel(struct context* ctx);
  NK_LIB void free_panel(struct context*, struct panel* pan);
  NK_LIB bool panel_has_header(flag flags, const char* title);
  NK_LIB struct vec2f panel_get_padding(const struct style* style, panel_type::value_type type);
  NK_LIB float panel_get_border(const struct style* style, flag flags, panel_type::value_type type);
  NK_LIB struct color panel_get_border_color(const struct style* style, panel_type::value_type type);
  NK_LIB bool panel_is_sub(panel_type::value_type type);
  NK_LIB bool panel_is_nonblock(panel_type::value_type type);
  NK_LIB bool panel_begin(struct context* ctx, const char* title, panel_type::value_type panel_type);
  NK_LIB void panel_end(struct context* ctx);

  /* layout */
  NK_LIB float layout_row_calculate_usable_space(const struct style* style, panel_type::value_type type, float total_space, int columns);
  NK_LIB void panel_layout(const struct context* ctx, struct window* win, float height, int cols);
  NK_LIB void row_layout(struct context* ctx, layout_format fmt, float height, int cols, int width);
  NK_LIB void panel_alloc_row(const struct context* ctx, struct window* win);
  NK_LIB void layout_widget_space(struct rectf* bounds, const struct context* ctx, struct window* win, int modify);
  NK_LIB void panel_alloc_space(struct rectf* bounds, const struct context* ctx);
  NK_LIB void layout_peek(struct rectf* bounds, const struct context* ctx);

  /* popup */
  NK_LIB bool nonblock_begin(struct context* ctx, flag flags, struct rectf body, struct rectf header, panel_type::value_type panel_type);

  /* text */
  struct text {
    struct vec2f padding;
    struct color background;
    struct color txt;
  };
  NK_LIB void widget_text(struct command_buffer* o, struct rectf b, const char* string, int len, const struct text* t, flag a, const struct user_font* f);
  NK_LIB void widget_text_wrap(struct command_buffer* o, struct rectf b, const char* string, int len, const struct text* t, const struct user_font* f);

  /* button */
  NK_LIB bool button_behavior(flag* state, struct rectf r, const struct input* i, btn_behavior behavior);
  NK_LIB const struct style_item* draw_button(struct command_buffer* out, const struct rectf* bounds, flag state, const struct style_button* style);
  NK_LIB bool do_button(flag* state, struct command_buffer* out, struct rectf r, const struct style_button* style, const struct input* in, btn_behavior behavior, struct rectf* content);
  NK_LIB void draw_button_text(struct command_buffer* out, const struct rectf* bounds, const struct rectf* content, flag state, const struct style_button* style, const char* txt, int len, flag text_alignment, const struct user_font* font);
  NK_LIB bool do_button_text(flag* state, struct command_buffer* out, struct rectf bounds, const char* string, int len, flag align, btn_behavior behavior, const struct style_button* style, const struct input* in, const struct user_font* font);
  NK_LIB void draw_button_symbol(struct command_buffer* out, const struct rectf* bounds, const struct rectf* content, flag state, const struct style_button* style, enum symbol_type type, const struct user_font* font);
  NK_LIB bool do_button_symbol(flag* state, struct command_buffer* out, struct rectf bounds, enum symbol_type symbol, btn_behavior behavior, const struct style_button* style, const struct input* in, const struct user_font* font);
  NK_LIB void draw_button_image(struct command_buffer* out, const struct rectf* bounds, const struct rectf* content, flag state, const struct style_button* style, const struct image* img);
  NK_LIB bool do_button_image(flag* state, struct command_buffer* out, struct rectf bounds, struct image img, btn_behavior b, const struct style_button* style, const struct input* in);
  NK_LIB void draw_button_text_symbol(struct command_buffer* out, const struct rectf* bounds, const struct rectf* label, const struct rectf* symbol, flag state, const struct style_button* style, const char* str, int len, enum symbol_type type, const struct user_font* font);
  NK_LIB bool do_button_text_symbol(flag* state, struct command_buffer* out, struct rectf bounds, enum symbol_type symbol, const char* str, int len, flag align, btn_behavior behavior, const struct style_button* style, const struct user_font* font, const struct input* in);
  NK_LIB void draw_button_text_image(struct command_buffer* out, const struct rectf* bounds, const struct rectf* label, const struct rectf* image, flag state, const struct style_button* style, const char* str, int len, const struct user_font* font, const struct image* img);
  NK_LIB bool do_button_text_image(flag* state, struct command_buffer* out, struct rectf bounds, struct image img, const char* str, int len, flag align, btn_behavior behavior, const struct style_button* style, const struct user_font* font, const struct input* in);

  /* toggle */
  enum toggle_type {
    NK_TOGGLE_CHECK,
    NK_TOGGLE_OPTION
  };
  NK_LIB bool toggle_behavior(const struct input* in, struct rectf select, flag* state, bool active);
  NK_LIB void draw_checkbox(struct command_buffer* out, flag state, const struct style_toggle* style, bool active, const struct rectf* label, const struct rectf* selector, const struct rectf* cursors, const char* string, int len, const struct user_font* font, flag text_alignment);
  NK_LIB void draw_option(struct command_buffer* out, flag state, const struct style_toggle* style, bool active, const struct rectf* label, const struct rectf* selector, const struct rectf* cursors, const char* string, int len, const struct user_font* font, flag text_alignment);
  NK_LIB bool do_toggle(flag* state, struct command_buffer* out, struct rectf r, bool* active, const char* str, int len, enum toggle_type type, const struct style_toggle* style, const struct input* in, const struct user_font* font, flag widget_alignment, flag text_alignment);

  /* progress */
  NK_LIB std::size_t progress_behavior(flag* state, struct input* in, struct rectf r, struct rectf cursor, std::size_t max, std::size_t value, bool modifiable);
  NK_LIB void draw_progress(struct command_buffer* out, flag state, const struct style_progress* style, const struct rectf* bounds, const struct rectf* scursor, std::size_t value, std::size_t max);
  NK_LIB std::size_t do_progress(flag* state, struct command_buffer* out, struct rectf bounds, std::size_t value, std::size_t max, bool modifiable, const struct style_progress* style, struct input* in);

  /* slider */
  NK_LIB float slider_behavior(flag* state, struct rectf* logical_cursor, struct rectf* visual_cursor, struct input* in, struct rectf bounds, float slider_min, float slider_max, float slider_value, float slider_step, float slider_steps);
  NK_LIB void draw_slider(struct command_buffer* out, flag state, const struct style_slider* style, const struct rectf* bounds, const struct rectf* visual_cursor, float min, float value, float max);
  NK_LIB float do_slider(flag* state, struct command_buffer* out, struct rectf bounds, float min, float val, float max, float step, const struct style_slider* style, struct input* in, const struct user_font* font);

  /* scrollbar */
  NK_LIB float scrollbar_behavior(flag* state, struct input* in, int has_scrolling, const struct rectf* scroll, const struct rectf* cursor, const struct rectf* empty0, const struct rectf* empty1, float scroll_offset, float target, float scroll_step, enum orientation o);
  NK_LIB void draw_scrollbar(struct command_buffer* out, flag state, const struct style_scrollbar* style, const struct rectf* bounds, const struct rectf* scroll);
  NK_LIB float do_scrollbarv(flag* state, struct command_buffer* out, struct rectf scroll, int has_scrolling, float offset, float target, float step, float button_pixel_inc, const struct style_scrollbar* style, struct input* in, const struct user_font* font);
  NK_LIB float do_scrollbarh(flag* state, struct command_buffer* out, struct rectf scroll, int has_scrolling, float offset, float target, float step, float button_pixel_inc, const struct style_scrollbar* style, struct input* in, const struct user_font* font);

  /* selectable */
  NK_LIB void draw_selectable(struct command_buffer* out, flag state, const struct style_selectable* style, bool active, const struct rectf* bounds, const struct rectf* icon, const struct image* img, enum symbol_type sym, const char* string, int len, flag align, const struct user_font* font);
  NK_LIB bool do_selectable(flag* state, struct command_buffer* out, struct rectf bounds, const char* str, int len, flag align, bool* value, const struct style_selectable* style, const struct input* in, const struct user_font* font);
  NK_LIB bool do_selectable_image(flag* state, struct command_buffer* out, struct rectf bounds, const char* str, int len, flag align, bool* value, const struct image* img, const struct style_selectable* style, const struct input* in, const struct user_font* font);

  /* edit */
  NK_LIB void edit_draw_text(struct command_buffer* out, const struct style_edit* style, float pos_x, float pos_y, float x_offset, const char* text, int byte_len, float row_height, const struct user_font* font, struct color background, struct color foreground, bool is_selected);
  NK_LIB flag do_edit(flag* state, struct command_buffer* out, struct rectf bounds, flag flags, plugin_filter filter, struct text_edit* edit, const struct style_edit* style, struct input* in, const struct user_font* font);

  /* color-picker */
  NK_LIB bool color_picker_behavior(flag* state, const struct rectf* bounds, const struct rectf* matrix, const struct rectf* hue_bar, const struct rectf* alpha_bar, struct colorf* color, const struct input* in);
  NK_LIB void draw_color_picker(struct command_buffer* o, const struct rectf* matrix, const struct rectf* hue_bar, const struct rectf* alpha_bar, struct colorf col);
  NK_LIB bool do_color_picker(flag* state, struct command_buffer* out, struct colorf* col, enum color_format fmt, struct rectf bounds, struct vec2f padding, const struct input* in, const struct user_font* font);

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
    enum property_kind kind;
    union property value;
    union property min_value;
    union property max_value;
    union property step;
  };
  NK_LIB struct property_variant property_variant_int(int value, int min_value, int max_value, int step);
  NK_LIB struct property_variant property_variant_float(float value, float min_value, float max_value, float step);
  NK_LIB struct property_variant property_variant_double(double value, double min_value, double max_value, double step);

  NK_LIB void drag_behavior(flag* state, const struct input* in, struct rectf drag, struct property_variant* variant, float inc_per_pixel);
  NK_LIB void property_behavior(flag* ws, const struct input* in, struct rectf property, struct rectf label, struct rectf edit, struct rectf empty, int* state, struct property_variant* variant, float inc_per_pixel);
  NK_LIB void draw_property(struct command_buffer* out, const struct style_property* style, const struct rectf* bounds, const struct rectf* label, flag state, const char* name, int len, const struct user_font* font);
  NK_LIB void do_property(flag* ws, struct command_buffer* out, struct rectf property, const char* name, struct property_variant* variant, float inc_per_pixel, char* buffer, int* len, int* state, int* cursor, int* select_begin, int* select_end, const struct style_property* style, enum property_filter filter, struct input* in, const struct user_font* font, struct text_edit* text_edit, enum btn_behavior behavior);
  NK_LIB void property(struct context* ctx, const char* name, struct property_variant* variant, float inc_per_pixel, const enum property_filter filter);

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
