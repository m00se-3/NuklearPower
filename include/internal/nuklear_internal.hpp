#ifndef INTERNAL_H
#define INTERNAL_H

#include <array>
#include <utility>
#include <cstdint>

namespace nk {

/*
 * ==============================================================
 *
 *                          CONSTANTS
 *
 * ===============================================================
 */

#define NK_UTF_INVALID 0xFFFD /**< internal invalid utf8 rune */
#define NK_UTF_SIZE 4 /**< describes the number of bytes a glyph consists of*/
#ifndef NK_INPUT_MAX
#define NK_INPUT_MAX 16
#endif
#ifndef NK_MAX_NUMBER_BUFFER
#define NK_MAX_NUMBER_BUFFER 64
#endif
#ifndef NK_SCROLLBAR_HIDING_TIMEOUT
#define NK_SCROLLBAR_HIDING_TIMEOUT 4.0f
#endif
/*
 * ==============================================================
 *
 *                          HELPER
 *
 * ===============================================================
 */

#ifndef NK_API
#ifdef NK_PRIVATE
#if (defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199409L))
#define NK_API static inline
#elif defined(__cplusplus)
#define NK_API static inline
#else
#define NK_API static
#endif
#else
#define NK_API extern
#endif
#endif
#ifndef NK_LIB
#ifdef NK_SINGLE_FILE
#define NK_LIB static
#else
#define NK_LIB extern
#endif
#endif

#define INTERN static
#define NK_STORAGE static
#define NK_GLOBAL inline

#define NK_STRINGIFY(x) #x
#define NK_STRING_JOIN(arg1, arg2) arg1##arg2

#ifdef _MSC_VER
#define NK_UNIQUE_NAME(name) NK_STRING_JOIN(name, __COUNTER__)
#else
#define NK_UNIQUE_NAME(name) NK_STRING_JOIN(name, __LINE__)
#endif

#ifndef NK_STATIC_ASSERT
#define NK_STATIC_ASSERT(exp) typedef char NK_UNIQUE_NAME(_dummy_array)[(exp) ? 1 : 0]
#endif

#ifndef NK_FILE_LINE
#ifdef _MSC_VER
#define NK_FILE_LINE __FILE__ ":" NK_STRINGIFY(__COUNTER__)
#else
#define NK_FILE_LINE __FILE__ ":" NK_STRINGIFY(__LINE__)
#endif
#endif

#if __cpp_lib_is_scoped_enum
template<typename T>
constexpr auto operator|(T lhs, T rhs)
  requires std::is_scoped_enum_v<T>
{
  using value_type = std::underlying_type_t<T>;
  return T{static_cast<value_type>(lhs) | static_cast<value_type>(rhs)};
}

template<typename T>
constexpr auto operator&(T lhs, T rhs)
  requires std::is_scoped_enum_v<T>
{
  using value_type = std::underlying_type_t<T>;
  return T{static_cast<value_type>(lhs) & static_cast<value_type>(rhs)};
}

template<typename T>
constexpr auto operator|=(T lhs, T rhs) -> T
  requires std::is_scoped_enum_v<T>
{
  return lhs | rhs;
}

#else
#error "Your compiler does not support C++23's is_scoped_enum"

#endif

#ifdef NK_INCLUDE_STANDARD_VARARGS
#include <stdarg.h>
#if defined(_MSC_VER) && (_MSC_VER >= 1600) /* VS 2010 and above */
#include <sal.h>
#define NK_PRINTF_FORMAT_STRING _Printf_format_string_
#else
#define NK_PRINTF_FORMAT_STRING
#endif
#if defined(__GNUC__)
#define NK_PRINTF_VARARG_FUNC(fmtargnumber) __attribute__((format(__printf__, fmtargnumber, fmtargnumber + 1)))
#define NK_PRINTF_VALIST_FUNC(fmtargnumber) __attribute__((format(__printf__, fmtargnumber, 0)))
#else
#define NK_PRINTF_VARARG_FUNC(fmtargnumber)
#define NK_PRINTF_VALIST_FUNC(fmtargnumber)
#endif
#endif

  /* =============================================================================
   *
   *                                  STYLE
   *
   * ============================================================================= */

#define NK_WIDGET_DISABLED_FACTOR 0.5f

  enum class style_colors {
    COLOR_TEXT,
    COLOR_WINDOW,
    COLOR_HEADER,
    COLOR_BORDER,
    COLOR_BUTTON,
    COLOR_BUTTON_HOVER,
    COLOR_BUTTON_ACTIVE,
    COLOR_TOGGLE,
    COLOR_TOGGLE_HOVER,
    COLOR_TOGGLE_CURSOR,
    COLOR_SELECT,
    COLOR_SELECT_ACTIVE,
    COLOR_SLIDER,
    COLOR_SLIDER_CURSOR,
    COLOR_SLIDER_CURSOR_HOVER,
    COLOR_SLIDER_CURSOR_ACTIVE,
    COLOR_PROPERTY,
    COLOR_EDIT,
    COLOR_EDIT_CURSOR,
    COLOR_COMBO,
    COLOR_CHART,
    COLOR_CHART_COLOR,
    COLOR_CHART_COLOR_HIGHLIGHT,
    COLOR_SCROLLBAR,
    COLOR_SCROLLBAR_CURSOR,
    COLOR_SCROLLBAR_CURSOR_HOVER,
    COLOR_SCROLLBAR_CURSOR_ACTIVE,
    COLOR_TAB_HEADER,
    COLOR_KNOB,
    COLOR_KNOB_CURSOR,
    COLOR_KNOB_CURSOR_HOVER,
    COLOR_KNOB_CURSOR_ACTIVE,
    COLOR_COUNT
  };

  enum class style_cursor {
    CURSOR_ARROW,
    CURSOR_TEXT,
    CURSOR_MOVE,
    CURSOR_RESIZE_VERTICAL,
    CURSOR_RESIZE_HORIZONTAL,
    CURSOR_RESIZE_TOP_LEFT_DOWN_RIGHT,
    CURSOR_RESIZE_TOP_RIGHT_DOWN_LEFT,
    CURSOR_COUNT
  };

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

  /*
     * ===============================================================
     *
     *                          BASIC
     *
     * ===============================================================
     */

  using hash = std::uint32_t;
  using flag = std::uint32_t;
  using rune = std::uint32_t;
  struct color {
    std::uint8_t r, g, b, a;
  };
  struct colorf {
    float r, g, b, a;
  };
  struct vec2f {
    float x, y;
  };
  struct vec2i {
    short x, y;
  };
  struct rectf {
    float x, y, w, h;
  };
  struct recti {
    short x, y, w, h;
  };

  /* ============================================================================
   *
   *                                  API
   *
   * =========================================================================== */


  struct allocator;
  struct command_buffer;
  struct draw_command;
  struct convert_config;
  struct style_item;
  struct text_edit;
  struct draw_list;
  struct user_font;
  struct panel;
  struct context;
  struct draw_vertex_layout_element;
  struct style_button;
  struct style_toggle;
  struct style_selectable;
  struct style_slide;
  struct style_progress;
  struct style_scrollbar;
  struct style_edit;
  struct style_property;
  struct style_chart;
  struct style_combo;
  struct style_tab;
  struct style_window_header;
  struct style_window;


  /* =============================================================================
   *
   *                                  INPUT
   *
   * =============================================================================*/

  enum keys {
    NK_KEY_NONE,
    NK_KEY_SHIFT,
    NK_KEY_CTRL,
    NK_KEY_DEL,
    NK_KEY_ENTER,
    NK_KEY_TAB,
    NK_KEY_BACKSPACE,
    NK_KEY_COPY,
    NK_KEY_CUT,
    NK_KEY_PASTE,
    NK_KEY_UP,
    NK_KEY_DOWN,
    NK_KEY_LEFT,
    NK_KEY_RIGHT,
    /* Shortcuts: text field */
    NK_KEY_TEXT_INSERT_MODE,
    NK_KEY_TEXT_REPLACE_MODE,
    NK_KEY_TEXT_RESET_MODE,
    NK_KEY_TEXT_LINE_START,
    NK_KEY_TEXT_LINE_END,
    NK_KEY_TEXT_START,
    NK_KEY_TEXT_END,
    NK_KEY_TEXT_UNDO,
    NK_KEY_TEXT_REDO,
    NK_KEY_TEXT_SELECT_ALL,
    NK_KEY_TEXT_WORD_LEFT,
    NK_KEY_TEXT_WORD_RIGHT,
    /* Shortcuts: scrollbar */
    NK_KEY_SCROLL_START,
    NK_KEY_SCROLL_END,
    NK_KEY_SCROLL_DOWN,
    NK_KEY_SCROLL_UP,
    NK_KEY_MAX
  };
  enum buttons {
    NK_BUTTON_LEFT,
    NK_BUTTON_MIDDLE,
    NK_BUTTON_RIGHT,
    NK_BUTTON_DOUBLE,
    NK_BUTTON_MAX
  };

  struct mouse_button {
    bool down;
    unsigned int clicked;
    vec2f clicked_pos;
  };
  struct mouse {
    mouse_button buttons[NK_BUTTON_MAX];
    vec2f pos;
#ifdef NK_BUTTON_TRIGGER_ON_RELEASE
    vec2ff down_pos;
#endif
    vec2f prev;
    vec2f delta;
    vec2f scroll_delta;
    unsigned char grab;
    unsigned char grabbed;
    unsigned char ungrab;
  };

  struct key {
    bool down;
    unsigned int clicked;
  };
  struct keyboard {
    key keys[NK_KEY_MAX];
    char text[NK_INPUT_MAX];
    int text_len;
  };

  struct input {
    keyboard keyboard;
    mouse mouse;
  };


  /** =============================================================================
   *
   *                                  DRAWING
   *
   * =============================================================================*/
  enum anti_aliasing { NK_ANTI_ALIASING_OFF,
                       NK_ANTI_ALIASING_ON };
  enum convert_result {
    NK_CONVERT_SUCCESS = 0,
    NK_CONVERT_INVALID_PARAM = 1,
    NK_CONVERT_COMMAND_BUFFER_FULL = (1 << (1)),
    NK_CONVERT_VERTEX_BUFFER_FULL = (1 << (2)),
    NK_CONVERT_ELEMENT_BUFFER_FULL = (1 << (3))
  };

  typedef char glyph[NK_UTF_SIZE];

  typedef union {
    void* ptr;
    int id;
  } resource_handle;

  struct image {
    resource_handle handle;
    unsigned short w, h;
    std::array<unsigned short, 4uz> region;
  };
  struct nine_slice {
    image img;
    unsigned short l, t, r, b;
  };
  struct cursor {
    image img;
    vec2f size, offset;
  };
  struct scroll {
    std::uint32_t x, y;
  };

  struct draw_null_texture {
    resource_handle texture; /**!< texture handle to a texture with a white pixel */
    vec2f uv; /**!< coordinates to a white pixel in the texture  */
  };
  struct convert_config {
    float global_alpha; /**!< global alpha value */
    anti_aliasing line_AA; /**!< line anti-aliasing flag can be turned off if you are tight on memory */
    anti_aliasing shape_AA; /**!< shape anti-aliasing flag can be turned off if you are tight on memory */
    unsigned circle_segment_count; /**!< number of segments used for circles: default to 22 */
    unsigned arc_segment_count; /**!< number of segments used for arcs: default to 22 */
    unsigned curve_segment_count; /**!< number of segments used for curves: default to 22 */
    draw_null_texture tex_null; /**!< handle to texture with a white pixel for shape drawing */
    const draw_vertex_layout_element* vertex_layout; /**!< describes the vertex output format and packing */
    std::size_t vertex_size; /**!< sizeof one vertex for vertex packing */
    std::size_t vertex_alignment; /**!< vertex alignment: Can be obtained by NK_ALIGNOF */
  };

  enum class style_item_type {
    STYLE_ITEM_COLOR,
    STYLE_ITEM_IMAGE,
    STYLE_ITEM_NINE_SLICE
  };

  union style_item_data {
    color color;
    struct image image;
    nine_slice slice;
  };

  struct style_item {
    style_item_type type;
    style_item_data data;
  };

  struct style_text {
    color color;
    vec2f padding;
    float color_factor;
    float disabled_factor;
  };

  struct style_button {
    /* background */
    style_item normal;
    style_item hover;
    style_item active;
    color border_color;
    float color_factor_background;

    /* text */
    color text_background;
    color text_normal;
    color text_hover;
    color text_active;
    flag text_alignment;
    float color_factor_text;

    /* properties */
    float border;
    float rounding;
    vec2f padding;
    vec2f image_padding;
    vec2f touch_padding;
    float disabled_factor;

    /*optional user callbacks*/
    resource_handle userdata;
    void(*draw_begin)(command_buffer*, resource_handle);
    void(*draw_end)(command_buffer*, resource_handle);
  };

    struct memory_status {
      void* memory;
      unsigned int type;
      std::size_t size;
      std::size_t allocated;
      std::size_t needed;
      std::size_t calls;
    };

    typedef void* (*plugin_alloc)(resource_handle, void* old, std::size_t);
    typedef void (*plugin_free)(resource_handle, void* old);
    typedef bool (*plugin_filter)(const text_edit*, rune unicode);
    typedef void (*plugin_paste)(resource_handle, text_edit*);
    typedef void (*plugin_copy)(resource_handle, const char*, int len);

    enum class allocation_type {
      BUFFER_FIXED,
      BUFFER_DYNAMIC
    };

    enum class buffer_allocation_type {
      BUFFER_FRONT,
      BUFFER_BACK,
      BUFFER_MAX
    };

    struct buffer_marker {
      bool active;
      std::size_t offset;
    };

    struct memory_handle {
      void* ptr;
      std::size_t size;
    };


    struct allocator {
      resource_handle userdata;
      plugin_alloc alloc;
      plugin_free free;
    };

    struct memory_buffer {
      std::array<buffer_marker, static_cast<unsigned>(buffer_allocation_type::BUFFER_MAX)> marker; /**!< buffer marker to free a buffer to a certain offset */
      allocator pool; /**!< allocator callback for dynamic buffers */
      allocation_type type; /**!< memory management type */
      memory_handle memory; /**!< memory and size of the current memory block */
      float grow_factor; /**!< growing factor for dynamic memory management */
      std::size_t allocated; /**!< total amount of memory allocated */
      std::size_t needed; /**!< totally consumed memory given that enough memory is present */
      std::size_t calls; /**!< number of allocation calls */
      std::size_t size; /**!< current size of the buffer */
    };
    /*==============================================================
     *                          STACK
     * =============================================================*/
#ifndef NK_BUTTON_BEHAVIOR_STACK_SIZE
#define NK_BUTTON_BEHAVIOR_STACK_SIZE 8
#endif

#ifndef NK_FONT_STACK_SIZE
#define NK_FONT_STACK_SIZE 8
#endif

#ifndef NK_STYLE_ITEM_STACK_SIZE
#define NK_STYLE_ITEM_STACK_SIZE 16
#endif

#ifndef NK_FLOAT_STACK_SIZE
#define NK_FLOAT_STACK_SIZE 32
#endif

#ifndef NK_VECTOR_STACK_SIZE
#define NK_VECTOR_STACK_SIZE 16
#endif

#ifndef NK_FLAGS_STACK_SIZE
#define NK_FLAGS_STACK_SIZE 32
#endif

#ifndef NK_COLOR_STACK_SIZE
#define NK_COLOR_STACK_SIZE 32
#endif

    /* optional user callbacks */
    resource_handle userdata;
    void (*draw_begin)(command_buffer*, resource_handle userdata);
    void (*draw_end)(command_buffer*, resource_handle userdata);

  struct style_toggle {
    /* background */
    style_item normal;
    style_item hover;
    style_item active;
    color border_color;

    /* cursor */
    style_item cursor_normal;
    style_item cursor_hover;

    /* text */
    color text_normal;
    color text_hover;
    color text_active;
    color text_background;
    flag text_alignment;

    /* properties */
    vec2f padding;
    vec2f touch_padding;
    float spacing;
    float border;
    float color_factor;
    float disabled_factor;

    /* optional user callbacks */
    resource_handle userdata;
    void (*draw_begin)(command_buffer*, resource_handle);
    void (*draw_end)(command_buffer*, resource_handle);
  };

  struct style_selectable {
    /* background (inactive) */
    style_item normal;
    style_item hover;
    style_item pressed;

    /* background (active) */
    style_item normal_active;
    style_item hover_active;
    style_item pressed_active;

    /* text color (inactive) */
    color text_normal;
    color text_hover;
    color text_pressed;

    /* text color (active) */
    color text_normal_active;
    color text_hover_active;
    color text_pressed_active;
    color text_background;
    flag text_alignment;

    /* properties */
    float rounding;
    vec2f padding;
    vec2f touch_padding;
    vec2f image_padding;
    float color_factor;
    float disabled_factor;

    /* optional user callbacks */
    resource_handle userdata;
    void (*draw_begin)(command_buffer*, resource_handle);
    void (*draw_end)(command_buffer*, resource_handle);
  };

  enum class symbol_type {
    SYMBOL_NONE,
    SYMBOL_X,
    SYMBOL_UNDERSCORE,
    SYMBOL_CIRCLE_SOLID,
    SYMBOL_CIRCLE_OUTLINE,
    SYMBOL_RECT_SOLID,
    SYMBOL_RECT_OUTLINE,
    SYMBOL_TRIANGLE_UP,
    SYMBOL_TRIANGLE_DOWN,
    SYMBOL_TRIANGLE_LEFT,
    SYMBOL_TRIANGLE_RIGHT,
    SYMBOL_PLUS,
    SYMBOL_MINUS,
    SYMBOL_TRIANGLE_UP_OUTLINE,
    SYMBOL_TRIANGLE_DOWN_OUTLINE,
    SYMBOL_TRIANGLE_LEFT_OUTLINE,
    SYMBOL_TRIANGLE_RIGHT_OUTLINE,
    SYMBOL_MAX
  };

  struct style_slider {
    /* background */
    style_item normal;
    style_item hover;
    style_item active;
    color border_color;

    /* background bar */
    color bar_normal;
    color bar_hover;
    color bar_active;
    color bar_filled;

    /* cursor */
    style_item cursor_normal;
    style_item cursor_hover;
    style_item cursor_active;

    /* properties */
    float border;
    float rounding;
    float bar_height;
    vec2f padding;
    vec2f spacing;
    vec2f cursor_size;
    float color_factor;
    float disabled_factor;

    /* optional buttons */
    int show_buttons;
    style_button inc_button;
    style_button dec_button;
    symbol_type inc_symbol;
    symbol_type dec_symbol;

    /* optional user callbacks */
    resource_handle userdata;
    void (*draw_begin)(command_buffer*, resource_handle);
    void (*draw_end)(command_buffer*, resource_handle);
  };

  struct style_knob {
    /* background */
    style_item normal;
    style_item hover;
    style_item active;
    color border_color;

    /* knob */
    color knob_normal;
    color knob_hover;
    color knob_active;
    color knob_border_color;

    /* cursor */
    color cursor_normal;
    color cursor_hover;
    color cursor_active;

    /* properties */
    float border;
    float knob_border;
    vec2f padding;
    vec2f spacing;
    float cursor_width;
    float color_factor;
    float disabled_factor;

    /* optional user callbacks */
    resource_handle userdata;
    void (*draw_begin)(command_buffer*, resource_handle);
    void (*draw_end)(command_buffer*, resource_handle);
  };

  struct style_progress {
    /* background */
    style_item normal;
    style_item hover;
    style_item active;
    color border_color;

    /* cursor */
    style_item cursor_normal;
    style_item cursor_hover;
    style_item cursor_active;
    color cursor_border_color;

    /* properties */
    float rounding;
    float border;
    float cursor_border;
    float cursor_rounding;
    vec2f padding;
    float color_factor;
    float disabled_factor;

    /* optional user callbacks */
    resource_handle userdata;
    void (*draw_begin)(command_buffer*, resource_handle);
    void (*draw_end)(command_buffer*, resource_handle);
  };

  struct style_scrollbar {
    /* background */
    style_item normal;
    style_item hover;
    style_item active;
    color border_color;

    /* cursor */
    style_item cursor_normal;
    style_item cursor_hover;
    style_item cursor_active;
    color cursor_border_color;

    /* properties */
    float border;
    float rounding;
    float border_cursor;
    float rounding_cursor;
    vec2f padding;
    float color_factor;
    float disabled_factor;

    /* optional buttons */
    int show_buttons;
    style_button inc_button;
    style_button dec_button;
    symbol_type inc_symbol;
    symbol_type dec_symbol;

    /* optional user callbacks */
    resource_handle userdata;
    void (*draw_begin)(command_buffer*, resource_handle);
    void (*draw_end)(command_buffer*, resource_handle);
  };

  struct style_edit {
    /* background */
    style_item normal;
    style_item hover;
    style_item active;
    color border_color;
    style_scrollbar scrollbar;

    /* cursor  */
    color cursor_normal;
    color cursor_hover;
    color cursor_text_normal;
    color cursor_text_hover;

    /* text (unselected) */
    color text_normal;
    color text_hover;
    color text_active;

    /* text (selected) */
    color selected_normal;
    color selected_hover;
    color selected_text_normal;
    color selected_text_hover;

    /* properties */
    float border;
    float rounding;
    float cursor_size;
    vec2f scrollbar_size;
    vec2f padding;
    float row_padding;
    float color_factor;
    float disabled_factor;
  };

  struct style_property {
    /* background */
    style_item normal;
    style_item hover;
    style_item active;
    color border_color;

    /* text */
    color label_normal;
    color label_hover;
    color label_active;

    /* symbols */
    symbol_type sym_left;
    symbol_type sym_right;

    /* properties */
    float border;
    float rounding;
    vec2f padding;
    float color_factor;
    float disabled_factor;

    style_edit edit;
    style_button inc_button;
    style_button dec_button;

    /* optional user callbacks */
    resource_handle userdata;
    void (*draw_begin)(command_buffer*, resource_handle);
    void (*draw_end)(command_buffer*, resource_handle);
  };

  struct style_chart {
    /* colors */
    style_item background;
    color border_color;
    color selected_color;
    color color;

    /* properties */
    float border;
    float rounding;
    vec2f padding;
    float color_factor;
    float disabled_factor;
    bool show_markers;
  };

  struct style_combo {
    /* background */
    style_item normal;
    style_item hover;
    style_item active;
    color border_color;

    /* label */
    color label_normal;
    color label_hover;
    color label_active;

    /* symbol */
    color symbol_normal;
    color symbol_hover;
    color symbol_active;

    /* button */
    style_button button;
    symbol_type sym_normal;
    symbol_type sym_hover;
    symbol_type sym_active;

    /* properties */
    float border;
    float rounding;
    vec2f content_padding;
    vec2f button_padding;
    vec2f spacing;
    float color_factor;
    float disabled_factor;
  };

  struct style_tab {
    /* background */
    style_item background;
    color border_color;
    color text;

    /* button */
    style_button tab_maximize_button;
    style_button tab_minimize_button;
    style_button node_maximize_button;
    style_button node_minimize_button;
    symbol_type sym_minimize;
    symbol_type sym_maximize;

    /* properties */
    float border;
    float rounding;
    float indent;
    vec2f padding;
    vec2f spacing;
    float color_factor;
    float disabled_factor;
  };

  enum class style_header_align {
    HEADER_LEFT,
    HEADER_RIGHT
  };
  struct style_window_header {
    /* background */
    style_item normal;
    style_item hover;
    style_item active;

    /* button */
    style_button close_button;
    style_button minimize_button;
    symbol_type close_symbol;
    symbol_type minimize_symbol;
    symbol_type maximize_symbol;

    /* title */
    color label_normal;
    color label_hover;
    color label_active;

    /* properties */
    style_header_align align;
    vec2f padding;
    vec2f label_padding;
    vec2f spacing;
  };

  struct style_window {
    style_window_header header;
    style_item fixed_background;
    color background;

    color border_color;
    color popup_border_color;
    color combo_border_color;
    color contextual_border_color;
    color menu_border_color;
    color group_border_color;
    color tooltip_border_color;
    style_item scaler;

    float border;
    float combo_border;
    float contextual_border;
    float menu_border;
    float group_border;
    float tooltip_border;
    float popup_border;
    float min_row_height_padding;

    float rounding;
    vec2f spacing;
    vec2f scrollbar_size;
    vec2f min_size;

    vec2f padding;
    vec2f group_padding;
    vec2f popup_padding;
    vec2f combo_padding;
    vec2f contextual_padding;
    vec2f menu_padding;
    vec2f tooltip_padding;
  };

  struct style {
    const user_font* font;
    const cursor* cursors[static_cast<std::size_t>(nk::style_cursor::CURSOR_COUNT)];
    const cursor* cursor_active;
    cursor* cursor_last;
    int cursor_visible;

    style_text text;
    style_button button;
    style_button contextual_button;
    style_button menu_button;
    style_toggle option;
    style_toggle checkbox;
    style_selectable selectable;
    style_slider slider;
    style_knob knob;
    style_progress progress;
    style_property property;
    style_edit edit;
    style_chart chart;
    style_scrollbar scrollh;
    style_scrollbar scrollv;
    style_tab tab;
    style_combo combo;
    style_window window;
  };

  enum class heading { UP,
                       RIGHT,
                       DOWN,
                       LEFT };
  enum class btn_behavior { BUTTON_DEFAULT,
                            BUTTON_REPEATER };
  enum class modify : int8_t { FIXED = 0,
                               MODIFIABLE = 1 };
  enum class orientation { VERTICAL,
                           HORIZONTAL };
  enum class collapse_states : int8_t { MINIMIZED = 0,
                                        MAXIMIZED = 1 };
  enum class show_states : int8_t { HIDDEN = 0,
                                    SHOWN = 1 };
  enum class chart_type { CHART_LINES,
                          CHART_COLUMN,
                          CHART_MAX };
  enum class chart_event { CHART_HOVERING = 0x01,
                           CHART_CLICKED = 0x02 };
  enum class color_format { RGB,
                            RGBA };
  enum class popup_type { POPUP_STATIC,
                          POPUP_DYNAMIC };
  enum class layout_format { DYNAMIC,
                             STATIC };
  enum class tree_type { TREE_NODE,
                         TREE_TAB };


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

  NK_GLOBAL constexpr rectf null_rect = {.x = -8192.0f, .y = -8192.0f, .w = 16384, .h = 16384};
#define NK_FLOAT_PRECISION 0.00000000000001

  NK_GLOBAL constexpr color red = {.r = 255, .g = 0, .b = 0, .a = 255};
  NK_GLOBAL constexpr color green = {.r = 0, .g = 255, .b = 0, .a = 255};
  NK_GLOBAL constexpr color blue = {.r = 0, .g = 0, .b = 255, .a = 255};
  NK_GLOBAL constexpr color white = {.r = 255, .g = 255, .b = 255, .a = 255};
  NK_GLOBAL constexpr color black = {.r = 0, .g = 0, .b = 0, .a = 255};
  NK_GLOBAL constexpr color yellow = {.r = 255, .g = 255, .b = 0, .a = 255};


  struct config_stack_style_item_element {
    style_item* address;
    style_item old_value;
  };
  struct config_stack_float_element {
    float* address;
    float old_value;
  };
  struct config_stack_vec2_element {
    vec2f* address;
    vec2f old_value;
  };
  struct config_stack_flags_element {
    flag* address;
    flag old_value;
  };
  struct config_stack_color_element {
    color* address;
    color old_value;
  };
  struct config_stack_user_font_element {
    const user_font** address;
    const user_font* old_value;
  };
  struct config_stack_button_behavior_element {
    btn_behavior* address;
    btn_behavior old_value;
  };

  struct config_stack_style_item {
    int head;
    std::array<config_stack_style_item_element, 16> elements;
  };
  struct config_stack_float {
    int head;
    std::array<config_stack_float_element, 32> elements;
  };
  struct config_stack_vec2 {
    int head;
    std::array<config_stack_vec2_element, 16> elements;
  };
  struct config_stack_flags {
    int head;
    std::array<config_stack_flags_element, 32> elements;
  };
  struct config_stack_color {
    int head;
    std::array<config_stack_color_element, 32> elements;
  };
  struct config_stack_user_font {
    int head;
    std::array<config_stack_user_font_element, 8> elements;
  };
  struct config_stack_button_behavior {
    int head;
    std::array<config_stack_button_behavior_element, 8> elements;
  };

  struct configuration_stacks {
    config_stack_style_item style_items;
    config_stack_float floats;
    config_stack_vec2 vectors;
    config_stack_flags flags;
    config_stack_color colors;
    config_stack_user_font fonts;
    config_stack_button_behavior button_behaviors;
  };

  enum class command_type {
    COMMAND_NOP,
    COMMAND_SCISSOR,
    COMMAND_LINE,
    COMMAND_CURVE,
    COMMAND_RECT,
    COMMAND_RECT_FILLED,
    COMMAND_RECT_MULTI_COLOR,
    COMMAND_CIRCLE,
    COMMAND_CIRCLE_FILLED,
    COMMAND_ARC,
    COMMAND_ARC_FILLED,
    COMMAND_TRIANGLE,
    COMMAND_TRIANGLE_FILLED,
    COMMAND_POLYGON,
    COMMAND_POLYGON_FILLED,
    COMMAND_POLYLINE,
    COMMAND_TEXT,
    COMMAND_IMAGE,
    COMMAND_CUSTOM
  };

  /** command base and header of every command inside the buffer */
  struct command {
    command_type type;
    std::size_t next;
#ifdef NK_INCLUDE_COMMAND_USERDATA
    resource_handle userdata;
#endif
  };

  struct command_scissor {
    command header;
    short x, y;
    unsigned short w, h;
  };

  struct command_line {
    command header;
    unsigned short line_thickness;
    vec2i begin;
    vec2i end;
    color color;
  };

  struct command_curve {
    command header;
    unsigned short line_thickness;
    vec2i begin;
    vec2i end;
    vec2i ctrl[2];
    color color;
  };

  struct command_rect {
    command header;
    unsigned short rounding;
    unsigned short line_thickness;
    short x, y;
    unsigned short w, h;
    color color;
  };

  struct command_rect_filled {
    command header;
    unsigned short rounding;
    short x, y;
    unsigned short w, h;
    color color;
  };

  struct command_rect_multi_color {
    command header;
    short x, y;
    unsigned short w, h;
    color left;
    color top;
    color bottom;
    color right;
  };

  struct command_triangle {
    command header;
    unsigned short line_thickness;
    vec2i a;
    vec2i b;
    vec2i c;
    color color;
  };

  struct command_triangle_filled {
    command header;
    vec2i a;
    vec2i b;
    vec2i c;
    color color;
  };

  struct command_circle {
    command header;
    short x, y;
    unsigned short line_thickness;
    unsigned short w, h;
    color color;
  };

  struct command_circle_filled {
    command header;
    short x, y;
    unsigned short w, h;
    color color;
  };

  struct command_arc {
    command header;
    short cx, cy;
    unsigned short r;
    unsigned short line_thickness;
    float a[2];
    color color;
  };

  struct command_arc_filled {
    command header;
    short cx, cy;
    unsigned short r;
    float a[2];
    color color;
  };

  struct command_polygon {
    command header;
    color color;
    unsigned short line_thickness;
    unsigned short point_count;
    vec2i points[1];
  };

  struct command_polygon_filled {
    command header;
    color color;
    unsigned short point_count;
    vec2i points[1];
  };

  struct command_polyline {
    command header;
    color color;
    unsigned short line_thickness;
    unsigned short point_count;
    vec2i points[1];
  };

  struct command_image {
    command header;
    short x, y;
    unsigned short w, h;
    struct image img;
    color col;
  };

  typedef void (*command_custom_callback)(void* canvas, short x, short y,
                                          unsigned short w, unsigned short h, resource_handle callback_data);
  struct command_custom {
    command header;
    short x, y;
    unsigned short w, h;
    resource_handle callback_data;
    command_custom_callback callback;
  };

  struct command_text {
    command header;
    const user_font* font;
    color background;
    color foreground;
    short x, y;
    unsigned short w, h;
    float height;
    int length;
    char string[2];
  };

  enum command_clipping {
    NK_CLIPPING_OFF = 0,
    NK_CLIPPING_ON = 1
  };

  struct command_buffer {
    memory_buffer* base;
    rectf clip;
    int use_clipping;
    resource_handle userdata;
    std::size_t begin, end, last;
  };

  namespace panel_flags {
    using value_type = unsigned int;
    constexpr value_type WINDOW_BORDER = 1 << (0);
    constexpr value_type WINDOW_MOVABLE = 1 << (1);
    constexpr value_type WINDOW_SCALABLE = 1 << (2);
    constexpr value_type WINDOW_CLOSABLE = 1 << (3);
    constexpr value_type WINDOW_MINIMIZABLE = 1 << (4);
    constexpr value_type WINDOW_NO_SCROLLBAR = 1 << (5);
    constexpr value_type WINDOW_TITLE = 1 << (6);
    constexpr value_type WINDOW_SCROLL_AUTO_HIDE = 1 << (7);
    constexpr value_type WINDOW_BACKGROUND = 1 << (8);
    constexpr value_type WINDOW_SCALE_LEFT = 1 << (9);
    constexpr value_type WINDOW_NO_INPUT = 1 << (10);
  } // namespace panel_flags

  /*==============================================================
   *                          PANEL
   * =============================================================*/
#ifndef NK_MAX_LAYOUT_ROW_TEMPLATE_COLUMNS
#define NK_MAX_LAYOUT_ROW_TEMPLATE_COLUMNS 16
#endif
#ifndef NK_CHART_MAX_SLOT
#define NK_CHART_MAX_SLOT 4
#endif

  namespace panel_type {
    using value_type = int;
    constexpr value_type PANEL_NONE = 0;
    constexpr value_type PANEL_WINDOW = (1 << (0));
    constexpr value_type PANEL_GROUP = (1 << (1));
    constexpr value_type PANEL_POPUP = (1 << (2));
    constexpr value_type PANEL_CONTEXTUAL = (1 << (4));
    constexpr value_type PANEL_COMBO = (1 << (5));
    constexpr value_type PANEL_MENU = (1 << (6));
    constexpr value_type PANEL_TOOLTIP = (1 << (7));
  } // namespace panel_type

  enum class panel_set {
    PANEL_SET_NONBLOCK = panel_type::PANEL_CONTEXTUAL | panel_type::PANEL_COMBO | panel_type::PANEL_MENU | panel_type::PANEL_TOOLTIP,
    PANEL_SET_POPUP = PANEL_SET_NONBLOCK | panel_type::PANEL_POPUP,
    PANEL_SET_SUB = panel_type::PANEL_GROUP | PANEL_SET_POPUP
  };
  enum class panel_row_layout_type {
    LAYOUT_DYNAMIC_FIXED = 0,
    LAYOUT_DYNAMIC_ROW,
    LAYOUT_DYNAMIC_FREE,
    LAYOUT_DYNAMIC,
    LAYOUT_STATIC_FIXED,
    LAYOUT_STATIC_ROW,
    LAYOUT_STATIC_FREE,
    LAYOUT_STATIC,
    LAYOUT_TEMPLATE,
    LAYOUT_COUNT
  };
  struct row_layout {
    panel_row_layout_type type;
    int index;
    float height;
    float min_height;
    int columns;
    const float* ratio;
    float item_width;
    float item_height;
    float item_offset;
    float filled;
    rectf item;
    int tree_depth;
    float templates[NK_MAX_LAYOUT_ROW_TEMPLATE_COLUMNS];
  };

  struct popup_buffer {
    std::size_t begin;
    std::size_t parent;
    std::size_t last;
    std::size_t end;
    bool active;
  };

  struct menu_state {
    float x, y, w, h;
    scroll offset;
  };

  struct chart_slot {
    chart_type type;
    color color;
    struct color highlight;
    float min, max, range;
    int count;
    vec2f last;
    int index;
    bool show_markers;
  };

  struct chart {
    int slot;
    float x, y, w, h;
    chart_slot slots[NK_CHART_MAX_SLOT];
  };

  /* =============================================================================
   *
   *                                  LIST VIEW
   *
   * ============================================================================= */
  struct list_view {
    /* public: */
    int begin, end, count;
    /* private: */
    int total_height;
    context* ctx;
    std::uint32_t* scroll_pointer;
    std::uint32_t scroll_value;
  };

  /*==============================================================
   *                          WINDOW
   * =============================================================*/
#ifndef NK_WINDOW_MAX_NAME
#define NK_WINDOW_MAX_NAME 64
#endif

  struct table;
  namespace window_flags {
    using value_type = unsigned int;
    constexpr value_type WINDOW_PRIVATE = (1 << (11));
    constexpr value_type WINDOW_DYNAMIC = WINDOW_PRIVATE; /**< special window type growing up in height while being filled to a certain maximum height */
    constexpr value_type WINDOW_ROM = (1 << (12)); /**< sets window widgets into a read only mode and does not allow input changes */
    constexpr value_type WINDOW_NOT_INTERACTIVE = WINDOW_ROM | panel_flags::WINDOW_NO_INPUT; /**< prevents all interaction caused by input to either window or widgets inside */
    constexpr value_type WINDOW_HIDDEN = (1 << (13)); /**< Hides window and stops any window interaction and drawing */
    constexpr value_type WINDOW_CLOSED = (1 << (14)); /**< Directly closes and frees the window at the end of the frame */
    constexpr value_type WINDOW_MINIMIZED = (1 << (15)); /**< marks the window as minimized */
    constexpr value_type WINDOW_REMOVE_ROM = (1 << (16)); /**< Removes read only mode at the end of the window */
  }; // namespace window_flags

  /* =============================================================================
   *
   *                                  TEXT
   *
   * ============================================================================= */
  enum text_align {
    NK_TEXT_ALIGN_LEFT = 0x01,
    NK_TEXT_ALIGN_CENTERED = 0x02,
    NK_TEXT_ALIGN_RIGHT = 0x04,
    NK_TEXT_ALIGN_TOP = 0x08,
    NK_TEXT_ALIGN_MIDDLE = 0x10,
    NK_TEXT_ALIGN_BOTTOM = 0x20
  };
  enum text_alignment {
    NK_TEXT_LEFT = NK_TEXT_ALIGN_MIDDLE | NK_TEXT_ALIGN_LEFT,
    NK_TEXT_CENTERED = NK_TEXT_ALIGN_MIDDLE | NK_TEXT_ALIGN_CENTERED,
    NK_TEXT_RIGHT = NK_TEXT_ALIGN_MIDDLE | NK_TEXT_ALIGN_RIGHT
  };

  /* =============================================================================
   *
   *                                  WIDGET
   *
   * ============================================================================= */
  enum widget_layout_states {
    NK_WIDGET_INVALID, /**< The widget cannot be seen and is completely out of view */
    NK_WIDGET_VALID, /**< The widget is completely inside the window and can be updated and drawn */
    NK_WIDGET_ROM, /**< The widget is partially visible and cannot be updated */
    NK_WIDGET_DISABLED /**< The widget is manually disabled and acts like NK_WIDGET_ROM */
  };
  enum widget_states {
    NK_WIDGET_STATE_MODIFIED = (1 << (1)),
    NK_WIDGET_STATE_INACTIVE = (1 << (2)), /**!< widget is neither active nor hovered */
    NK_WIDGET_STATE_ENTERED = (1 << (3)), /**!< widget has been hovered on the current frame */
    NK_WIDGET_STATE_HOVER = (1 << (4)), /**!< widget is being hovered */
    NK_WIDGET_STATE_ACTIVED = (1 << (5)), /**!< widget is currently activated */
    NK_WIDGET_STATE_LEFT = (1 << (6)), /**!< widget is from this frame on not hovered anymore */
    NK_WIDGET_STATE_HOVERED = NK_WIDGET_STATE_HOVER | NK_WIDGET_STATE_MODIFIED, /**!< widget is being hovered */
    NK_WIDGET_STATE_ACTIVE = NK_WIDGET_STATE_ACTIVED | NK_WIDGET_STATE_MODIFIED /**!< widget is currently activated */
  };

  /* widget */
  constexpr auto widget_state_reset = [](auto* state) {
    *state = NK_WIDGET_STATE_INACTIVE;
    if ((*state & NK_WIDGET_STATE_MODIFIED) == 1u) {
      *state |= NK_WIDGET_STATE_MODIFIED;
    }
  };

  /* =============================================================================
   *
   *                                  TEXT EDIT
   *
   * ============================================================================= */
  namespace edit_flags {
    constexpr unsigned EDIT_DEFAULT = 0;
    constexpr unsigned EDIT_READ_ONLY = 1 << 0;
    constexpr unsigned EDIT_AUTO_SELECT = 1 << 1;
    constexpr unsigned EDIT_SIG_ENTER = 1 << 2;
    constexpr unsigned EDIT_ALLOW_TAB = 1 << 3;
    constexpr unsigned EDIT_NO_CURSOR = 1 << 4;
    constexpr unsigned EDIT_SELECTABLE = 1 << 5;
    constexpr unsigned EDIT_CLIPBOARD = 1 << 6;
    constexpr unsigned EDIT_CTRL_ENTER_NEWLINE = 1 << 7;
    constexpr unsigned EDIT_NO_HORIZONTAL_SCROLL = 1 << 8;
    constexpr unsigned EDIT_ALWAYS_INSERT_MODE = 1 << 9;
    constexpr unsigned EDIT_MULTILINE = 1 << 9;
    constexpr unsigned EDIT_GOTO_END_ON_ACTIVATE = 1 << 11;
  } // namespace edit_flags

  enum class edit_types : std::uint32_t {
    EDIT_SIMPLE = edit_flags::EDIT_ALWAYS_INSERT_MODE,
    EDIT_FIELD = EDIT_SIMPLE | edit_flags::EDIT_SELECTABLE | edit_flags::EDIT_CLIPBOARD,
    EDIT_BOX = edit_flags::EDIT_ALWAYS_INSERT_MODE | edit_flags::EDIT_SELECTABLE | edit_flags::EDIT_MULTILINE | edit_flags::EDIT_ALLOW_TAB | edit_flags::EDIT_CLIPBOARD,
    EDIT_EDITOR = edit_flags::EDIT_SELECTABLE | edit_flags::EDIT_MULTILINE | edit_flags::EDIT_ALLOW_TAB | edit_flags::EDIT_CLIPBOARD
  };
  enum class edit_events {
    EDIT_ACTIVE = (1 << (0)), /**!< edit widget is currently being modified */
    EDIT_INACTIVE = (1 << (1)), /**!< edit widget is not active and is not being modified */
    EDIT_ACTIVATED = (1 << (2)), /**!< edit widget went from state inactive to state active */
    EDIT_DEACTIVATED = (1 << (3)), /**!< edit widget went from state active to state inactive */
    EDIT_COMMITED = (1 << (4)) /**!< edit widget has received an enter and lost focus */
  };

#ifndef NK_TEXTEDIT_UNDOSTATECOUNT
#define NK_TEXTEDIT_UNDOSTATECOUNT 99
#endif

#ifndef NK_TEXTEDIT_UNDOCHARCOUNT
#define NK_TEXTEDIT_UNDOCHARCOUNT 999
#endif

  /** ==============================================================
   *
   *                          STRING
   *
   * ===============================================================*/
  /**  Basic string buffer which is only used in context with the text editor
   *  to manage and manipulate dynamic or fixed size string content. This is _NOT_
   *  the default string handling method. The only instance you should have any contact
   *  with this API is if you interact with an `text_edit` object inside one of the
   *  copy and paste functions and even there only for more advanced cases. */
  struct str {
    memory_buffer buffer;
    int len; /**!< in codepoints/runes/glyphs */
  };

  struct text_edit;
  struct clipboard {
    resource_handle userdata;
    plugin_paste paste;
    plugin_copy copy;
  };

  struct text_undo_record {
    int where;
    short insert_length;
    short delete_length;
    short char_storage;
  };

  struct text_undo_state {
    std::array<text_undo_record, NK_TEXTEDIT_UNDOSTATECOUNT> undo_rec;
    std::array<rune, NK_TEXTEDIT_UNDOCHARCOUNT> undo_char;
    short undo_point;
    short redo_point;
    short undo_char_point;
    short redo_char_point;
  };

  enum class text_edit_type {
    TEXT_EDIT_SINGLE_LINE,
    TEXT_EDIT_MULTI_LINE
  };

  enum class text_edit_mode {
    TEXT_EDIT_MODE_VIEW,
    TEXT_EDIT_MODE_INSERT,
    TEXT_EDIT_MODE_REPLACE
  };

  struct text_edit {
    clipboard clip;
    str string;
    plugin_filter filter;
    vec2f scrollbar;

    int cursor;
    int select_start;
    int select_end;
    unsigned char mode;
    unsigned char cursor_at_end_of_line;
    unsigned char initialized;
    unsigned char has_preferred_x;
    unsigned char single_line;
    unsigned char active;
    unsigned char padding1;
    float preferred_x;
    text_undo_state undo;
  };

  struct window;

  struct panel {
    panel_type::value_type type;
    flag flags;
    rectf bounds;
    std::uint32_t* offset_x;
    std::uint32_t* offset_y;
    float at_x, at_y, max_x;
    float footer_height;
    float header_height;
    float border;
    unsigned int has_scrolling;
    rectf clip;
    menu_state menu;
    row_layout row;
    chart chart;
    command_buffer* buffer;
    panel* parent;
  };

  struct popup_state {
    window* win;
    panel_type::value_type type;
    popup_buffer buf;
    hash name;
    bool active;
    unsigned combo_count;
    unsigned con_count, con_old;
    unsigned active_con;
    rectf header;
  };

  struct edit_state {
    hash name;
    unsigned int seq;
    unsigned int old;
    int active, prev;
    int cursor;
    int sel_start;
    int sel_end;
    scroll scrollbar;
    unsigned char mode;
    unsigned char single_line;
  };

  struct property_state {
    int active, prev;
    char buffer[NK_MAX_NUMBER_BUFFER];
    int length;
    int cursor;
    int select_start;
    int select_end;
    hash name;
    unsigned int seq;
    unsigned int old;
    int state;
  };

  enum widget_align {
    NK_WIDGET_ALIGN_LEFT = 0x01,
    NK_WIDGET_ALIGN_CENTERED = 0x02,
    NK_WIDGET_ALIGN_RIGHT = 0x04,
    NK_WIDGET_ALIGN_TOP = 0x08,
    NK_WIDGET_ALIGN_MIDDLE = 0x10,
    NK_WIDGET_ALIGN_BOTTOM = 0x20
  };
  enum widget_alignment {
    NK_WIDGET_LEFT = NK_WIDGET_ALIGN_MIDDLE | NK_WIDGET_ALIGN_LEFT,
    NK_WIDGET_CENTERED = NK_WIDGET_ALIGN_MIDDLE | NK_WIDGET_ALIGN_CENTERED,
    NK_WIDGET_RIGHT = NK_WIDGET_ALIGN_MIDDLE | NK_WIDGET_ALIGN_RIGHT
  };

  struct window {
    unsigned int seq;
    hash name;
    char name_string[NK_WINDOW_MAX_NAME];
    flag flags;

    rectf bounds;
    scroll scrollbar;
    command_buffer buffer;
    panel* layout;
    float scrollbar_hiding_timer;

    /* persistent widget state */
    property_state property;
    popup_state popup;
    edit_state edit;
    unsigned int scrolled;
    bool widgets_disabled;

    table* tables;
    unsigned int table_count;

    /* window list hooks */
    window* next;
    window* prev;
    window* parent;
  };

  /*==============================================================
   *                          CONTEXT
   * =============================================================*/
#define NK_VALUE_PAGE_CAPACITY \
(((std::max(sizeof(struct window), sizeof(struct panel)) / sizeof(std::uint32_t))) / 2)

  struct table {
    unsigned int seq;
    unsigned int size;
    hash keys[NK_VALUE_PAGE_CAPACITY];
    std::uint32_t values[NK_VALUE_PAGE_CAPACITY];
    table *next, *prev;
  };

  union page_data {
    table tbl;
    panel pan;
    window win;
  };

  struct page_element {
    page_data data;
    page_element* next;
    page_element* prev;
  };

  struct page {
    unsigned int size;
    page* next;
    page_element win[1];
  };

  struct pool {
    allocator alloc;
    allocation_type type;
    unsigned int page_count;
    page* pages;
    page_element* freelist;
    unsigned capacity;
    std::size_t size;
    std::size_t cap;
  };


  /* util */
  enum { NK_DO_NOT_STOP_ON_NEW_LINE,
         NK_STOP_ON_NEW_LINE };
  /* window */
  enum window_insert_location {
    NK_INSERT_BACK, /* inserts window into the back of list (front of screen) */
    NK_INSERT_FRONT /* inserts window into the front of list (back of screen) */
  };
  /* text */
  struct text {
    vec2f padding;
    color background;
    color txt;
  };
  /* toggle */
  enum toggle_type {
    NK_TOGGLE_CHECK,
    NK_TOGGLE_OPTION
  };
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

  /* ===============================================================
   *
   *                          FONT
   *
   * ===============================================================*/
  /**
   * \page Font
   * Font handling in this library was designed to be quite customizable and lets
   * you decide what you want to use and what you want to provide. There are three
   * different ways to use the font atlas. The first two will use your font
   * handling scheme and only requires essential data to run nuklear. The next
   * slightly more advanced features is font handling with vertex buffer output.
   * Finally the most complex API wise is using nuklear's font baking API.
   *
   * # Using your own implementation without vertex buffer output
   *
   * So first up the easiest way to do font handling is by just providing a
   * `user_font` struct which only requires the height in pixel of the used
   * font and a callback to calculate the width of a string. This way of handling
   * fonts is best fitted for using the normal draw shape command API where you
   * do all the text drawing yourself and the library does not require any kind
   * of deeper knowledge about which font handling mechanism you use.
   * IMPORTANT: the `user_font` pointer provided to nuklear has to persist
   * over the complete life time! I know this sucks but it is currently the only
   * way to switch between fonts.
   *
   * ```c
   *     float your_text_width_calculation(handle handle, float height, const char *text, int len)
   *     {
   *         your_font_type *type = handle.ptr;
   *         float text_width = ...;
   *         return text_width;
   *     }
   *
   *     struct user_font font;
   *     font.userdata.ptr = &your_font_class_or_struct;
   *     font.height = your_font_height;
   *     font.width = your_text_width_calculation;
   *
   *     struct context ctx;
   *     init_default(&ctx, &font);
   * ```
   * # Using your own implementation with vertex buffer output
   *
   * While the first approach works fine if you don't want to use the optional
   * vertex buffer output it is not enough if you do. To get font handling working
   * for these cases you have to provide two additional parameters inside the
   * `user_font`. First a texture atlas handle used to draw text as subimages
   * of a bigger font atlas texture and a callback to query a character's glyph
   * information (offset, size, ...). So it is still possible to provide your own
   * font and use the vertex buffer output.
   *
   * ```c
   *     float your_text_width_calculation(handle handle, float height, const char *text, int len)
   *     {
   *         your_font_type *type = handle.ptr;
   *         float text_width = ...;
   *         return text_width;
   *     }
   *     void query_your_font_glyph(handle handle, float font_height, struct user_font_glyph *glyph, rune codepoint, rune next_codepoint)
   *     {
   *         your_font_type *type = handle.ptr;
   *         glyph.width = ...;
   *         glyph.height = ...;
   *         glyph.xadvance = ...;
   *         glyph.uv[0].x = ...;
   *         glyph.uv[0].y = ...;
   *         glyph.uv[1].x = ...;
   *         glyph.uv[1].y = ...;
   *         glyph.offset.x = ...;
   *         glyph.offset.y = ...;
   *     }
   *
   *     struct user_font font;
   *     font.userdata.ptr = &your_font_class_or_struct;
   *     font.height = your_font_height;
   *     font.width = your_text_width_calculation;
   *     font.query = query_your_font_glyph;
   *     font.texture.id = your_font_texture;
   *
   *     struct context ctx;
   *     init_default(&ctx, &font);
   * ```
   *
   * # Nuklear font baker
   *
   * The final approach if you do not have a font handling functionality or don't
   * want to use it in this library is by using the optional font baker.
   * The font baker APIs can be used to create a font plus font atlas texture
   * and can be used with or without the vertex buffer output.
   *
   * It still uses the `user_font` struct and the two different approaches
   * previously stated still work. The font baker is not located inside
   * `context` like all other systems since it can be understood as more of
   * an extension to nuklear and does not really depend on any `context` state.
   *
   * Font baker need to be initialized first by one of the font_atlas_init_xxx
   * functions. If you don't care about memory just call the default version
   * `font_atlas_init_default` which will allocate all memory from the standard library.
   * If you want to control memory allocation but you don't care if the allocated
   * memory is temporary and therefore can be freed directly after the baking process
   * is over or permanent you can call `font_atlas_init`.
   *
   * After successfully initializing the font baker you can add Truetype(.ttf) fonts from
   * different sources like memory or from file by calling one of the `font_atlas_add_xxx`.
   * functions. Adding font will permanently store each font, font config and ttf memory block(!)
   * inside the font atlas and allows to reuse the font atlas. If you don't want to reuse
   * the font baker by for example adding additional fonts you can call
   * `font_atlas_cleanup` after the baking process is over (after calling font_atlas_end).
   *
   * As soon as you added all fonts you wanted you can now start the baking process
   * for every selected glyph to image by calling `font_atlas_bake`.
   * The baking process returns image memory, width and height which can be used to
   * either create your own image object or upload it to any graphics library.
   * No matter which case you finally have to call `font_atlas_end` which
   * will free all temporary memory including the font atlas image so make sure
   * you created our texture beforehand. `font_atlas_end` requires a handle
   * to your font texture or object and optionally fills a `struct draw_null_texture`
   * which can be used for the optional vertex output. If you don't want it just
   * set the argument to `NULL`.
   *
   * At this point you are done and if you don't want to reuse the font atlas you
   * can call `font_atlas_cleanup` to free all truetype blobs and configuration
   * memory. Finally if you don't use the font atlas and any of it's fonts anymore
   * you need to call `font_atlas_clear` to free all memory still being used.
   *
   * ```c
   *     struct font_atlas atlas;
   *     font_atlas_init_default(&atlas);
   *     font_atlas_begin(&atlas);
   *     font *font = font_atlas_add_from_file(&atlas, "Path/To/Your/TTF_Font.ttf", 13, 0);
   *     font *font2 = font_atlas_add_from_file(&atlas, "Path/To/Your/TTF_Font2.ttf", 16, 0);
   *     const void* img = font_atlas_bake(&atlas, &img_width, &img_height, NK_FONT_ATLAS_RGBA32);
   *     font_atlas_end(&atlas, handle_id(texture), 0);
   *
   *     struct context ctx;
   *     init_default(&ctx, &font->handle);
   *     while (1) {
   *
   *     }
   *     font_atlas_clear(&atlas);
   * ```
   * The font baker API is probably the most complex API inside this library and
   * I would suggest reading some of my examples `example/` to get a grip on how
   * to use the font atlas. There are a number of details I left out. For example
   * how to merge fonts, configure a font with `font_configure` to use other languages,
   * use another texture coordinate format and a lot more:
   *
   * ```c
   *     font_config cfg = font_configure(font_pixel_height);
   *     cfg.merge_mode = false or true;
   *     cfg.range = font_korean_glyph_ranges();
   *     cfg.coord_type = NK_COORD_PIXEL;
   *     font *font = font_atlas_add_from_file(&atlas, "Path/To/Your/TTF_Font.ttf", 13, &cfg);
   * ```
   */

  struct user_font_glyph;
  typedef float (*text_width_f)(resource_handle, float h, const char*, int len);
  typedef void (*query_font_glyph_f)(resource_handle handle, float font_height,
                                     user_font_glyph* glyph,
                                     rune codepoint, rune next_codepoint);

#if defined(NK_INCLUDE_VERTEX_BUFFER_OUTPUT) || defined(NK_INCLUDE_SOFTWARE_FONT)
  struct draw_list {
    rectf clip_rect;
    vec2f circle_vtx[12];
    struct convert_config config;

    memory_buffer* buffer;
    memory_buffer* vertices;
    memory_buffer* elements;

    unsigned int element_count;
    unsigned int vertex_count;
    unsigned int cmd_count;
    std::size_t cmd_offset;

    unsigned int path_count;
    unsigned int path_offset;

    enum anti_aliasing line_AA;
    enum anti_aliasing shape_AA;
    query_font_glyph_f query; /**!< font glyph callback to query drawing info */
    resource_handle texture; /**!< texture handle to the used font atlas or texture */
#ifdef NK_INCLUDE_COMMAND_USERDATA
    resource_handle userdata;
#endif
  };

  struct user_font_glyph {
    vec2f uv[2]; /**!< texture coordinates */
    vec2f offset; /**!< offset between top left and glyph */
    float width, height; /**!< size of the glyph  */
    float xadvance; /**!< offset to the next glyph */
  };
#endif

  struct context {
    /* public: can be accessed freely */
    input input;
    style style;
    memory_buffer memory;
    clipboard clip;
    flag last_widget_state;
    btn_behavior button_behavior;
    configuration_stacks stacks;
    float delta_time_seconds;
    /* private:
        should only be accessed if you
        know what you are doing */
#ifdef NK_INCLUDE_VERTEX_BUFFER_OUTPUT
    struct draw_list draw_list;
#endif
#ifdef NK_INCLUDE_COMMAND_USERDATA
    resource_handle userdata;
#endif
    /** text editor objects are quite big because of an internal
     * undo/redo stack. Therefore, it does not make sense to have one for
     * each window for temporary use cases, so I only provide *one* instance
     * for all windows. This works because the content is cleared anyway */
    text_edit text_edit;
    /** draw buffer used for overlay drawing operation like cursor */
    command_buffer overlay;

    /** windows */
    int build;
    int use_pool;
    pool pool;
    window* begin;
    window* end;
    window* active;
    window* current;
    page_element* freelist;
    unsigned int count;
    unsigned int seq;
  };

  struct user_font {
    resource_handle userdata; /**!< user provided font handle */
    float height; /**!< max height of the font */
    text_width_f width; /**!< font string width in pixel callback */
#ifdef NK_INCLUDE_COMMAND_USERDATA
    query_font_glyph_f query;
    resource_handle texture;
#endif
    };
#ifdef NK_INCLUDE_VERTEX_BUFFER_OUTPUT

    struct draw_vertex_layout_element {
      enum draw_vertex_layout_attribute attribute;
      enum draw_vertex_layout_format format;
      std::size_t offset;
    };

    struct draw_command {
      unsigned int elem_count; /**< number of elements in the current draw batch */
      rectf clip_rect; /**< current screen clipping rectangle */
      resource_handle texture; /**< current texture to set */

#ifdef NK_INCLUDE_COMMAND_USERDATA
      resource_handle userdata;
#endif
    };

#endif

#ifdef NK_INCLUDE_FONT_BAKING
  enum font_coord_type {
    NK_COORD_UV, /**!< texture coordinates inside font glyphs are clamped between 0-1 */
    NK_COORD_PIXEL /**!< texture coordinates inside font glyphs are in absolute pixel */
  };

  struct font;
  struct baked_font {
    float height; /**!< height of the font  */
    float ascent; /**!< font glyphs ascent and descent  */
    float descent; /**!< font glyphs ascent and descent  */
    rune glyph_offset; /**!< glyph array offset inside the font glyph baking output array  */
    rune glyph_count; /**!< number of glyphs of this font inside the glyph baking array output */
    const rune* ranges; /**!< font codepoint ranges as pairs of (from/to) and 0 as last element */
  };

  struct font_config {
    font_config* next; /**!< NOTE: only used internally */
    void* ttf_blob; /**!< pointer to loaded TTF file memory block.  * \note not needed for font_atlas_add_from_memory and font_atlas_add_from_file. */
    std::size_t ttf_size; /**!< size of the loaded TTF file memory block * \note not needed for font_atlas_add_from_memory and font_atlas_add_from_file. */

    unsigned char ttf_data_owned_by_atlas; /**!< used inside font atlas: default to: 0*/
    unsigned char merge_mode; /**!< merges this font into the last font */
    unsigned char pixel_snap; /**!< align every character to pixel boundary (if true set oversample (1,1)) */
    unsigned char oversample_v, oversample_h; /**!< rasterize at high quality for sub-pixel position */
    unsigned char padding[3];

    float size; /**!< baked pixel height of the font */
    font_coord_type coord_type; /**!< texture coordinate format with either pixel or UV coordinates */
    vec2f spacing; /**!< extra pixel spacing between glyphs  */
    const rune* range; /**!< list of unicode ranges (2 values per range, zero terminated) */
    baked_font* font; /**!< font to setup in the baking process: NOTE: not needed for font atlas */
    rune fallback_glyph; /**!< fallback glyph to use if a given rune is not found */
    font_config* n;
    font_config* p;
  };

  struct font_glyph {
    rune codepoint;
    float xadvance;
    float x0, y0, x1, y1, w, h;
    float u0, v0, u1, v1;
  };

  struct font {
    struct font* next;
    struct user_font handle;
    struct baked_font info;
    float scale;
    struct font_glyph* glyphs;
    const struct font_glyph* fallback;
    rune fallback_codepoint;
    resource_handle texture;
    font_config* config;
  };

  enum font_atlas_format {
    NK_FONT_ATLAS_ALPHA8,
    NK_FONT_ATLAS_RGBA32
  };

  struct font_atlas {
    void* pixel;
    int tex_width;
    int tex_height;

    allocator permanent;
    allocator temporary;

    recti custom;
    cursor cursors[static_cast<unsigned>(style_cursor::CURSOR_COUNT)];

    int glyph_count;
    font_glyph* glyphs;
    font* default_font;
    font* fonts;
    font_config* config;
    int font_num;
  };

  /** some language glyph codepoint ranges */
  NK_API const rune* font_default_glyph_ranges(void);
  NK_API const rune* font_chinese_glyph_ranges(void);
  NK_API const rune* font_cyrillic_glyph_ranges(void);
  NK_API const rune* font_korean_glyph_ranges(void);

#ifdef NK_INCLUDE_DEFAULT_ALLOCATOR
  NK_API void font_atlas_init_default(struct font_atlas*);
#endif
  NK_API void font_atlas_init(struct font_atlas*, const struct allocator*);
  NK_API void font_atlas_init_custom(struct font_atlas*, const struct allocator* persistent, const struct allocator* transient);
  NK_API void font_atlas_begin(struct font_atlas*);
  NK_API struct font_config font_configure(float pixel_height);
  NK_API struct font* font_atlas_add(struct font_atlas*, const font_config*);
#ifdef NK_INCLUDE_DEFAULT_FONT
  NK_API struct font* font_atlas_add_default(struct font_atlas*, float height, const struct font_config*);
#endif
  NK_API struct font* font_atlas_add_from_memory(struct font_atlas* atlas, void* memory, std::size_t size, float height, const font_config* config);
#ifdef NK_INCLUDE_STANDARD_IO
  NK_API struct font* font_atlas_add_from_file(struct font_atlas* atlas, const char* file_path, float height, const struct font_config*);
#endif
  NK_API struct font* font_atlas_add_compressed(struct font_atlas*, void* memory, std::size_t size, float height, const font_config*);
  NK_API struct font* font_atlas_add_compressed_base85(struct font_atlas*, const char* data, float height, const font_config* config);
  NK_API const void* font_atlas_bake(struct font_atlas*, int* width, int* height, enum font_atlas_format);
  NK_API void font_atlas_end(struct font_atlas*, resource_handle tex, struct draw_null_texture*);
  NK_API const struct font_glyph* font_find_glyph(const struct font*, rune unicode);
  NK_API void font_atlas_cleanup(struct font_atlas* atlas);
  NK_API void font_atlas_clear(struct font_atlas*);

#endif

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


  /* ==============================================================
   *                          MATH
   * =============================================================== */
#define NK_PI 3.141592654f
#define NK_PI_HALF 1.570796326f
#define NK_UTF_INVALID 0xFFFD
#define NK_MAX_FLOAT_PRECISION 2

#define NK_UNUSED(x) ((void) (x))
#define NK_SATURATE(x) (std::max(0.0f, std::min(1.0f, x)))
#define NK_LEN(a) (sizeof(a) / sizeof(a)[0])
#define NK_ABS(a) (((a) < 0) ? -(a) : (a))
#define NK_BETWEEN(x, a, b) ((a) <= (x) && (x) < (b))
#define NK_INBOX(px, py, x, y, w, h) \
  (NK_BETWEEN(px, x, x + w) && NK_BETWEEN(py, y, y + h))
#define INTERSECT(x0, y0, w0, h0, x1, y1, w1, h1) \
  ((x1 < (x0 + w0)) && (x0 < (x1 + w1)) &&        \
   (y1 < (y0 + h0)) && (y0 < (y1 + h1)))
#define NK_CONTAINS(x, y, w, h, bx, by, bw, bh) \
  (NK_INBOX(x, y, bx, by, bw, bh) && NK_INBOX(x + w, y + h, bx, by, bw, bh))

#define vec2_sub(a, b) vec2_from_floats((a).x - (b).x, (a).y - (b).y)
#define vec2_add(a, b) vec2_from_floats((a).x + (b).x, (a).y + (b).y)
#define vec2_len_sqr(a) ((a).x * (a).x + (a).y * (a).y)
#define vec2_muls(a, t) vec2_from_floats((a).x*(t), (a).y*(t))

#define ptr_add(t, p, i) ((t*) ((void*) ((std::byte*) (p) + (i))))
#define ptr_add_const(t, p, i) ((const t*) ((const void*) ((const std::byte*) (p) + (i))))
#define zero_struct(s) zero(&s, sizeof(s))

  /* ==============================================================
   *                          ALIGNMENT
   * =============================================================== */
  /* Pointer to Integer type conversion for pointer alignment */
#if defined(__PTRDIFF_TYPE__) /* This case should work for GCC*/
#define NK_UINT_TO_PTR(x) ((void*) (__PTRDIFF_TYPE__) (x))
#define NK_PTR_TO_UINT(x) ((std::size_t) (__PTRDIFF_TYPE__) (x))
#elif !defined(__GNUC__) /* works for compilers other than LLVM */
#define NK_UINT_TO_PTR(x) ((void*) &((char*) 0)[x])
#define NK_PTR_TO_UINT(x) ((std::size_t) (((char*) x) - (char*) 0))
#elif defined(NK_USE_FIXED_TYPES) /* used if we have <stdint.h> */
#define NK_UINT_TO_PTR(x) ((void*) (uintptr_t) (x))
#define NK_PTR_TO_UINT(x) ((uintptr_t) (x))
#else /* generates warning but works */
#define NK_UINT_TO_PTR(x) ((void*) (x))
#define NK_PTR_TO_UINT(x) ((std::size_t) (x))
#endif

#define NK_ALIGN_PTR(x, mask) \
  (NK_UINT_TO_PTR((NK_PTR_TO_UINT((std::byte*) (x) + (mask - 1)) & ~(mask - 1))))
#define NK_ALIGN_PTR_BACK(x, mask) \
  (NK_UINT_TO_PTR((NK_PTR_TO_UINT((std::byte*) (x)) & ~(mask - 1))))

#if ((defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)) && !defined(EMSCRIPTEN)
#define NK_OFFSETOF(st, m) (__builtin_offsetof(st, m))
#else
#define NK_OFFSETOF(st, m) ((std::uintptr_t) &(((st*) 0)->m))
#endif


  /*template<typename T> struct alignof;
  template<typename T, int size_diff> struct helper{enum {value = size_diff};};
  template<typename T> struct helper<T,0>{enum {value = alignof<T>::value};};
  template<typename T> struct alignof{struct Big {T x; char c;}; enum {
    diff = sizeof(Big) - sizeof(T), value = helper<Big, diff>::value};};*/
#define NK_ALIGNOF(t) (alignof<t>::value)

#define NK_CONTAINER_OF(ptr, type, member) \
  (type*) ((void*) ((char*) (1 ? (ptr) : &((type*) 0)->member) - NK_OFFSETOF(type, member)))
} // namespace nk
#endif
