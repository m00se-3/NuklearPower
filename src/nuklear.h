/** \file nuklear.h
 * \brief main API and documentation file
 *
 * \details
 */
#ifndef NK_NUKLEAR_H_
#define NK_NUKLEAR_H_

#include <cstdint>
#include <array>
#include <utility>
#include <concepts>

/*
 * ==============================================================
 *
 *                          CONSTANTS
 *
 * ===============================================================
 */

#define NK_UNDEFINED (-1.0f)
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

#define NK_FLAG(x) (1 << (x))
#define NK_STRINGIFY(x) #x
#define NK_MACRO_STRINGIFY(x) NK_STRINGIFY(x)
#define NK_STRING_JOIN_IMMEDIATE(arg1, arg2) arg1 ## arg2
#define NK_STRING_JOIN_DELAY(arg1, arg2) NK_STRING_JOIN_IMMEDIATE(arg1, arg2)
#define NK_STRING_JOIN(arg1, arg2) NK_STRING_JOIN_DELAY(arg1, arg2)

#ifdef _MSC_VER
  #define NK_UNIQUE_NAME(name) NK_STRING_JOIN(name,__COUNTER__)
#else
  #define NK_UNIQUE_NAME(name) NK_STRING_JOIN(name,__LINE__)
#endif

#ifndef NK_STATIC_ASSERT
  #define NK_STATIC_ASSERT(exp) typedef char NK_UNIQUE_NAME(_dummy_array)[(exp)?1:0]
#endif

#ifndef NK_FILE_LINE
#ifdef _MSC_VER
  #define NK_FILE_LINE __FILE__ ":" NK_MACRO_STRINGIFY(__COUNTER__)
#else
  #define NK_FILE_LINE __FILE__ ":" NK_MACRO_STRINGIFY(__LINE__)
#endif
#endif

#define NK_MIN(a,b) ((a) < (b) ? (a) : (b))
#define NK_MAX(a,b) ((a) < (b) ? (b) : (a))
#define NK_CLAMP(i,v,x) (NK_MAX(NK_MIN(v,x), i))

template<typename T>
 constexpr auto operator|(T lhs, T rhs) requires std::is_scoped_enum_v<T> {
  return T{std::to_underlying(lhs) | std::to_underlying(rhs)};
}

template<typename T>
 constexpr auto operator|(T lhs, std::integral auto rhs) requires std::is_scoped_enum_v<T> {
  return T{std::to_underlying(lhs) | rhs};
}

template<typename T>
 constexpr auto operator|(std::integral auto lhs, T rhs) -> decltype(lhs) requires std::is_scoped_enum_v<T> {
  return lhs | std::to_underlying(rhs);
}

template<typename T>
 constexpr auto operator&(T lhs, T rhs) requires std::is_scoped_enum_v<T> {
  return T{std::to_underlying(lhs) & std::to_underlying(rhs)};
}

template<typename T>
 constexpr auto operator&(T lhs, std::integral auto rhs) -> decltype(lhs) requires std::is_scoped_enum_v<T> {
  return T{std::to_underlying(lhs) & rhs};
}

template<typename T>
 constexpr auto operator&(std::integral auto lhs, T rhs) -> decltype(lhs) requires std::is_scoped_enum_v<T> {
  return lhs & std::to_underlying(rhs);
}

template<typename T>
 constexpr auto operator|=(std::integral auto lhs, T rhs) -> decltype(lhs) requires std::is_scoped_enum_v<T> {
  return lhs | std::to_underlying(rhs);
}

namespace nk {
  #ifdef NK_INCLUDE_STANDARD_VARARGS
  #include <stdarg.h>
  #if defined(_MSC_VER) && (_MSC_VER >= 1600) /* VS 2010 and above */
  #include <sal.h>
  #define NK_PRINTF_FORMAT_STRING _Printf_format_string_
  #else
  #define NK_PRINTF_FORMAT_STRING
  #endif
  #if defined(__GNUC__)
  #define NK_PRINTF_VARARG_FUNC(fmtargnumber) __attribute__((format(__printf__, fmtargnumber, fmtargnumber+1)))
  #define NK_PRINTF_VALIST_FUNC(fmtargnumber) __attribute__((format(__printf__, fmtargnumber, 0)))
  #else
  #define NK_PRINTF_VARARG_FUNC(fmtargnumber)
  #define NK_PRINTF_VALIST_FUNC(fmtargnumber)
  #endif
  #endif

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

    /* ============================================================================
     *
     *                                  API
     *
     * =========================================================================== */
    struct memory_buffer;
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

    struct color {std::uint8_t r,g,b,a;};
    struct colorf {float r,g,b,a;};
    struct vec2f {float x,y;};
    struct vec2i {short x, y;};
    struct rectf {float x,y,w,h;};
    struct recti {short x,y,w,h;};
    typedef char glyph[NK_UTF_SIZE];
    typedef union {void *ptr; int id;} resource_handle;
    struct image {resource_handle handle; unsigned short w, h; std::array<unsigned short, 4uz> region;};
    struct nine_slice {struct image img; unsigned short l, t, r, b;};
    struct cursor {struct image img; struct vec2f size, offset;};
    struct scroll {std::uint32_t x, y;};

      enum class heading         {UP, RIGHT, DOWN, LEFT};
      enum class btn_behavior {BUTTON_DEFAULT, BUTTON_REPEATER};
      enum class modify : int8_t {FIXED = 0, MODIFIABLE = 1};
      enum class orientation     {VERTICAL, HORIZONTAL};
      enum class collapse_states : int8_t {MINIMIZED = 0, MAXIMIZED = 1};
      enum class show_states : int8_t     {HIDDEN = 0, SHOWN = 1};
      enum class chart_type      {CHART_LINES, CHART_COLUMN, CHART_MAX};
      enum class chart_event     {CHART_HOVERING = 0x01, CHART_CLICKED = 0x02};
      enum class color_format    {RGB, RGBA};
      enum class popup_type      {POPUP_STATIC, POPUP_DYNAMIC};
      enum class layout_format   {DYNAMIC, STATIC};
      enum class tree_type       {TREE_NODE, TREE_TAB};

    typedef void*(*plugin_alloc)(resource_handle, void *old, std::size_t);
    typedef void (*plugin_free)(resource_handle, void *old);
    typedef bool (*plugin_filter)(const text_edit*, rune unicode);
    typedef void(*plugin_paste)(resource_handle, text_edit*);
    typedef void(*plugin_copy)(resource_handle, const char*, int len);

    struct allocator {
      resource_handle userdata;
      plugin_alloc alloc;
      plugin_free free;
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
    /* =============================================================================
     *
     *                                  CONTEXT
     *
     * =============================================================================*/
    /**
     * \page Context
     * Contexts are the main entry point and the majestro of nuklear and contain all required state.
     * They are used for window, memory, input, style, stack, commands and time management and need
     * to be passed into all nuklear GUI specific functions.
     *
     * # Usage
     * To use a context it first has to be initialized which can be achieved by calling
     * one of either `init_default`, `init_fixed`, `init`, `init_custom`.
     * Each takes in a font handle and a specific way of handling memory. Memory control
     * hereby ranges from standard library to just specifying a fixed sized block of memory
     * which nuklear has to manage itself from.
     *
     * ```c
     * struct context ctx;
     * init_xxx(&ctx, ...);
     * while (1) {
     *     // [...]
     *     clear(&ctx);
     * }
     * free(&ctx);
     * ```
     *
     * # Reference
     * Function            | Description
     * --------------------|-------------------------------------------------------
     * \ref init_default | Initializes context with standard library memory allocation (malloc,free)
     * \ref init_fixed   | Initializes context from single fixed size memory block
     * \ref init         | Initializes context with memory allocator callbacks for alloc and free
     * \ref init_custom  | Initializes context from two buffers. One for draw commands the other for window/panel/table allocations
     * \ref clear        | Called at the end of the frame to reset and prepare the context for the next frame
     * \ref free         | Shutdown and free all memory allocated inside the context
     * \ref set_user_data| Utility function to pass user data to draw command
     */

  #ifdef NK_INCLUDE_DEFAULT_ALLOCATOR

    /**
     * # init_default
     * Initializes a `context` struct with a default standard library allocator.
     * Should be used if you don't want to be bothered with memory management in nuklear.
     *
     * ```c
     * bool init_default(struct context *ctx, const struct user_font *font);
     * ```
     *
     * Parameter   | Description
     * ------------|---------------------------------------------------------------
     * \param[in] ctx     | Must point to an either stack or heap allocated `context` struct
     * \param[in] font    | Must point to a previously initialized font handle for more info look at font documentation
     *
     * \returns either `false(0)` on failure or `true(1)` on success.
     */
    NK_API bool init_default(struct context*, const struct user_font*);
  #endif
    /**
     * # init_fixed
     * Initializes a `context` struct from single fixed size memory block
     * Should be used if you want complete control over nuklear's memory management.
     * Especially recommended for system with little memory or systems with virtual memory.
     * For the later case you can just allocate for example 16MB of virtual memory
     * and only the required amount of memory will actually be committed.
     *
     * ```c
     * bool init_fixed(struct context *ctx, void *memory, std::size_t size, const struct user_font *font);
     * ```
     *
     * !!! Warning
     *     make sure the passed memory block is aligned correctly for `draw_commands`.
     *
     * Parameter   | Description
     * ------------|--------------------------------------------------------------
     * \param[in] ctx     | Must point to an either stack or heap allocated `context` struct
     * \param[in] memory  | Must point to a previously allocated memory block
     * \param[in] size    | Must contain the total size of memory
     * \param[in] font    | Must point to a previously initialized font handle for more info look at font documentation
     *
     * \returns either `false(0)` on failure or `true(1)` on success.
     */
    NK_API bool init_fixed(struct context*, void *memory, std::size_t size, const struct user_font*);

    /**
     * # init
     * Initializes a `context` struct with memory allocation callbacks for nuklear to allocate
     * memory from. Used internally for `init_default` and provides a kitchen sink allocation
     * interface to nuklear. Can be useful for cases like monitoring memory consumption.
     *
     * ```c
     * bool init(struct context *ctx, const struct allocator *alloc, const struct user_font *font);
     * ```
     *
     * Parameter   | Description
     * ------------|---------------------------------------------------------------
     * \param[in] ctx     | Must point to an either stack or heap allocated `context` struct
     * \param[in] alloc   | Must point to a previously allocated memory allocator
     * \param[in] font    | Must point to a previously initialized font handle for more info look at font documentation
     *
     * \returns either `false(0)` on failure or `true(1)` on success.
     */
    NK_API bool init(context*, const allocator*, const user_font*);

    /**
     * \brief Initializes a `context` struct from two different either fixed or growing buffers.
     *
     * \details
     * The first buffer is for allocating draw commands while the second buffer is
     * used for allocating windows, panels and state tables.
     *
     * ```c
     * bool init_custom(struct context *ctx, struct buffer *cmds, struct buffer *pool, const struct user_font *font);
     * ```
     *
     * \param[in] ctx    Must point to an either stack or heap allocated `context` struct
     * \param[in] cmds   Must point to a previously initialized memory buffer either fixed or dynamic to store draw commands into
     * \param[in] pool   Must point to a previously initialized memory buffer either fixed or dynamic to store windows, panels and tables
     * \param[in] font   Must point to a previously initialized font handle for more info look at font documentation
     *
     * \returns either `false(0)` on failure or `true(1)` on success.
     */
    NK_API bool init_custom(context*, memory_buffer *cmds, memory_buffer *pool, const user_font*);

    /**
     * \brief Resets the context state at the end of the frame.
     *
     * \details
     * This includes mostly garbage collector tasks like removing windows or table
     * not called and therefore used anymore.
     *
     * ```c
     * void clear(struct context *ctx);
     * ```
     *
     * \param[in] ctx  Must point to a previously initialized `context` struct
     */
    NK_API void clear(struct context*);

    /**
     * \brief Frees all memory allocated by nuklear; Not needed if context was initialized with `init_fixed`.
     *
     * \details
     * ```c
     * void free(struct context *ctx);
     * ```
     *
     * \param[in] ctx  Must point to a previously initialized `context` struct
     */
    NK_API void free(struct context*);

  #ifdef NK_INCLUDE_COMMAND_USERDATA
    /**
     * \brief Sets the currently passed userdata passed down into each draw command.
     *
     * \details
     * ```c
     * void set_user_data(struct context *ctx, handle data);
     * ```
     *
     * \param[in] ctx Must point to a previously initialized `context` struct
     * \param[in] data  Handle with either pointer or index to be passed into every draw commands
     */
    NK_API void set_user_data(struct context*, handle handle);
  #endif
    /* =============================================================================
     *
     *                                  INPUT
     *
     * =============================================================================*/
    /**
     * \page Input
     *
     * The input API is responsible for holding the current input state composed of
     * mouse, key and text input states.
     * It is worth noting that no direct OS or window handling is done in nuklear.
     * Instead all input state has to be provided by platform specific code. This on one hand
     * expects more work from the user and complicates usage but on the other hand
     * provides simple abstraction over a big number of platforms, libraries and other
     * already provided functionality.
     *
     * ```c
     * input_begin(&ctx);
     * while (GetEvent(&evt)) {
     *     if (evt.type == MOUSE_MOVE)
     *         input_motion(&ctx, evt.motion.x, evt.motion.y);
     *     else if (evt.type == [...]) {
     *         // [...]
     *     }
     * } input_end(&ctx);
     * ```
     *
     * # Usage
     * Input state needs to be provided to nuklear by first calling `input_begin`
     * which resets internal state like delta mouse position and button transitions.
     * After `input_begin` all current input state needs to be provided. This includes
     * mouse motion, button and key pressed and released, text input and scrolling.
     * Both event- or state-based input handling are supported by this API
     * and should work without problems. Finally after all input state has been
     * mirrored `input_end` needs to be called to finish input process.
     *
     * ```c
     * struct context ctx;
     * init_xxx(&ctx, ...);
     * while (1) {
     *     Event evt;
     *     input_begin(&ctx);
     *     while (GetEvent(&evt)) {
     *         if (evt.type == MOUSE_MOVE)
     *             input_motion(&ctx, evt.motion.x, evt.motion.y);
     *         else if (evt.type == [...]) {
     *             // [...]
     *         }
     *     }
     *     input_end(&ctx);
     *     // [...]
     *     clear(&ctx);
     * } free(&ctx);
     * ```
     *
     * # Reference
     * Function            | Description
     * --------------------|-------------------------------------------------------
     * \ref input_begin  | Begins the input mirroring process. Needs to be called before all other `input_xxx` calls
     * \ref input_motion | Mirrors mouse cursor position
     * \ref input_key    | Mirrors key state with either pressed or released
     * \ref input_button | Mirrors mouse button state with either pressed or released
     * \ref input_scroll | Mirrors mouse scroll values
     * \ref input_char   | Adds a single ASCII text character into an internal text buffer
     * \ref input_glyph  | Adds a single multi-byte UTF-8 character into an internal text buffer
     * \ref input_unicode| Adds a single unicode rune into an internal text buffer
     * \ref input_end    | Ends the input mirroring process by calculating state changes. Don't call any `input_xxx` function referenced above after this call
     */

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

    /**
     * \brief Begins the input mirroring process by resetting text, scroll
     * mouse, previous mouse position and movement as well as key state transitions.
     *
     * \details
     * ```c
     * void input_begin(struct context*);
     * ```
     *
     * \param[in] ctx Must point to a previously initialized `context` struct
     */
    NK_API void input_begin(struct context*);

    /**
     * \brief Mirrors current mouse position to nuklear
     *
     * \details
     * ```c
     * void input_motion(struct context *ctx, int x, int y);
     * ```
     *
     * \param[in] ctx   Must point to a previously initialized `context` struct
     * \param[in] x     Must hold an integer describing the current mouse cursor x-position
     * \param[in] y     Must hold an integer describing the current mouse cursor y-position
     */
    NK_API void input_motion(struct context*, int x, int y);

    /**
     * \brief Mirrors the state of a specific key to nuklear
     *
     * \details
     * ```c
     * void input_key(struct context*, enum keys key, bool down);
     * ```
     *
     * \param[in] ctx      Must point to a previously initialized `context` struct
     * \param[in] key      Must be any value specified in enum `keys` that needs to be mirrored
     * \param[in] down     Must be 0 for key is up and 1 for key is down
     */
    NK_API void input_key(context*, keys, bool down);

    /**
     * \brief Mirrors the state of a specific mouse button to nuklear
     *
     * \details
     * ```c
     * void input_button(struct context *ctx, enum buttons btn, int x, int y, bool down);
     * ```
     *
     * \param[in] ctx     Must point to a previously initialized `context` struct
     * \param[in] btn     Must be any value specified in enum `buttons` that needs to be mirrored
     * \param[in] x       Must contain an integer describing mouse cursor x-position on click up/down
     * \param[in] y       Must contain an integer describing mouse cursor y-position on click up/down
     * \param[in] down    Must be 0 for key is up and 1 for key is down
     */
    NK_API void input_button(context*, buttons, int x, int y, bool down);

    /**
     * \brief Copies the last mouse scroll value to nuklear.
     *
     * \details
     * Is generally a scroll value. So does not have to come from mouse and could
     * also originate from balls, tracks, linear guide rails, or other programs.
     *
     * ```c
     * void input_scroll(struct context *ctx, struct vec2 val);
     * ```
     *
     * \param[in] ctx     | Must point to a previously initialized `context` struct
     * \param[in] val     | vector with both X- as well as Y-scroll value
     */
    NK_API void input_scroll(struct context*, struct vec2f val);

    /**
     * \brief Copies a single ASCII character into an internal text buffer
     *
     * \details
     * This is basically a helper function to quickly push ASCII characters into
     * nuklear.
     *
     * \note
     *     Stores up to NK_INPUT_MAX bytes between `input_begin` and `input_end`.
     *
     * ```c
     * void input_char(struct context *ctx, char c);
     * ```
     *
     * \param[in] ctx     | Must point to a previously initialized `context` struct
     * \param[in] c       | Must be a single ASCII character preferable one that can be printed
     */
    NK_API void input_char(struct context*, char);

    /**
     * \brief Converts an encoded unicode rune into UTF-8 and copies the result into an
     * internal text buffer.
     *
     * \note
     *     Stores up to NK_INPUT_MAX bytes between `input_begin` and `input_end`.
     *
     * ```c
     * void input_glyph(struct context *ctx, const glyph g);
     * ```
     *
     * \param[in] ctx     | Must point to a previously initialized `context` struct
     * \param[in] g       | UTF-32 unicode codepoint
     */
    NK_API void input_glyph(struct context*, const glyph);

    /**
     * \brief Converts a unicode rune into UTF-8 and copies the result
     * into an internal text buffer.
     *
     * \details
     * \note
     *     Stores up to NK_INPUT_MAX bytes between `input_begin` and `input_end`.
     *
     * ```c
     * void input_unicode(struct context*, rune rune);
     * ```
     *
     * \param[in] ctx     | Must point to a previously initialized `context` struct
     * \param[in] rune    | UTF-32 unicode codepoint
     */
    NK_API void input_unicode(struct context*, rune);

    /**
     * \brief End the input mirroring process by resetting mouse grabbing
     * state to ensure the mouse cursor is not grabbed indefinitely.
     *
     * \details
     * ```c
     * void input_end(struct context *ctx);
     * ```
     *
     * \param[in] ctx     | Must point to a previously initialized `context` struct
     */
    NK_API void input_end(struct context*);

    /** =============================================================================
     *
     *                                  DRAWING
     *
     * =============================================================================*/
    /**
     * \page Drawing
     * This library was designed to be render backend agnostic so it does
     * not draw anything to screen directly. Instead all drawn shapes, widgets
     * are made of, are buffered into memory and make up a command queue.
     * Each frame therefore fills the command buffer with draw commands
     * that then need to be executed by the user and his own render backend.
     * After that the command buffer needs to be cleared and a new frame can be
     * started. It is probably important to note that the command buffer is the main
     * drawing API and the optional vertex buffer API only takes this format and
     * converts it into a hardware accessible format.
     *
     * # Usage
     * To draw all draw commands accumulated over a frame you need your own render
     * backend able to draw a number of 2D primitives. This includes at least
     * filled and stroked rectangles, circles, text, lines, triangles and scissors.
     * As soon as this criterion is met you can iterate over each draw command
     * and execute each draw command in a interpreter like fashion:
     *
     * ```c
     * const struct command *cmd = 0;
     * foreach(cmd, &ctx) {
     *     switch (cmd->type) {
     *     case NK_COMMAND_LINE:
     *         your_draw_line_function(...)
     *         break;
     *     case NK_COMMAND_RECT
     *         your_draw_rect_function(...)
     *         break;
     *     case //...:
     *         //[...]
     *     }
     * }
     * ```
     *
     * In program flow context draw commands need to be executed after input has been
     * gathered and the complete UI with windows and their contained widgets have
     * been executed and before calling `clear` which frees all previously
     * allocated draw commands.
     *
     * ```c
     * struct context ctx;
     * init_xxx(&ctx, ...);
     * while (1) {
     *     Event evt;
     *     input_begin(&ctx);
     *     while (GetEvent(&evt)) {
     *         if (evt.type == MOUSE_MOVE)
     *             input_motion(&ctx, evt.motion.x, evt.motion.y);
     *         else if (evt.type == [...]) {
     *             [...]
     *         }
     *     }
     *     input_end(&ctx);
     *     //
     *     // [...]
     *     //
     *     const struct command *cmd = 0;
     *     foreach(cmd, &ctx) {
     *     switch (cmd->type) {
     *     case NK_COMMAND_LINE:
     *         your_draw_line_function(...)
     *         break;
     *     case NK_COMMAND_RECT
     *         your_draw_rect_function(...)
     *         break;
     *     case ...:
     *         // [...]
     *     }
     *     clear(&ctx);
     * }
     * free(&ctx);
     * ```
     *
     * You probably noticed that you have to draw all of the UI each frame which is
     * quite wasteful. While the actual UI updating loop is quite fast rendering
     * without actually needing it is not. So there are multiple things you could do.
     *
     * First is only update on input. This of course is only an option if your
     * application only depends on the UI and does not require any outside calculations.
     * If you actually only update on input make sure to update the UI two times each
     * frame and call `clear` directly after the first pass and only draw in
     * the second pass. In addition it is recommended to also add additional timers
     * to make sure the UI is not drawn more than a fixed number of frames per second.
     *
     * ```c
     * struct context ctx;
     * init_xxx(&ctx, ...);
     * while (1) {
     *     // [...wait for input ]
     *     // [...do two UI passes ...]
     *     do_ui(...)
     *     clear(&ctx);
     *     do_ui(...)
     *     //
     *     // draw
     *     const struct command *cmd = 0;
     *     foreach(cmd, &ctx) {
     *     switch (cmd->type) {
     *     case NK_COMMAND_LINE:
     *         your_draw_line_function(...)
     *         break;
     *     case NK_COMMAND_RECT
     *         your_draw_rect_function(...)
     *         break;
     *     case ...:
     *         //[...]
     *     }
     *     clear(&ctx);
     * }
     * free(&ctx);
     * ```
     *
     * The second probably more applicable trick is to only draw if anything changed.
     * It is not really useful for applications with continuous draw loop but
     * quite useful for desktop applications. To actually get nuklear to only
     * draw on changes you first have to define `NK_ZERO_COMMAND_MEMORY` and
     * allocate a memory buffer that will store each unique drawing output.
     * After each frame you compare the draw command memory inside the library
     * with your allocated buffer by memcmp. If memcmp detects differences
     * you have to copy the command buffer into the allocated buffer
     * and then draw like usual (this example uses fixed memory but you could
     * use dynamically allocated memory).
     *
     * ```c
     * //[... other defines ...]
     * #define NK_ZERO_COMMAND_MEMORY
     * #include "nuklear.h"
     * //
     * // setup context
     * struct context ctx;
     * void *last = calloc(1,64*1024);
     * void *buf = calloc(1,64*1024);
     * init_fixed(&ctx, buf, 64*1024);
     * //
     * // loop
     * while (1) {
     *     // [...input...]
     *     // [...ui...]
     *     void *cmds = buffer_memory(&ctx.memory);
     *     if (memcmp(cmds, last, ctx.memory.allocated)) {
     *         memcpy(last,cmds,ctx.memory.allocated);
     *         const struct command *cmd = 0;
     *         foreach(cmd, &ctx) {
     *             switch (cmd->type) {
     *             case NK_COMMAND_LINE:
     *                 your_draw_line_function(...)
     *                 break;
     *             case NK_COMMAND_RECT
     *                 your_draw_rect_function(...)
     *                 break;
     *             case ...:
     *                 // [...]
     *             }
     *         }
     *     }
     *     clear(&ctx);
     * }
     * free(&ctx);
     * ```
     *
     * Finally while using draw commands makes sense for higher abstracted platforms like
     * X11 and Win32 or drawing libraries it is often desirable to use graphics
     * hardware directly. Therefore it is possible to just define
     * `NK_INCLUDE_VERTEX_BUFFER_OUTPUT` which includes optional vertex output.
     * To access the vertex output you first have to convert all draw commands into
     * vertexes by calling `convert` which takes in your preferred vertex format.
     * After successfully converting all draw commands just iterate over and execute all
     * vertex draw commands:
     *
     * ```c
     * // fill configuration
     * struct your_vertex
     * {
     *     float pos[2]; // important to keep it to 2 floats
     *     float uv[2];
     *     unsigned char col[4];
     * };
     * struct convert_config cfg = {};
     * static const struct draw_vertex_layout_element vertex_layout[] = {
     *     {NK_VERTEX_POSITION, NK_FORMAT_FLOAT, NK_OFFSETOF(struct your_vertex, pos)},
     *     {NK_VERTEX_TEXCOORD, NK_FORMAT_FLOAT, NK_OFFSETOF(struct your_vertex, uv)},
     *     {NK_VERTEX_COLOR, NK_FORMAT_R8G8B8A8, NK_OFFSETOF(struct your_vertex, col)},
     *     {NK_VERTEX_LAYOUT_END}
     * };
     * cfg.shape_AA = NK_ANTI_ALIASING_ON;
     * cfg.line_AA = NK_ANTI_ALIASING_ON;
     * cfg.vertex_layout = vertex_layout;
     * cfg.vertex_size = sizeof(struct your_vertex);
     * cfg.vertex_alignment = NK_ALIGNOF(struct your_vertex);
     * cfg.circle_segment_count = 22;
     * cfg.curve_segment_count = 22;
     * cfg.arc_segment_count = 22;
     * cfg.global_alpha = 1.0f;
     * cfg.tex_null = dev->tex_null;
     * //
     * // setup buffers and convert
     * struct buffer cmds, verts, idx;
     * buffer_init_default(&cmds);
     * buffer_init_default(&verts);
     * buffer_init_default(&idx);
     * convert(&ctx, &cmds, &verts, &idx, &cfg);
     * //
     * // draw
     * draw_foreach(cmd, &ctx, &cmds) {
     * if (!cmd->elem_count) continue;
     *     //[...]
     * }
     * buffer_free(&cms);
     * buffer_free(&verts);
     * buffer_free(&idx);
     * ```
     *
     * # Reference
     * Function            | Description
     * --------------------|-------------------------------------------------------
     * \ref _begin       | Returns the first draw command in the context draw command list to be drawn
     * \ref _next        | Increments the draw command iterator to the next command inside the context draw command list
     * \ref foreach      | Iterates over each draw command inside the context draw command list
     * \ref convert      | Converts from the abstract draw commands list into a hardware accessible vertex format
     * \ref draw_begin   | Returns the first vertex command in the context vertex draw list to be executed
     * \ref _draw_next   | Increments the vertex command iterator to the next command inside the context vertex command list
     * \ref _draw_end    | Returns the end of the vertex draw list
     * \ref draw_foreach | Iterates over each vertex draw command inside the vertex draw list
     */

    enum anti_aliasing {NK_ANTI_ALIASING_OFF, NK_ANTI_ALIASING_ON};
    enum convert_result {
      NK_CONVERT_SUCCESS = 0,
      NK_CONVERT_INVALID_PARAM = 1,
      NK_CONVERT_COMMAND_BUFFER_FULL = NK_FLAG(1),
      NK_CONVERT_VERTEX_BUFFER_FULL = NK_FLAG(2),
      NK_CONVERT_ELEMENT_BUFFER_FULL = NK_FLAG(3)
  };
    struct draw_null_texture {
      resource_handle texture; /**!< texture handle to a texture with a white pixel */
      struct vec2f uv; /**!< coordinates to a white pixel in the texture  */
    };
    struct convert_config {
      float global_alpha;             /**!< global alpha value */
      anti_aliasing line_AA;  /**!< line anti-aliasing flag can be turned off if you are tight on memory */
      anti_aliasing shape_AA; /**!< shape anti-aliasing flag can be turned off if you are tight on memory */
      unsigned circle_segment_count;  /**!< number of segments used for circles: default to 22 */
      unsigned arc_segment_count;     /**!< number of segments used for arcs: default to 22 */
      unsigned curve_segment_count;   /**!< number of segments used for curves: default to 22 */
      draw_null_texture tex_null; /**!< handle to texture with a white pixel for shape drawing */
      const draw_vertex_layout_element *vertex_layout; /**!< describes the vertex output format and packing */
      std::size_t vertex_size;      /**!< sizeof one vertex for vertex packing */
      std::size_t vertex_alignment; /**!< vertex alignment: Can be obtained by NK_ALIGNOF */
    };

    /**
     * \brief Returns a draw command list iterator to iterate all draw
     * commands accumulated over one frame.
     *
     * \details
     * ```c
     * const struct command* _begin(struct context*);
     * ```
     *
     * \param[in] ctx     | must point to an previously initialized `context` struct at the end of a frame
     *
     * \returns draw command pointer pointing to the first command inside the draw command list
     */
    NK_API const struct command* _begin(struct context*);

    /**
     * \brief Returns draw command pointer pointing to the next command inside the draw command list
     *
     * \details
     * ```c
     * const struct command* _next(struct context*, const struct command*);
     * ```
     *
     * \param[in] ctx     | Must point to an previously initialized `context` struct at the end of a frame
     * \param[in] cmd     | Must point to an previously a draw command either returned by `_begin` or `_next`
     *
     * \returns draw command pointer pointing to the next command inside the draw command list
     */
    NK_API const struct command* _next(struct context*, const struct command*);

    /**
     * \brief Iterates over each draw command inside the context draw command list
     *
     * ```c
     * #define foreach(c, ctx)
     * ```
     *
     * \param[in] ctx     | Must point to an previously initialized `context` struct at the end of a frame
     * \param[in] cmd     | Command pointer initialized to NULL
     */
  #define foreach(c, ctx) for((c) = _begin(ctx); (c) != 0; (c) = _next(ctx,c))

  #ifdef NK_INCLUDE_VERTEX_BUFFER_OUTPUT

    /**
     * \brief Converts all internal draw commands into vertex draw commands and fills
     * three buffers with vertexes, vertex draw commands and vertex indices.
     *
     * \details
     * The vertex format as well as some other configuration values have to be
     * configured by filling out a `convert_config` struct.
     *
     * ```c
     * flag convert(struct context *ctx, struct buffer *cmds,
     *     struct buffer *vertices, struct buffer *elements, const struct convert_config*);
     * ```
     *
     * \param[in] ctx      Must point to an previously initialized `context` struct at the end of a frame
     * \param[out] cmds     Must point to a previously initialized buffer to hold converted vertex draw commands
     * \param[out] vertices Must point to a previously initialized buffer to hold all produced vertices
     * \param[out] elements Must point to a previously initialized buffer to hold all produced vertex indices
     * \param[in] config   Must point to a filled out `config` struct to configure the conversion process
     *
     * \returns one of enum convert_result error codes
     *
     * Parameter                       | Description
     * --------------------------------|-----------------------------------------------------------
     * NK_CONVERT_SUCCESS              | Signals a successful draw command to vertex buffer conversion
     * NK_CONVERT_INVALID_PARAM        | An invalid argument was passed in the function call
     * NK_CONVERT_COMMAND_BUFFER_FULL  | The provided buffer for storing draw commands is full or failed to allocate more memory
     * NK_CONVERT_VERTEX_BUFFER_FULL   | The provided buffer for storing vertices is full or failed to allocate more memory
     * NK_CONVERT_ELEMENT_BUFFER_FULL  | The provided buffer for storing indices is full or failed to allocate more memory
     */
    NK_API flags convert(struct context*, struct buffer *cmds, struct buffer *vertices, struct buffer *elements, const struct convert_config*);

    /**
     * \brief Returns a draw vertex command buffer iterator to iterate over the vertex draw command buffer
     *
     * \details
     * ```c
     * const struct draw_command* _draw_begin(const struct context*, const struct buffer*);
     * ```
     *
     * \param[in] ctx     | Must point to an previously initialized `context` struct at the end of a frame
     * \param[in] buf     | Must point to an previously by `convert` filled out vertex draw command buffer
     *
     * \returns vertex draw command pointer pointing to the first command inside the vertex draw command buffer
     */
    NK_API const struct draw_command* _draw_begin(const struct context*, const struct buffer*);

    /**

     * # # _draw_end
     * \returns the vertex draw command at the end of the vertex draw command buffer
     *
     * ```c
     * const struct draw_command* _draw_end(const struct context *ctx, const struct buffer *buf);
     * ```
     *
     * Parameter   | Description
     * ------------|-----------------------------------------------------------
     * \param[in] ctx     | Must point to an previously initialized `context` struct at the end of a frame
     * \param[in] buf     | Must point to an previously by `convert` filled out vertex draw command buffer
     *
     * \returns vertex draw command pointer pointing to the end of the last vertex draw command inside the vertex draw command buffer

     */
    NK_API const struct draw_command* _draw_end(const struct context*, const struct buffer*);

    /**
     * # # _draw_next
     * Increments the vertex draw command buffer iterator
     *
     * ```c
     * const struct draw_command* _draw_next(const struct draw_command*, const struct buffer*, const struct context*);
     * ```
     *
     * Parameter   | Description
     * ------------|-----------------------------------------------------------
     * \param[in] cmd     | Must point to an previously either by `_draw_begin` or `_draw_next` returned vertex draw command
     * \param[in] buf     | Must point to an previously by `convert` filled out vertex draw command buffer
     * \param[in] ctx     | Must point to an previously initialized `context` struct at the end of a frame
     *
     * \returns vertex draw command pointer pointing to the end of the last vertex draw command inside the vertex draw command buffer

     */
    NK_API const struct draw_command* _draw_next(const struct draw_command*, const struct buffer*, const struct context*);

    /**
     * # # draw_foreach
     * Iterates over each vertex draw command inside a vertex draw command buffer
     *
     * ```c
     * #define draw_foreach(cmd,ctx, b)
     * ```
     *
     * Parameter   | Description
     * ------------|-----------------------------------------------------------
     * \param[in] cmd     | `draw_command`iterator set to NULL
     * \param[in] buf     | Must point to an previously by `convert` filled out vertex draw command buffer
     * \param[in] ctx     | Must point to an previously initialized `context` struct at the end of a frame
     */

  #define draw_foreach(cmd,ctx, b) for((cmd)=_draw_begin(ctx, b); (cmd)!=0; (cmd)=_draw_next(cmd, b, ctx))
  #endif

    /** =============================================================================
     *
     *                                  WINDOW
     *
     * =============================================================================*/
    /**
     * \page Window
     * Windows are the main persistent state used inside nuklear and are life time
     * controlled by simply "retouching" (i.e. calling) each window each frame.
     * All widgets inside nuklear can only be added inside the function pair `begin_xxx`
     * and `end`. Calling any widgets outside these two functions will result in an
     * assert in debug or no state change in release mode.<br /><br />
     *
     * Each window holds frame persistent state like position, size, flag, state tables,
     * and some garbage collected internal persistent widget state. Each window
     * is linked into a window stack list which determines the drawing and overlapping
     * order. The topmost window thereby is the currently active window.<br /><br />
     *
     * To change window position inside the stack occurs either automatically by
     * user input by being clicked on or programmatically by calling `window_focus`.
     * Windows by default are visible unless explicitly being defined with flag
     * `NK_WINDOW_HIDDEN`, the user clicked the close button on windows with flag
     * `NK_WINDOW_CLOSABLE` or if a window was explicitly hidden by calling
     * `window_show`. To explicitly close and destroy a window call `window_close`.<br /><br />
     *
     * # Usage
     * To create and keep a window you have to call one of the two `begin_xxx`
     * functions to start window declarations and `end` at the end. Furthermore it
     * is recommended to check the return value of `begin_xxx` and only process
     * widgets inside the window if the value is not 0. Either way you have to call
     * `end` at the end of window declarations. Furthermore, do not attempt to
     * nest `begin_xxx` calls which will hopefully result in an assert or if not
     * in a segmentation fault.
     *
     * ```c
     * if (begin_xxx(...) {
     *     // [... widgets ...]
     * }
     * end(ctx);
     * ```
     *
     * In the grand concept window and widget declarations need to occur after input
     * handling and before drawing to screen. Not doing so can result in higher
     * latency or at worst invalid behavior. Furthermore make sure that `clear`
     * is called at the end of the frame. While nuklear's default platform backends
     * already call `clear` for you if you write your own backend not calling
     * `clear` can cause asserts or even worse undefined behavior.
     *
     * ```c
     * struct context ctx;
     * init_xxx(&ctx, ...);
     * while (1) {
     *     Event evt;
     *     input_begin(&ctx);
     *     while (GetEvent(&evt)) {
     *         if (evt.type == MOUSE_MOVE)
     *             input_motion(&ctx, evt.motion.x, evt.motion.y);
     *         else if (evt.type == [...]) {
     *             input_xxx(...);
     *         }
     *     }
     *     input_end(&ctx);
     *
     *     if (begin_xxx(...) {
     *         //[...]
     *     }
     *     end(ctx);
     *
     *     const struct command *cmd = 0;
     *     foreach(cmd, &ctx) {
     *     case NK_COMMAND_LINE:
     *         your_draw_line_function(...)
     *         break;
     *     case NK_COMMAND_RECT
     *         your_draw_rect_function(...)
     *         break;
     *     case //...:
     *         //[...]
     *     }
     *     clear(&ctx);
     * }
     * free(&ctx);
     * ```
     *
     * # Reference
     * Function                            | Description
     * ------------------------------------|----------------------------------------
     * \ref begin                            | Starts a new window; needs to be called every frame for every window (unless hidden) or otherwise the window gets removed
     * \ref begin_titled                     | Extended window start with separated title and identifier to allow multiple windows with same name but not title
     * \ref end                              | Needs to be called at the end of the window building process to process scaling, scrollbars and general cleanup
     *
     * \ref window_find                      | Finds and returns the window with give name
     * \ref window_get_bounds                | Returns a rectangle with screen position and size of the currently processed window.
     * \ref window_get_position              | Returns the position of the currently processed window
     * \ref window_get_size                  | Returns the size with width and height of the currently processed window
     * \ref window_get_width                 | Returns the width of the currently processed window
     * \ref window_get_height                | Returns the height of the currently processed window
     * \ref window_get_panel                 | Returns the underlying panel which contains all processing state of the current window
     * \ref window_get_content_region        | Returns the position and size of the currently visible and non-clipped space inside the currently processed window
     * \ref window_get_content_region_min    | Returns the upper rectangle position of the currently visible and non-clipped space inside the currently processed window
     * \ref window_get_content_region_max    | Returns the upper rectangle position of the currently visible and non-clipped space inside the currently processed window
     * \ref window_get_content_region_size   | Returns the size of the currently visible and non-clipped space inside the currently processed window
     * \ref window_get_canvas                | Returns the draw command buffer. Can be used to draw custom widgets
     * \ref window_get_scroll                | Gets the scroll offset of the current window
     * \ref window_has_focus                 | Returns if the currently processed window is currently active
     * \ref window_is_collapsed              | Returns if the window with given name is currently minimized/collapsed
     * \ref window_is_closed                 | Returns if the currently processed window was closed
     * \ref window_is_hidden                 | Returns if the currently processed window was hidden
     * \ref window_is_active                 | Same as window_has_focus for some reason
     * \ref window_is_hovered                | Returns if the currently processed window is currently being hovered by mouse
     * \ref window_is_any_hovered            | Return if any window currently hovered
     * \ref item_is_any_active               | Returns if any window or widgets is currently hovered or active
    //
     * \ref window_set_bounds                | Updates position and size of the currently processed window
     * \ref window_set_position              | Updates position of the currently process window
     * \ref window_set_size                  | Updates the size of the currently processed window
     * \ref window_set_focus                 | Set the currently processed window as active window
     * \ref window_set_scroll                | Sets the scroll offset of the current window
    //
     * \ref window_close                     | Closes the window with given window name which deletes the window at the end of the frame
     * \ref window_collapse                  | Collapses the window with given window name
     * \ref window_collapse_if               | Collapses the window with given window name if the given condition was met
     * \ref window_show                      | Hides a visible or reshows a hidden window
     * \ref window_show_if                   | Hides/shows a window depending on condition

     * # panel_flags
     * Flag                        | Description
     * ----------------------------|----------------------------------------
     * NK_WINDOW_BORDER            | Draws a border around the window to visually separate window from the background
     * NK_WINDOW_MOVABLE           | The movable flag indicates that a window can be moved by user input or by dragging the window header
     * NK_WINDOW_SCALABLE          | The scalable flag indicates that a window can be scaled by user input by dragging a scaler icon at the button of the window
     * NK_WINDOW_CLOSABLE          | Adds a closable icon into the header
     * NK_WINDOW_MINIMIZABLE       | Adds a minimize icon into the header
     * NK_WINDOW_NO_SCROLLBAR      | Removes the scrollbar from the window
     * NK_WINDOW_TITLE             | Forces a header at the top at the window showing the title
     * NK_WINDOW_SCROLL_AUTO_HIDE  | Automatically hides the window scrollbar if no user interaction: also requires delta time in `context` to be set each frame
     * NK_WINDOW_BACKGROUND        | Always keep window in the background
     * NK_WINDOW_SCALE_LEFT        | Puts window scaler in the left-bottom corner instead right-bottom
     * NK_WINDOW_NO_INPUT          | Prevents window of scaling, moving or getting focus
     *
     * # collapse_states
     * State           | Description
     * ----------------|-----------------------------------------------------------
     * NK_MINIMIZED| UI section is collapsed and not visible until maximized
     * NK_MAXIMIZED| UI section is extended and visible until minimized
     */

    enum class panel_flags {
      WINDOW_BORDER            = NK_FLAG(0),
      WINDOW_MOVABLE           = NK_FLAG(1),
      WINDOW_SCALABLE          = NK_FLAG(2),
      WINDOW_CLOSABLE          = NK_FLAG(3),
      WINDOW_MINIMIZABLE       = NK_FLAG(4),
      WINDOW_NO_SCROLLBAR      = NK_FLAG(5),
      WINDOW_TITLE             = NK_FLAG(6),
      WINDOW_SCROLL_AUTO_HIDE  = NK_FLAG(7),
      WINDOW_BACKGROUND        = NK_FLAG(8),
      WINDOW_SCALE_LEFT        = NK_FLAG(9),
      WINDOW_NO_INPUT          = NK_FLAG(10)
  };

    /**
     * # # begin
     * Starts a new window; needs to be called every frame for every
     * window (unless hidden) or otherwise the window gets removed
     *
     * ```c
     * bool begin(struct context *ctx, const char *title, struct rect bounds, flag flag);
     * ```
     *
     * Parameter   | Description
     * ------------|-----------------------------------------------------------
     * \param[in] ctx     | Must point to an previously initialized `context` struct
     * \param[in] title   | Window title and identifier. Needs to be persistent over frames to identify the window
     * \param[in] bounds  | Initial position and window size. However if you do not define `NK_WINDOW_SCALABLE` or `NK_WINDOW_MOVABLE` you can set window position and size every frame
     * \param[in] flags   | Window flags defined in the panel_flags section with a number of different window behaviors
     *
     * \returns `true(1)` if the window can be filled up with widgets from this point
     * until `end` or `false(0)` otherwise for example if minimized

     */
    NK_API bool begin(context *ctx, const char *title, rectf bounds, flag flags);

    /**
     * # # begin_titled
     * Extended window start with separated title and identifier to allow multiple
     * windows with same title but not name
     *
     * ```c
     * bool begin_titled(struct context *ctx, const char *name, const char *title, struct rect bounds, flag flag);
     * ```
     *
     * Parameter   | Description
     * ------------|-----------------------------------------------------------
     * \param[in] ctx     | Must point to an previously initialized `context` struct
     * \param[in] name    | Window identifier. Needs to be persistent over frames to identify the window
     * \param[in] title   | Window title displayed inside header if flag `NK_WINDOW_TITLE` or either `NK_WINDOW_CLOSABLE` or `NK_WINDOW_MINIMIZED` was set
     * \param[in] bounds  | Initial position and window size. However if you do not define `NK_WINDOW_SCALABLE` or `NK_WINDOW_MOVABLE` you can set window position and size every frame
     * \param[in] flags   | Window flags defined in the panel_flags section with a number of different window behaviors
     *
     * \returns `true(1)` if the window can be filled up with widgets from this point
     * until `end` or `false(0)` otherwise for example if minimized

     */
    NK_API bool begin_titled(context *ctx, const char *name, const char *title, rectf bounds, flag flags);

    /**
     * # # end
     * Needs to be called at the end of the window building process to process scaling, scrollbars and general cleanup.
     * All widget calls after this functions will result in asserts or no state changes
     *
     * ```c
     * void end(struct context *ctx);
     * ```
     *
     * Parameter   | Description
     * ------------|-----------------------------------------------------------
     * \param[in] ctx     | Must point to an previously initialized `context` struct

     */
    NK_API void end(struct context *ctx);

    /**
     * # # window_find
     * Finds and returns a window from passed name
     *
     * ```c
     * struct window *window_find(struct context *ctx, const char *name);
     * ```
     *
     * Parameter   | Description
     * ------------|-----------------------------------------------------------
     * \param[in] ctx     | Must point to an previously initialized `context` struct
     * \param[in] name    | Window identifier
     *
     * \returns a `window` struct pointing to the identified window or NULL if
     * no window with the given name was found
     */
    NK_API struct window *window_find(const struct context *ctx, const char *name);

    /**
     * # # window_get_bounds
     * \returns a rectangle with screen position and size of the currently processed window
     *
     * !!! \warning
     *     Only call this function between calls `begin_xxx` and `end`
     * ```c
     * struct rect window_get_bounds(const struct context *ctx);
     * ```
     *
     * Parameter   | Description
     * ------------|-----------------------------------------------------------
     * \param[in] ctx     | Must point to an previously initialized `context` struct
     *
     * \returns a `rect` struct with window upper left window position and size

     */
    NK_API struct rectf window_get_bounds(const struct context *ctx);

    /**
     * # # window_get_position
     * \returns the position of the currently processed window.
     *
     * !!! \warning
     *     Only call this function between calls `begin_xxx` and `end`
     * ```c
     * struct vec2 window_get_position(const struct context *ctx);
     * ```
     *
     * Parameter   | Description
     * ------------|-----------------------------------------------------------
     * \param[in] ctx     | Must point to an previously initialized `context` struct
     *
     * \returns a `vec2` struct with window upper left position

     */
    NK_API struct vec2f window_get_position(const struct context *ctx);

    /**
     * # # window_get_size
     * \returns the size with width and height of the currently processed window.
     *
     * !!! \warning
     *     Only call this function between calls `begin_xxx` and `end`
     * ```c
     * struct vec2 window_get_size(const struct context *ctx);
     * ```
     *
     * Parameter   | Description
     * ------------|-----------------------------------------------------------
     * \param[in] ctx     | Must point to an previously initialized `context` struct
     *
     * \returns a `vec2` struct with window width and height

     */
    NK_API struct vec2f window_get_size(const struct context *ctx);

    /**
     * window_get_width
     * \returns the width of the currently processed window.
     *
     * !!! \warning
     *     Only call this function between calls `begin_xxx` and `end`
     * ```c
     * float window_get_width(const struct context *ctx);
     * ```
     *
     * Parameter   | Description
     * ------------|-----------------------------------------------------------
     * \param[in] ctx     | Must point to an previously initialized `context` struct
     *
     * \returns the current window width
     */
    NK_API float window_get_width(const struct context *ctx);

    /**
     * # # window_get_height
     * \returns the height of the currently processed window.
     *
     * !!! \warning
     *     Only call this function between calls `begin_xxx` and `end`
     * ```c
     * float window_get_height(const struct context *ctx);
     * ```
     *
     * Parameter   | Description
     * ------------|-----------------------------------------------------------
     * \param[in] ctx     | Must point to an previously initialized `context` struct
     *
     * \returns the current window height

     */
    NK_API float window_get_height(const struct context* ctx);

    /**
     * # # window_get_panel
     * \returns the underlying panel which contains all processing state of the current window.
     *
     * !!! \warning
     *     Only call this function between calls `begin_xxx` and `end`
     * !!! \warning
     *     Do not keep the returned panel pointer around, it is only valid until `end`
     * ```c
     * struct panel* window_get_panel(struct context *ctx);
     * ```
     *
     * Parameter   | Description
     * ------------|-----------------------------------------------------------
     * \param[in] ctx     | Must point to an previously initialized `context` struct
     *
     * \returns a pointer to window internal `panel` state.

     */
    NK_API struct panel* window_get_panel(const struct context* ctx);

    /**
     * # # window_get_content_region
     * \returns the position and size of the currently visible and non-clipped space
     * inside the currently processed window.
     *
     * !!! \warning
     *     Only call this function between calls `begin_xxx` and `end`
     *
     * ```c
     * struct rect window_get_content_region(struct context *ctx);
     * ```
     *
     * Parameter   | Description
     * ------------|-----------------------------------------------------------
     * \param[in] ctx     | Must point to an previously initialized `context` struct
     *
     * \returns `rect` struct with screen position and size (no scrollbar offset)
     * of the visible space inside the current window

     */
    NK_API struct rectf window_get_content_region(const struct context* ctx);

    /**
     * # # window_get_content_region_min
     * \returns the upper left position of the currently visible and non-clipped
     * space inside the currently processed window.
     *
     * !!! \warning
     *     Only call this function between calls `begin_xxx` and `end`
     *
     * ```c
     * struct vec2 window_get_content_region_min(struct context *ctx);
     * ```
     *
     * Parameter   | Description
     * ------------|-----------------------------------------------------------
     * \param[in] ctx     | Must point to an previously initialized `context` struct
     *
     * returns `vec2` struct with  upper left screen position (no scrollbar offset)
     * of the visible space inside the current window

     */
    NK_API struct vec2f window_get_content_region_min(const struct context *ctx);

    /**
     * # # window_get_content_region_max
     * \returns the lower right screen position of the currently visible and
     * non-clipped space inside the currently processed window.
     *
     * !!! \warning
     *     Only call this function between calls `begin_xxx` and `end`
     *
     * ```c
     * struct vec2 window_get_content_region_max(struct context *ctx);
     * ```
     *
     * Parameter   | Description
     * ------------|-----------------------------------------------------------
     * \param[in] ctx     | Must point to an previously initialized `context` struct
     *
     * \returns `vec2` struct with lower right screen position (no scrollbar offset)
     * of the visible space inside the current window

     */
    NK_API struct vec2f window_get_content_region_max(const struct context *ctx);

    /**
     * # # window_get_content_region_size
     * \returns the size of the currently visible and non-clipped space inside the
     * currently processed window
     *
     * !!! \warning
     *     Only call this function between calls `begin_xxx` and `end`
     *
     * ```c
     * struct vec2 window_get_content_region_size(struct context *ctx);
     * ```
     *
     * Parameter   | Description
     * ------------|-----------------------------------------------------------
     * \param[in] ctx     | Must point to an previously initialized `context` struct
     *
     * \returns `vec2` struct with size the visible space inside the current window

     */
    NK_API struct vec2f window_get_content_region_size(const struct context *ctx);

    /**
     * # # window_get_canvas
     * \returns the draw command buffer. Can be used to draw custom widgets
     * !!! \warning
     *     Only call this function between calls `begin_xxx` and `end`
     * !!! \warning
     *     Do not keep the returned command buffer pointer around it is only valid until `end`
     *
     * ```c
     * struct command_buffer* window_get_canvas(struct context *ctx);
     * ```
     *
     * Parameter   | Description
     * ------------|-----------------------------------------------------------
     * \param[in] ctx     | Must point to an previously initialized `context` struct
     *
     * \returns a pointer to window internal `command_buffer` struct used as
     * drawing canvas. Can be used to do custom drawing.
     */
    NK_API struct command_buffer* window_get_canvas(const struct context* ctx);

    /**
     * # # window_get_scroll
     * Gets the scroll offset for the current window
     * !!! \warning
     *     Only call this function between calls `begin_xxx` and `end`
     *
     * ```c
     * void window_get_scroll(struct context *ctx, std::uint32_t *offset_x, std::uint32_t *offset_y);
     * ```
     *
     * Parameter    | Description
     * -------------|-----------------------------------------------------------
     * \param[in] ctx      | Must point to an previously initialized `context` struct
     * \param[in] offset_x | A pointer to the x offset output (or NULL to ignore)
     * \param[in] offset_y | A pointer to the y offset output (or NULL to ignore)

     */
    NK_API void window_get_scroll(const context *ctx, unsigned int *offset_x, unsigned int *offset_y);

    /**
     * # # window_has_focus
     * \returns if the currently processed window is currently active
     * !!! \warning
     *     Only call this function between calls `begin_xxx` and `end`
     * ```c
     * bool window_has_focus(const struct context *ctx);
     * ```
     *
     * Parameter   | Description
     * ------------|-----------------------------------------------------------
     * \param[in] ctx     | Must point to an previously initialized `context` struct
     *
     * \returns `false(0)` if current window is not active or `true(1)` if it is

     */
    NK_API bool window_has_focus(const context *ctx);

    /**
     * # # window_is_hovered
     * Return if the current window is being hovered
     * !!! \warning
     *     Only call this function between calls `begin_xxx` and `end`
     * ```c
     * bool window_is_hovered(struct context *ctx);
     * ```
     *
     * Parameter   | Description
     * ------------|-----------------------------------------------------------
     * \param[in] ctx     | Must point to an previously initialized `context` struct
     *
     * \returns `true(1)` if current window is hovered or `false(0)` otherwise

     */
    NK_API bool window_is_hovered(const context *ctx);

    /**
     * # # window_is_collapsed
     * \returns if the window with given name is currently minimized/collapsed
     * ```c
     * bool window_is_collapsed(struct context *ctx, const char *name);
     * ```
     *
     * Parameter   | Description
     * ------------|-----------------------------------------------------------
     * \param[in] ctx     | Must point to an previously initialized `context` struct
     * \param[in] name    | Identifier of window you want to check if it is collapsed
     *
     * \returns `true(1)` if current window is minimized and `false(0)` if window not
     * found or is not minimized

     */
    NK_API bool window_is_collapsed(const context *ctx, const char *name);

    /**
     * # # window_is_closed
     * \returns if the window with given name was closed by calling `close`
     * ```c
     * bool window_is_closed(struct context *ctx, const char *name);
     * ```
     *
     * Parameter   | Description
     * ------------|-----------------------------------------------------------
     * \param[in] ctx     | Must point to an previously initialized `context` struct
     * \param[in] name    | Identifier of window you want to check if it is closed
     *
     * \returns `true(1)` if current window was closed or `false(0)` window not found or not closed

     */
    NK_API bool window_is_closed(const context *ctx, const char* name);

    /**
     * # # window_is_hidden
     * \returns if the window with given name is hidden
     * ```c
     * bool window_is_hidden(struct context *ctx, const char *name);
     * ```
     *
     * Parameter   | Description
     * ------------|-----------------------------------------------------------
     * \param[in] ctx     | Must point to an previously initialized `context` struct
     * \param[in] name    | Identifier of window you want to check if it is hidden
     *
     * \returns `true(1)` if current window is hidden or `false(0)` window not found or visible

     */
    NK_API bool window_is_hidden(const context *ctx, const char* name);

    /**
     * # # window_is_active
     * Same as window_has_focus for some reason
     * ```c
     * bool window_is_active(struct context *ctx, const char *name);
     * ```
     *
     * Parameter   | Description
     * ------------|-----------------------------------------------------------
     * \param[in] ctx     | Must point to an previously initialized `context` struct
     * \param[in] name    | Identifier of window you want to check if it is active
     *
     * \returns `true(1)` if current window is active or `false(0)` window not found or not active
     */
    NK_API bool window_is_active(const context *ctx, const char* name);

    /**
     * # # window_is_any_hovered
     * \returns if the any window is being hovered
     * ```c
     * bool window_is_any_hovered(struct context*);
     * ```
     *
     * Parameter   | Description
     * ------------|-----------------------------------------------------------
     * \param[in] ctx     | Must point to an previously initialized `context` struct
     *
     * \returns `true(1)` if any window is hovered or `false(0)` otherwise
     */
    NK_API bool window_is_any_hovered(const context *ctx);

    /**
     * # # item_is_any_active
     * \returns if the any window is being hovered or any widget is currently active.
     * Can be used to decide if input should be processed by UI or your specific input handling.
     * Example could be UI and 3D camera to move inside a 3D space.
     * ```c
     * bool item_is_any_active(struct context*);
     * ```
     *
     * Parameter   | Description
     * ------------|-----------------------------------------------------------
     * \param[in] ctx     | Must point to an previously initialized `context` struct
     *
     * \returns `true(1)` if any window is hovered or any item is active or `false(0)` otherwise

     */
    NK_API bool item_is_any_active(const context *ctx);

    /**
     * # # window_set_bounds
     * Updates position and size of window with passed in name
     * ```c
     * void window_set_bounds(struct context*, const char *name, struct rect bounds);
     * ```
     *
     * Parameter   | Description
     * ------------|-----------------------------------------------------------
     * \param[in] ctx     | Must point to an previously initialized `context` struct
     * \param[in] name    | Identifier of the window to modify both position and size
     * \param[in] bounds  | Must point to a `rect` struct with the new position and size

     */
    NK_API void window_set_bounds(struct context *ctx, const char *name, struct rectf bounds);

    /**
     * # # window_set_position
     * Updates position of window with passed name
     * ```c
     * void window_set_position(struct context*, const char *name, struct vec2 pos);
     * ```
     *
     * Parameter   | Description
     * ------------|-----------------------------------------------------------
     * \param[in] ctx     | Must point to an previously initialized `context` struct
     * \param[in] name    | Identifier of the window to modify both position
     * \param[in] pos     | Must point to a `vec2` struct with the new position

     */
    NK_API void window_set_position(struct context *ctx, const char *name, struct vec2f pos);

    /**
     * # # window_set_size
     * Updates size of window with passed in name
     * ```c
     * void window_set_size(struct context*, const char *name, struct vec2);
     * ```
     *
     * Parameter   | Description
     * ------------|-----------------------------------------------------------
     * \param[in] ctx     | Must point to an previously initialized `context` struct
     * \param[in] name    | Identifier of the window to modify both window size
     * \param[in] size    | Must point to a `vec2` struct with new window size

     */
    NK_API void window_set_size(struct context *ctx, const char *name, struct vec2f size);

    /**
     * # # window_set_focus
     * Sets the window with given name as active
     * ```c
     * void window_set_focus(struct context*, const char *name);
     * ```
     *
     * Parameter   | Description
     * ------------|-----------------------------------------------------------
     * \param[in] ctx     | Must point to an previously initialized `context` struct
     * \param[in] name    | Identifier of the window to set focus on

     */
    NK_API void window_set_focus(struct context *ctx, const char *name);

    /**
     * # # window_set_scroll
     * Sets the scroll offset for the current window
     * !!! \warning
     *     Only call this function between calls `begin_xxx` and `end`
     *
     * ```c
     * void window_set_scroll(struct context *ctx, std::uint32_t offset_x, std::uint32_t offset_y);
     * ```
     *
     * Parameter    | Description
     * -------------|-----------------------------------------------------------
     * \param[in] ctx      | Must point to an previously initialized `context` struct
     * \param[in] offset_x | The x offset to scroll to
     * \param[in] offset_y | The y offset to scroll to

     */
    NK_API void window_set_scroll(struct context *ctx, std::uint32_t offset_x, std::uint32_t offset_y);

    /**
     * # # window_close
     * Closes a window and marks it for being freed at the end of the frame
     * ```c
     * void window_close(struct context *ctx, const char *name);
     * ```
     *
     * Parameter   | Description
     * ------------|-----------------------------------------------------------
     * \param[in] ctx     | Must point to an previously initialized `context` struct
     * \param[in] name    | Identifier of the window to close

     */
    NK_API void window_close(struct context *ctx, const char *name);

    /**
     * # # window_collapse
     * Updates collapse state of a window with given name
     * ```c
     * void window_collapse(struct context*, const char *name, enum collapse_states state);
     * ```
     *
     * Parameter   | Description
     * ------------|-----------------------------------------------------------
     * \param[in] ctx     | Must point to an previously initialized `context` struct
     * \param[in] name    | Identifier of the window to close
     * \param[in] state   | value out of collapse_states section

     */
    NK_API void window_collapse(struct context *ctx, const char *name, nk::collapse_states state);

    /**
     * # # window_collapse_if
     * Updates collapse state of a window with given name if given condition is met
     * ```c
     * void window_collapse_if(struct context*, const char *name, enum collapse_states, int cond);
     * ```
     *
     * Parameter   | Description
     * ------------|-----------------------------------------------------------
     * \param[in] ctx     | Must point to an previously initialized `context` struct
     * \param[in] name    | Identifier of the window to either collapse or maximize
     * \param[in] state   | value out of collapse_states section the window should be put into
     * \param[in] cond    | condition that has to be met to actually commit the collapse state change

     */
    NK_API void window_collapse_if(struct context *ctx, const char *name, nk::collapse_states state, int cond);

    /**
     * # # window_show
     * updates visibility state of a window with given name
     * ```c
     * void window_show(struct context*, const char *name, enum show_states);
     * ```
     *
     * Parameter   | Description
     * ------------|-----------------------------------------------------------
     * \param[in] ctx     | Must point to an previously initialized `context` struct
     * \param[in] name    | Identifier of the window to either collapse or maximize
     * \param[in] state   | state with either visible or hidden to modify the window with
     */
    NK_API void window_show(struct context *ctx, const char *name, nk::show_states state);

    /**
     * # # window_show_if
     * Updates visibility state of a window with given name if a given condition is met
     * ```c
     * void window_show_if(struct context*, const char *name, enum show_states, int cond);
     * ```
     *
     * Parameter   | Description
     * ------------|-----------------------------------------------------------
     * \param[in] ctx     | Must point to an previously initialized `context` struct
     * \param[in] name    | Identifier of the window to either hide or show
     * \param[in] state   | state with either visible or hidden to modify the window with
     * \param[in] cond    | condition that has to be met to actually commit the visibility state change

     */
    NK_API void window_show_if(struct context *ctx, const char *name, nk::show_states state, int cond);

    /**
     * # # window_show_if
     * Line for visual separation. Draws a line with thickness determined by the current row height.
     * ```c
     * void rule_horizontal(struct context *ctx, struct color color, bool rounding)
     * ```
     *
     * Parameter       | Description
     * ----------------|-------------------------------------------------------
     * \param[in] ctx         | Must point to an previously initialized `context` struct
     * \param[in] color       | Color of the horizontal line
     * \param[in] rounding    | Whether or not to make the line round
     */
    NK_API void rule_horizontal(struct context *ctx, struct color color, bool rounding);

    /* =============================================================================
     *
     *                                  LAYOUT
     *
     * =============================================================================*/
    /**
     * \page Layouting
     * Layouting in general describes placing widget inside a window with position and size.
     * While in this particular implementation there are five different APIs for layouting
     * each with different trade offs between control and ease of use. <br /><br />
     *
     * All layouting methods in this library are based around the concept of a row.
     * A row has a height the window content grows by and a number of columns and each
     * layouting method specifies how each widget is placed inside the row.
     * After a row has been allocated by calling a layouting functions and then
     * filled with widgets will advance an internal pointer over the allocated row. <br /><br />
     *
     * To actually define a layout you just call the appropriate layouting function
     * and each subsequent widget call will place the widget as specified. Important
     * here is that if you define more widgets then columns defined inside the layout
     * functions it will allocate the next row without you having to make another layouting <br /><br />
     * call.
     *
     * Biggest limitation with using all these APIs outside the `layout_space_xxx` API
     * is that you have to define the row height for each. However the row height
     * often depends on the height of the font. <br /><br />
     *
     * To fix that internally nuklear uses a minimum row height that is set to the
     * height plus padding of currently active font and overwrites the row height
     * value if zero. <br /><br />
     *
     * If you manually want to change the minimum row height then
     * use layout_set_min_row_height, and use layout_reset_min_row_height to
     * reset it back to be derived from font height. <br /><br />
     *
     * Also if you change the font in nuklear it will automatically change the minimum
     * row height for you and. This means if you change the font but still want
     * a minimum row height smaller than the font you have to repush your value. <br /><br />
     *
     * For actually more advanced UI I would even recommend using the `layout_space_xxx`
     * layouting method in combination with a cassowary constraint solver (there are
     * some versions on github with permissive license model) to take over all control over widget
     * layouting yourself. However for quick and dirty layouting using all the other layouting
     * functions should be fine.
     *
     * # Usage
     * 1.  __layout_row_dynamic__<br /><br />
     *     The easiest layouting function is `layout_row_dynamic`. It provides each
     *     widgets with same horizontal space inside the row and dynamically grows
     *     if the owning window grows in width. So the number of columns dictates
     *     the size of each widget dynamically by formula:
     *
     *     ```c
     *     widget_width = (window_width - padding - spacing) * (1/column_count)
     *     ```
     *
     *     Just like all other layouting APIs if you define more widget than columns this
     *     library will allocate a new row and keep all layouting parameters previously
     *     defined.
     *
     *     ```c
     *     if (begin_xxx(...) {
     *         // first row with height: 30 composed of two widgets
     *         layout_row_dynamic(&ctx, 30, 2);
     *         widget(...);
     *         widget(...);
     *         //
     *         // second row with same parameter as defined above
     *         widget(...);
     *         widget(...);
     *         //
     *         // third row uses 0 for height which will use auto layouting
     *         layout_row_dynamic(&ctx, 0, 2);
     *         widget(...);
     *         widget(...);
     *     }
     *     end(...);
     *     ```
     *
     * 2.  __layout_row_static__<br /><br />
     *     Another easy layouting function is `layout_row_static`. It provides each
     *     widget with same horizontal pixel width inside the row and does not grow
     *     if the owning window scales smaller or bigger.
     *
     *     ```c
     *     if (begin_xxx(...) {
     *         // first row with height: 30 composed of two widgets with width: 80
     *         layout_row_static(&ctx, 30, 80, 2);
     *         widget(...);
     *         widget(...);
     *         //
     *         // second row with same parameter as defined above
     *         widget(...);
     *         widget(...);
     *         //
     *         // third row uses 0 for height which will use auto layouting
     *         layout_row_static(&ctx, 0, 80, 2);
     *         widget(...);
     *         widget(...);
     *     }
     *     end(...);
     *     ```
     *
     * 3.  __layout_row_xxx__<br /><br />
     *     A little bit more advanced layouting API are functions `layout_row_begin`,
     *     `layout_row_push` and `layout_row_end`. They allow to directly
     *     specify each column pixel or window ratio in a row. It supports either
     *     directly setting per column pixel width or widget window ratio but not
     *     both. Furthermore it is a immediate mode API so each value is directly
     *     pushed before calling a widget. Therefore the layout is not automatically
     *     repeating like the last two layouting functions.
     *
     *     ```c
     *     if (begin_xxx(...) {
     *         // first row with height: 25 composed of two widgets with width 60 and 40
     *         layout_row_begin(ctx, NK_STATIC, 25, 2);
     *         layout_row_push(ctx, 60);
     *         widget(...);
     *         layout_row_push(ctx, 40);
     *         widget(...);
     *         layout_row_end(ctx);
     *         //
     *         // second row with height: 25 composed of two widgets with window ratio 0.25 and 0.75
     *         layout_row_begin(ctx, NK_DYNAMIC, 25, 2);
     *         layout_row_push(ctx, 0.25f);
     *         widget(...);
     *         layout_row_push(ctx, 0.75f);
     *         widget(...);
     *         layout_row_end(ctx);
     *         //
     *         // third row with auto generated height: composed of two widgets with window ratio 0.25 and 0.75
     *         layout_row_begin(ctx, NK_DYNAMIC, 0, 2);
     *         layout_row_push(ctx, 0.25f);
     *         widget(...);
     *         layout_row_push(ctx, 0.75f);
     *         widget(...);
     *         layout_row_end(ctx);
     *     }
     *     end(...);
     *     ```
     *
     * 4.  __layout_row__<br /><br />
     *     The array counterpart to API layout_row_xxx is the single layout_row
     *     functions. Instead of pushing either pixel or window ratio for every widget
     *     it allows to define it by array. The trade of for less control is that
     *     `layout_row` is automatically repeating. Otherwise the behavior is the
     *     same.
     *
     *     ```c
     *     if (begin_xxx(...) {
     *         // two rows with height: 30 composed of two widgets with width 60 and 40
     *         const float ratio[] = {60,40};
     *         layout_row(ctx, NK_STATIC, 30, 2, ratio);
     *         widget(...);
     *         widget(...);
     *         widget(...);
     *         widget(...);
     *         //
     *         // two rows with height: 30 composed of two widgets with window ratio 0.25 and 0.75
     *         const float ratio[] = {0.25, 0.75};
     *         layout_row(ctx, NK_DYNAMIC, 30, 2, ratio);
     *         widget(...);
     *         widget(...);
     *         widget(...);
     *         widget(...);
     *         //
     *         // two rows with auto generated height composed of two widgets with window ratio 0.25 and 0.75
     *         const float ratio[] = {0.25, 0.75};
     *         layout_row(ctx, NK_DYNAMIC, 30, 2, ratio);
     *         widget(...);
     *         widget(...);
     *         widget(...);
     *         widget(...);
     *     }
     *     end(...);
     *     ```
     *
     * 5.  __layout_row_template_xxx__<br /><br />
     *     The most complex and second most flexible API is a simplified flexbox version without
     *     line wrapping and weights for dynamic widgets. It is an immediate mode API but
     *     unlike `layout_row_xxx` it has auto repeat behavior and needs to be called
     *     before calling the templated widgets.
     *     The row template layout has three different per widget size specifier. The first
     *     one is the `layout_row_template_push_static`  with fixed widget pixel width.
     *     They do not grow if the row grows and will always stay the same.
     *     The second size specifier is `layout_row_template_push_variable`
     *     which defines a minimum widget size but it also can grow if more space is available
     *     not taken by other widgets.
     *     Finally there are dynamic widgets with `layout_row_template_push_dynamic`
     *     which are completely flexible and unlike variable widgets can even shrink
     *     to zero if not enough space is provided.
     *
     *     ```c
     *     if (begin_xxx(...) {
     *         // two rows with height: 30 composed of three widgets
     *         layout_row_template_begin(ctx, 30);
     *         layout_row_template_push_dynamic(ctx);
     *         layout_row_template_push_variable(ctx, 80);
     *         layout_row_template_push_static(ctx, 80);
     *         layout_row_template_end(ctx);
     *         //
     *         // first row
     *         widget(...); // dynamic widget can go to zero if not enough space
     *         widget(...); // variable widget with min 80 pixel but can grow bigger if enough space
     *         widget(...); // static widget with fixed 80 pixel width
     *         //
     *         // second row same layout
     *         widget(...);
     *         widget(...);
     *         widget(...);
     *     }
     *     end(...);
     *     ```
     *
     * 6.  __layout_space_xxx__<br /><br />
     *     Finally the most flexible API directly allows you to place widgets inside the
     *     window. The space layout API is an immediate mode API which does not support
     *     row auto repeat and directly sets position and size of a widget. Position
     *     and size hereby can be either specified as ratio of allocated space or
     *     allocated space local position and pixel size. Since this API is quite
     *     powerful there are a number of utility functions to get the available space
     *     and convert between local allocated space and screen space.
     *
     *     ```c
     *     if (begin_xxx(...) {
     *         // static row with height: 500 (you can set column count to INT_MAX if you don't want to be bothered)
     *         layout_space_begin(ctx, NK_STATIC, 500, INT_MAX);
     *         layout_space_push(ctx, rect(0,0,150,200));
     *         widget(...);
     *         layout_space_push(ctx, rect(200,200,100,200));
     *         widget(...);
     *         layout_space_end(ctx);
     *         //
     *         // dynamic row with height: 500 (you can set column count to INT_MAX if you don't want to be bothered)
     *         layout_space_begin(ctx, NK_DYNAMIC, 500, INT_MAX);
     *         layout_space_push(ctx, rect(0.5,0.5,0.1,0.1));
     *         widget(...);
     *         layout_space_push(ctx, rect(0.7,0.6,0.1,0.1));
     *         widget(...);
     *     }
     *     end(...);
     *     ```
     *
     * # Reference
     * Function                                     | Description
     * ---------------------------------------------|------------------------------------
     * \ref layout_set_min_row_height            | Set the currently used minimum row height to a specified value
     * \ref layout_reset_min_row_height          | Resets the currently used minimum row height to font height
     * \ref layout_widget_bounds                 | Calculates current width a static layout row can fit inside a window
     * \ref layout_ratio_from_pixel              | Utility functions to calculate window ratio from pixel size
     * \ref layout_row_dynamic                   | Current layout is divided into n same sized growing columns
     * \ref layout_row_static                    | Current layout is divided into n same fixed sized columns
     * \ref layout_row_begin                     | Starts a new row with given height and number of columns
     * \ref layout_row_push                      | Pushes another column with given size or window ratio
     * \ref layout_row_end                       | Finished previously started row
     * \ref layout_row                           | Specifies row columns in array as either window ratio or size
     * \ref layout_row_template_begin            | Begins the row template declaration
     * \ref layout_row_template_push_dynamic     | Adds a dynamic column that dynamically grows and can go to zero if not enough space
     * \ref layout_row_template_push_variable    | Adds a variable column that dynamically grows but does not shrink below specified pixel width
     * \ref layout_row_template_push_static      | Adds a static column that does not grow and will always have the same size
     * \ref layout_row_template_end              | Marks the end of the row template
     * \ref layout_space_begin                   | Begins a new layouting space that allows to specify each widgets position and size
     * \ref layout_space_push                    | Pushes position and size of the next widget in own coordinate space either as pixel or ratio
     * \ref layout_space_end                     | Marks the end of the layouting space
     * \ref layout_space_bounds                  | Callable after layout_space_begin and returns total space allocated
     * \ref layout_space_to_screen               | Converts vector from layout_space coordinate space into screen space
     * \ref layout_space_to_local                | Converts vector from screen space into layout_space coordinates
     * \ref layout_space_rect_to_screen          | Converts rectangle from layout_space coordinate space into screen space
     * \ref layout_space_rect_to_local           | Converts rectangle from screen space into layout_space coordinates
     */



    enum widget_align {
      NK_WIDGET_ALIGN_LEFT        = 0x01,
      NK_WIDGET_ALIGN_CENTERED    = 0x02,
      NK_WIDGET_ALIGN_RIGHT       = 0x04,
      NK_WIDGET_ALIGN_TOP         = 0x08,
      NK_WIDGET_ALIGN_MIDDLE      = 0x10,
      NK_WIDGET_ALIGN_BOTTOM      = 0x20
  };
    enum widget_alignment {
      NK_WIDGET_LEFT        = NK_WIDGET_ALIGN_MIDDLE|NK_WIDGET_ALIGN_LEFT,
      NK_WIDGET_CENTERED    = NK_WIDGET_ALIGN_MIDDLE|NK_WIDGET_ALIGN_CENTERED,
      NK_WIDGET_RIGHT       = NK_WIDGET_ALIGN_MIDDLE|NK_WIDGET_ALIGN_RIGHT
  };

    /**
     * Sets the currently used minimum row height.
     * !!! \warning
     *     The passed height needs to include both your preferred row height
     *     as well as padding. No internal padding is added.
     *
     * ```c
     * void layout_set_min_row_height(struct context*, float height);
     * ```
     *
     * \param[in] ctx     | Must point to an previously initialized `context` struct after call `begin_xxx`
     * \param[in] height  | New minimum row height to be used for auto generating the row height
     */
    NK_API void layout_set_min_row_height(struct context*, float height);

    /**
     * Reset the currently used minimum row height back to `font_height + text_padding + padding`
     * ```c
     * void layout_reset_min_row_height(struct context*);
     * ```
     *
     * \param[in] ctx     | Must point to an previously initialized `context` struct after call `begin_xxx`
     */
    NK_API void layout_reset_min_row_height(struct context*);

    /**
     * \brief Returns the width of the next row allocate by one of the layouting functions
     *
     * \details
     * ```c
     * struct rect layout_widget_bounds(struct context*);
     * ```
     *
     * \param[in] ctx     | Must point to an previously initialized `context` struct after call `begin_xxx`
     *
     * \return `rect` with both position and size of the next row
     */
    NK_API struct rectf layout_widget_bounds(const struct context *ctx);

    /**
     * \brief Utility functions to calculate window ratio from pixel size
     *
     * \details
     * ```c
     * float layout_ratio_from_pixel(struct context*, float pixel_width);
     * ```
     *
     * \param[in] ctx     | Must point to an previously initialized `context` struct after call `begin_xxx`
     * \param[in] pixel   | Pixel_width to convert to window ratio
     *
     * \returns `rect` with both position and size of the next row
     */
    NK_API float layout_ratio_from_pixel(const struct context *ctx, float pixel_width);

    /**
     * \brief Sets current row layout to share horizontal space
     * between @cols number of widgets evenly. Once called all subsequent widget
     * calls greater than @cols will allocate a new row with same layout.
     *
     * \details
     * ```c
     * void layout_row_dynamic(struct context *ctx, float height, int cols);
     * ```
     *
     * \param[in] ctx     | Must point to an previously initialized `context` struct after call `begin_xxx`
     * \param[in] height  | Holds height of each widget in row or zero for auto layouting
     * \param[in] columns | Number of widget inside row
     */
    NK_API void layout_row_dynamic(struct context *ctx, float height, int cols);

    /**
     * \brief Sets current row layout to fill @cols number of widgets
     * in row with same @item_width horizontal size. Once called all subsequent widget
     * calls greater than @cols will allocate a new row with same layout.
     *
     * \details
     * ```c
     * void layout_row_static(struct context *ctx, float height, int item_width, int cols);
     * ```
     *
     * \param[in] ctx     | Must point to an previously initialized `context` struct after call `begin_xxx`
     * \param[in] height  | Holds height of each widget in row or zero for auto layouting
     * \param[in] width   | Holds pixel width of each widget in the row
     * \param[in] columns | Number of widget inside row
     */
    NK_API void layout_row_static(struct context *ctx, float height, int item_width, int cols);

    /**
     * \brief Starts a new dynamic or fixed row with given height and columns.
     *
     * \details
     * ```c
     * void layout_row_begin(struct context *ctx, enum layout_format fmt, float row_height, int cols);
     * ```
     *
     * \param[in] ctx     | Must point to an previously initialized `context` struct after call `begin_xxx`
     * \param[in] fmt     | either `NK_DYNAMIC` for window ratio or `NK_STATIC` for fixed size columns
     * \param[in] height  | holds height of each widget in row or zero for auto layouting
     * \param[in] columns | Number of widget inside row
     */
    NK_API void layout_row_begin(struct context *ctx, nk::layout_format fmt, float row_height, int cols);

    /**
     * \breif Specifies either window ratio or width of a single column
     *
     * \details
     * ```c
     * void layout_row_push(struct context*, float value);
     * ```
     *
     * \param[in] ctx     | Must point to an previously initialized `context` struct after call `begin_xxx`
     * \param[in] value   | either a window ratio or fixed width depending on @fmt in previous `layout_row_begin` call
     */
    NK_API void layout_row_push(struct context*, float value);

    /**
     * \brief Finished previously started row
     *
     * \details
     * ```c
     * void layout_row_end(struct context*);
     * ```
     *
     * \param[in] ctx     | Must point to an previously initialized `context` struct after call `begin_xxx`
     */
    NK_API void layout_row_end(struct context*);

    /**
     * \brief Specifies row columns in array as either window ratio or size
     *
     * \details
     * ```c
     * void layout_row(struct context*, enum layout_format, float height, int cols, const float *ratio);
     * ```
     *
     * \param[in] ctx     | Must point to an previously initialized `context` struct after call `begin_xxx`
     * \param[in] fmt     | Either `NK_DYNAMIC` for window ratio or `NK_STATIC` for fixed size columns
     * \param[in] height  | Holds height of each widget in row or zero for auto layouting
     * \param[in] columns | Number of widget inside row
     */
    NK_API void layout_row(struct context*, nk::layout_format, float height, int cols, const float *ratio);

    /**
     * # # layout_row_template_begin
     * Begins the row template declaration
     * ```c
     * void layout_row_template_begin(struct context*, float row_height);
     * ```
     *
     * Parameter   | Description
     * ------------|-----------------------------------------------------------
     * \param[in] ctx     | Must point to an previously initialized `context` struct after call `begin_xxx`
     * \param[in] height  | Holds height of each widget in row or zero for auto layouting
     */
    NK_API void layout_row_template_begin(struct context*, float row_height);

    /**
     * # # layout_row_template_push_dynamic
     * Adds a dynamic column that dynamically grows and can go to zero if not enough space
     * ```c
     * void layout_row_template_push_dynamic(struct context*);
     * ```
     *
     * Parameter   | Description
     * ------------|-----------------------------------------------------------
     * \param[in] ctx     | Must point to an previously initialized `context` struct after call `begin_xxx`
     * \param[in] height  | Holds height of each widget in row or zero for auto layouting
     */
    NK_API void layout_row_template_push_dynamic(struct context*);

    /**
     * # # layout_row_template_push_variable
     * Adds a variable column that dynamically grows but does not shrink below specified pixel width
     * ```c
     * void layout_row_template_push_variable(struct context*, float min_width);
     * ```
     *
     * Parameter   | Description
     * ------------|-----------------------------------------------------------
     * \param[in] ctx     | Must point to an previously initialized `context` struct after call `begin_xxx`
     * \param[in] width   | Holds the minimum pixel width the next column must always be
     */
    NK_API void layout_row_template_push_variable(struct context*, float min_width);

    /**
     * # # layout_row_template_push_static
     * Adds a static column that does not grow and will always have the same size
     * ```c
     * void layout_row_template_push_static(struct context*, float width);
     * ```
     *
     * Parameter   | Description
     * ------------|-----------------------------------------------------------
     * \param[in] ctx     | Must point to an previously initialized `context` struct after call `begin_xxx`
     * \param[in] width   | Holds the absolute pixel width value the next column must be
     */
    NK_API void layout_row_template_push_static(struct context*, float width);

    /**
     * # # layout_row_template_end
     * Marks the end of the row template
     * ```c
     * void layout_row_template_end(struct context*);
     * ```
     *
     * Parameter   | Description
     * ------------|-----------------------------------------------------------
     * \param[in] ctx     | Must point to an previously initialized `context` struct after call `begin_xxx`
     */
    NK_API void layout_row_template_end(struct context*);

    /**
     * # # layout_space_begin
     * Begins a new layouting space that allows to specify each widgets position and size.
     * ```c
     * void layout_space_begin(struct context*, enum layout_format, float height, int widget_count);
     * ```
     *
     * Parameter   | Description
     * ------------|-----------------------------------------------------------
     * \param[in] ctx     | Must point to an previously initialized `context` struct after call `begin_xxx`
     * \param[in] fmt     | Either `NK_DYNAMIC` for window ratio or `NK_STATIC` for fixed size columns
     * \param[in] height  | Holds height of each widget in row or zero for auto layouting
     * \param[in] columns | Number of widgets inside row
     */
    NK_API void layout_space_begin(struct context*, nk::layout_format, float height, int widget_count);

    /**
     * # # layout_space_push
     * Pushes position and size of the next widget in own coordinate space either as pixel or ratio
     * ```c
     * void layout_space_push(struct context *ctx, struct rect bounds);
     * ```
     *
     * Parameter   | Description
     * ------------|-----------------------------------------------------------
     * \param[in] ctx     | Must point to an previously initialized `context` struct after call `layout_space_begin`
     * \param[in] bounds  | Position and size in laoyut space local coordinates
     */
    NK_API void layout_space_push(struct context*, struct rectf bounds);

    /**
     * # # layout_space_end
     * Marks the end of the layout space
     * ```c
     * void layout_space_end(struct context*);
     * ```
     *
     * Parameter   | Description
     * ------------|-----------------------------------------------------------
     * \param[in] ctx     | Must point to an previously initialized `context` struct after call `layout_space_begin`
     */
    NK_API void layout_space_end(struct context*);

    /**
     * # # layout_space_bounds
     * Utility function to calculate total space allocated for `layout_space`
     * ```c
     * struct rect layout_space_bounds(struct context*);
     * ```
     *
     * Parameter   | Description
     * ------------|-----------------------------------------------------------
     * \param[in] ctx     | Must point to an previously initialized `context` struct after call `layout_space_begin`
     *
     * \returns `rect` holding the total space allocated
     */
    NK_API struct rectf layout_space_bounds(const struct context *ctx);

    /**
     * # # layout_space_to_screen
     * Converts vector from layout_space coordinate space into screen space
     * ```c
     * struct vec2 layout_space_to_screen(struct context*, struct vec2);
     * ```
     *
     * Parameter   | Description
     * ------------|-----------------------------------------------------------
     * \param[in] ctx     | Must point to an previously initialized `context` struct after call `layout_space_begin`
     * \param[in] vec     | Position to convert from layout space into screen coordinate space
     *
     * \returns transformed `vec2` in screen space coordinates
     */
    NK_API struct vec2f layout_space_to_screen(const struct context* ctx, struct vec2f vec);

    /**
     * # # layout_space_to_local
     * Converts vector from layout space into screen space
     * ```c
     * struct vec2 layout_space_to_local(struct context*, struct vec2);
     * ```
     *
     * Parameter   | Description
     * ------------|-----------------------------------------------------------
     * \param[in] ctx     | Must point to an previously initialized `context` struct after call `layout_space_begin`
     * \param[in] vec     | Position to convert from screen space into layout coordinate space
     *
     * \returns transformed `vec2` in layout space coordinates
     */
    NK_API struct vec2f layout_space_to_local(const struct context *ctx, struct vec2f vec);

    /**
     * # # layout_space_rect_to_screen
     * Converts rectangle from screen space into layout space
     * ```c
     * struct rect layout_space_rect_to_screen(struct context*, struct rect);
     * ```
     *
     * Parameter   | Description
     * ------------|-----------------------------------------------------------
     * \param[in] ctx     | Must point to an previously initialized `context` struct after call `layout_space_begin`
     * \param[in] bounds  | Rectangle to convert from layout space into screen space
     *
     * \returns transformed `rect` in screen space coordinates
     */
    NK_API struct rectf layout_space_rect_to_screen(const struct context *ctx, struct rectf bounds);

    /**
     * # # layout_space_rect_to_local
     * Converts rectangle from layout space into screen space
     * ```c
     * struct rect layout_space_rect_to_local(struct context*, struct rect);
     * ```
     *
     * Parameter   | Description
     * ------------|-----------------------------------------------------------
     * \param[in] ctx     | Must point to an previously initialized `context` struct after call `layout_space_begin`
     * \param[in] bounds  | Rectangle to convert from layout space into screen space
     *
     * \returns transformed `rect` in layout space coordinates
     */
    NK_API struct rectf layout_space_rect_to_local(const struct context *ctx, struct rectf bounds);

    /**
     * # # spacer
     * Spacer is a dummy widget that consumes space as usual but doesn't draw anything
     * ```c
     * void spacer(struct context* );
     * ```
     *
     * Parameter   | Description
     * ------------|-----------------------------------------------------------
     * \param[in] ctx     | Must point to an previously initialized `context` struct after call `layout_space_begin`
     *
     */
    NK_API void spacer(struct context *ctx);


    /** =============================================================================
     *
     *                                  GROUP
     *
     * =============================================================================*/
    /**
     * \page Groups
     * Groups are basically windows inside windows. They allow to subdivide space
     * in a window to layout widgets as a group. Almost all more complex widget
     * layouting requirements can be solved using groups and basic layouting
     * fuctionality. Groups just like windows are identified by an unique name and
     * internally keep track of scrollbar offsets by default. However additional
     * versions are provided to directly manage the scrollbar.
     *
     * # Usage
     * To create a group you have to call one of the three `group_begin_xxx`
     * functions to start group declarations and `group_end` at the end. Furthermore it
     * is required to check the return value of `group_begin_xxx` and only process
     * widgets inside the window if the value is not 0.
     * Nesting groups is possible and even encouraged since many layouting schemes
     * can only be achieved by nesting. Groups, unlike windows, need `group_end`
     * to be only called if the corresponding `group_begin_xxx` call does not return 0:
     *
     * ```c
     * if (group_begin_xxx(ctx, ...) {
     *     // [... widgets ...]
     *     group_end(ctx);
     * }
     * ```
     *
     * In the grand concept groups can be called after starting a window
     * with `begin_xxx` and before calling `end`:
     *
     * ```c
     * struct context ctx;
     * init_xxx(&ctx, ...);
     * while (1) {
     *     // Input
     *     Event evt;
     *     input_begin(&ctx);
     *     while (GetEvent(&evt)) {
     *         if (evt.type == MOUSE_MOVE)
     *             input_motion(&ctx, evt.motion.x, evt.motion.y);
     *         else if (evt.type == [...]) {
     *             input_xxx(...);
     *         }
     *     }
     *     input_end(&ctx);
     *     //
     *     // Window
     *     if (begin_xxx(...) {
     *         // [...widgets...]
     *         layout_row_dynamic(...);
     *         if (group_begin_xxx(ctx, ...) {
     *             //[... widgets ...]
     *             group_end(ctx);
     *         }
     *     }
     *     end(ctx);
     *     //
     *     // Draw
     *     const struct command *cmd = 0;
     *     foreach(cmd, &ctx) {
     *     switch (cmd->type) {
     *     case NK_COMMAND_LINE:
     *         your_draw_line_function(...)
     *         break;
     *     case NK_COMMAND_RECT
     *         your_draw_rect_function(...)
     *         break;
     *     case ...:
     *         // [...]
     *     }
     *     clear(&ctx);
     * }
     * free(&ctx);
     * ```
     * # Reference
     * Function                        | Description
     * --------------------------------|-------------------------------------------
     * \ref group_begin                  | Start a new group with internal scrollbar handling
     * \ref group_begin_titled           | Start a new group with separated name and title and internal scrollbar handling
     * \ref group_end                    | Ends a group. Should only be called if group_begin returned non-zero
     * \ref group_scrolled_offset_begin  | Start a new group with manual separated handling of scrollbar x- and y-offset
     * \ref group_scrolled_begin         | Start a new group with manual scrollbar handling
     * \ref group_scrolled_end           | Ends a group with manual scrollbar handling. Should only be called if group_begin returned non-zero
     * \ref group_get_scroll             | Gets the scroll offset for the given group
     * \ref group_set_scroll             | Sets the scroll offset for the given group
     */

    /**
    * \brief Starts a new widget group. Requires a previous layouting function to specify a pos/size.
    * ```c
    * bool group_begin(struct context*, const char *title, flags);
    * ```
    *
    * Parameter   | Description
    * ------------|-----------------------------------------------------------
    * \param[in] ctx     | Must point to an previously initialized `context` struct
    * \param[in] title   | Must be an unique identifier for this group that is also used for the group header
    * \param[in] flags   | Window flags defined in the panel_flags section with a number of different group behaviors
    *
    * \returns `true(1)` if visible and fillable with widgets or `false(0)` otherwise
    */
    NK_API bool group_begin(struct context*, const char *title, flag);

    /**
    * \brief Starts a new widget group. Requires a previous layouting function to specify a pos/size.
    * ```c
    * bool group_begin_titled(struct context*, const char *name, const char *title, flags);
    * ```
    *
    * \param[in] ctx     | Must point to an previously initialized `context` struct
    * \param[in] id      | Must be an unique identifier for this group
    * \param[in] title   | Group header title
    * \param[in] flags   | Window flags defined in the panel_flags section with a number of different group behaviors
    *
    * \returns `true(1)` if visible and fillable with widgets or `false(0)` otherwise
    */
    NK_API bool group_begin_titled(struct context*, const char *name, const char *title, flag);

    /**
     * # # group_end
     * Ends a widget group
     * ```c
     * void group_end(struct context*);
     * ```
     *
     * Parameter   | Description
     * ------------|-----------------------------------------------------------
     * \param[in] ctx     | Must point to an previously initialized `context` struct
     */
    NK_API void group_end(struct context*);

    /**
     * # # group_scrolled_offset_begin
     * starts a new widget group. requires a previous layouting function to specify
     * a size. Does not keep track of scrollbar.
     * ```c
     * bool group_scrolled_offset_begin(struct context*, std::uint32_t *x_offset, std::uint32_t *y_offset, const char *title, flag flag);
     * ```
     *
     * Parameter   | Description
     * ------------|-----------------------------------------------------------
     * \param[in] ctx     | Must point to an previously initialized `context` struct
     * \param[in] x_offset| Scrollbar x-offset to offset all widgets inside the group horizontally.
     * \param[in] y_offset| Scrollbar y-offset to offset all widgets inside the group vertically
     * \param[in] title   | Window unique group title used to both identify and display in the group header
     * \param[in] flags   | Window flags from the panel_flags section
     *
     * \returns `true(1)` if visible and fillable with widgets or `false(0)` otherwise
     */
    NK_API bool group_scrolled_offset_begin(struct context*, std::uint32_t *x_offset, std::uint32_t *y_offset, const char *title, flag flags);

    /**
     * # # group_scrolled_begin
     * Starts a new widget group. requires a previous
     * layouting function to specify a size. Does not keep track of scrollbar.
     * ```c
     * bool group_scrolled_begin(struct context*, struct scroll *off, const char *title, flag);
     * ```
     *
     * Parameter   | Description
     * ------------|-----------------------------------------------------------
     * \param[in] ctx     | Must point to an previously initialized `context` struct
     * \param[in] off     | Both x- and y- scroll offset. Allows for manual scrollbar control
     * \param[in] title   | Window unique group title used to both identify and display in the group header
     * \param[in] flags   | Window flags from panel_flags section
     *
     * \returns `true(1)` if visible and fillable with widgets or `false(0)` otherwise
     */
    NK_API bool group_scrolled_begin(struct context*, struct scroll *off, const char *title, flag);

    /**
     * # # group_scrolled_end
     * Ends a widget group after calling group_scrolled_offset_begin or group_scrolled_begin.
     * ```c
     * void group_scrolled_end(struct context*);
     * ```
     *
     * Parameter   | Description
     * ------------|-----------------------------------------------------------
     * \param[in] ctx     | Must point to an previously initialized `context` struct
     */
    NK_API void group_scrolled_end(struct context*);

    /**
     * # # group_get_scroll
     * Gets the scroll position of the given group.
     * ```c
     * void group_get_scroll(struct context*, const char *id, std::uint32_t *x_offset, std::uint32_t *y_offset);
     * ```
     *
     * Parameter    | Description
     * -------------|-----------------------------------------------------------
     * \param[in] ctx      | Must point to an previously initialized `context` struct
     * \param[in] id       | The id of the group to get the scroll position of
     * \param[in] x_offset | A pointer to the x offset output (or NULL to ignore)
     * \param[in] y_offset | A pointer to the y offset output (or NULL to ignore)
     */
    NK_API void group_get_scroll(struct context*, const char *id, std::uint32_t *x_offset, std::uint32_t *y_offset);

    /**
     * # # group_set_scroll
     * Sets the scroll position of the given group.
     * ```c
     * void group_set_scroll(struct context*, const char *id, std::uint32_t x_offset, std::uint32_t y_offset);
     * ```
     *
     * Parameter    | Description
     * -------------|-----------------------------------------------------------
     * \param[in] ctx      | Must point to an previously initialized `context` struct
     * \param[in] id       | The id of the group to scroll
     * \param[in] x_offset | The x offset to scroll to
     * \param[in] y_offset | The y offset to scroll to
     */
    NK_API void group_set_scroll(struct context*, const char *id, std::uint32_t x_offset, std::uint32_t y_offset);

    /** =============================================================================
     *
     *                                  TREE
     *
     * =============================================================================*/
    /**
     * \page Tree
     * Trees represent two different concept. First the concept of a collapsible
     * UI section that can be either in a hidden or visible state. They allow the UI
     * user to selectively minimize the current set of visible UI to comprehend.
     * The second concept are tree widgets for visual UI representation of trees.<br /><br />
     *
     * Trees thereby can be nested for tree representations and multiple nested
     * collapsible UI sections. All trees are started by calling of the
     * `tree_xxx_push_tree` functions and ended by calling one of the
     * `tree_xxx_pop_xxx()` functions. Each starting functions takes a title label
     * and optionally an image to be displayed and the initial collapse state from
     * the collapse_states section.<br /><br />
     *
     * The runtime state of the tree is either stored outside the library by the caller
     * or inside which requires a unique ID. The unique ID can either be generated
     * automatically from `__FILE__` and `__LINE__` with function `tree_push`,
     * by `__FILE__` and a user provided ID generated for example by loop index with
     * function `tree_push_id` or completely provided from outside by user with
     * function `tree_push_hashed`.
     *
     * # Usage
     * To create a tree you have to call one of the seven `tree_xxx_push_xxx`
     * functions to start a collapsible UI section and `tree_xxx_pop` to mark the
     * end.
     * Each starting function will either return `false(0)` if the tree is collapsed
     * or hidden and therefore does not need to be filled with content or `true(1)`
     * if visible and required to be filled.
     *
     * !!! Note
     *     The tree header does not require and layouting function and instead
     *     calculates a auto height based on the currently used font size
     *
     * The tree ending functions only need to be called if the tree content is
     * actually visible. So make sure the tree push function is guarded by `if`
     * and the pop call is only taken if the tree is visible.
     *
     * ```c
     * if (tree_push(ctx, NK_TREE_TAB, "Tree", NK_MINIMIZED)) {
     *     layout_row_dynamic(...);
     *     widget(...);
     *     tree_pop(ctx);
     * }
     * ```
     *
     * # Reference
     * Function                    | Description
     * ----------------------------|-------------------------------------------
     * tree_push                | Start a collapsible UI section with internal state management
     * tree_push_id             | Start a collapsible UI section with internal state management callable in a look
     * tree_push_hashed         | Start a collapsible UI section with internal state management with full control over internal unique ID use to store state
     * tree_image_push          | Start a collapsible UI section with image and label header
     * tree_image_push_id       | Start a collapsible UI section with image and label header and internal state management callable in a look
     * tree_image_push_hashed   | Start a collapsible UI section with image and label header and internal state management with full control over internal unique ID use to store state
     * tree_pop                 | Ends a collapsible UI section
     * tree_state_push          | Start a collapsible UI section with external state management
     * tree_state_image_push    | Start a collapsible UI section with image and label header and external state management
     * tree_state_pop           | Ends a collapsabale UI section
     *
     * # tree_type
     * Flag            | Description
     * ----------------|----------------------------------------
     * NK_TREE_NODE    | Highlighted tree header to mark a collapsible UI section
     * NK_TREE_TAB     | Non-highlighted tree header closer to tree representations
     */

    /**
     * # # tree_push
     * Starts a collapsible UI section with internal state management
     * !!! \warning
     *     To keep track of the runtime tree collapsible state this function uses
     *     defines `__FILE__` and `__LINE__` to generate a unique ID. If you want
     *     to call this function in a loop please use `tree_push_id` or
     *     `tree_push_hashed` instead.
     *
     * ```c
     * #define tree_push(ctx, type, title, state)
     * ```
     *
     * Parameter   | Description
     * ------------|-----------------------------------------------------------
     * \param[in] ctx     | Must point to an previously initialized `context` struct
     * \param[in] type    | Value from the tree_type section to visually mark a tree node header as either a collapseable UI section or tree node
     * \param[in] title   | Label printed in the tree header
     * \param[in] state   | Initial tree state value out of collapse_states
     *
     * \returns `true(1)` if visible and fillable with widgets or `false(0)` otherwise
     */
  #define tree_push(ctx, type, title, state) tree_push_hashed(ctx, type, title, state, NK_FILE_LINE,strlen(NK_FILE_LINE),__LINE__)

    /**
     * # # tree_push_id
     * Starts a collapsible UI section with internal state management callable in a look
     * ```c
     * #define tree_push_id(ctx, type, title, state, id)
     * ```
     *
     * Parameter   | Description
     * ------------|-----------------------------------------------------------
     * \param[in] ctx     | Must point to an previously initialized `context` struct
     * \param[in] type    | Value from the tree_type section to visually mark a tree node header as either a collapseable UI section or tree node
     * \param[in] title   | Label printed in the tree header
     * \param[in] state   | Initial tree state value out of collapse_states
     * \param[in] id      | Loop counter index if this function is called in a loop
     *
     * \returns `true(1)` if visible and fillable with widgets or `false(0)` otherwise
     */
  #define tree_push_id(ctx, type, title, state, id) tree_push_hashed(ctx, type, title, state, NK_FILE_LINE,strlen(NK_FILE_LINE),id)

    /**
     * # # tree_push_hashed
     * Start a collapsible UI section with internal state management with full
     * control over internal unique ID used to store state
     * ```c
     * bool tree_push_hashed(struct context*, enum tree_type, const char *title, enum collapse_states initial_state, const char *hash, int len,int seed);
     * ```
     *
     * Parameter   | Description
     * ------------|-----------------------------------------------------------
     * \param[in] ctx     | Must point to an previously initialized `context` struct
     * \param[in] type    | Value from the tree_type section to visually mark a tree node header as either a collapseable UI section or tree node
     * \param[in] title   | Label printed in the tree header
     * \param[in] state   | Initial tree state value out of collapse_states
     * \param[in] hash    | Memory block or string to generate the ID from
     * \param[in] len     | Size of passed memory block or string in __hash__
     * \param[in] seed    | Seeding value if this function is called in a loop or default to `0`
     *
     * \returns `true(1)` if visible and fillable with widgets or `false(0)` otherwise
     */
    NK_API bool tree_push_hashed(struct context*, nk::tree_type, const char *title, nk::collapse_states initial_state, const char *hash, int len,int seed);

    /**
     * # # tree_image_push
     * Start a collapsible UI section with image and label header
     * !!! \warning
     *     To keep track of the runtime tree collapsible state this function uses
     *     defines `__FILE__` and `__LINE__` to generate a unique ID. If you want
     *     to call this function in a loop please use `tree_image_push_id` or
     *     `tree_image_push_hashed` instead.
     *
     * ```c
     * #define tree_image_push(ctx, type, img, title, state)
     * ```
     * Parameter   | Description
     * ------------|-----------------------------------------------------------
     * \param[in] ctx     | Must point to an previously initialized `context` struct
     * \param[in] type    | Value from the tree_type section to visually mark a tree node header as either a collapseable UI section or tree node
     * \param[in] img     | Image to display inside the header on the left of the label
     * \param[in] title   | Label printed in the tree header
     * \param[in] state   | Initial tree state value out of collapse_states
     *
     * \returns `true(1)` if visible and fillable with widgets or `false(0)` otherwise
     */
  #define tree_image_push(ctx, type, img, title, state) tree_image_push_hashed(ctx, type, img, title, state, NK_FILE_LINE,strlen(NK_FILE_LINE),__LINE__)

    /**
     * # # tree_image_push_id
     * Start a collapsible UI section with image and label header and internal state
     * management callable in a look
     *
     * ```c
     * #define tree_image_push_id(ctx, type, img, title, state, id)
     * ```
     *
     * Parameter   | Description
     * ------------|-----------------------------------------------------------
     * \param[in] ctx     | Must point to an previously initialized `context` struct
     * \param[in] type    | Value from the tree_type section to visually mark a tree node header as either a collapseable UI section or tree node
     * \param[in] img     | Image to display inside the header on the left of the label
     * \param[in] title   | Label printed in the tree header
     * \param[in] state   | Initial tree state value out of collapse_states
     * \param[in] id      | Loop counter index if this function is called in a loop
     *
     * \returns `true(1)` if visible and fillable with widgets or `false(0)` otherwise
     */
  #define tree_image_push_id(ctx, type, img, title, state, id) tree_image_push_hashed(ctx, type, img, title, state, NK_FILE_LINE,strlen(NK_FILE_LINE),id)

    /**
     * # # tree_image_push_hashed
     * Start a collapsible UI section with internal state management with full
     * control over internal unique ID used to store state
     * ```c
     * bool tree_image_push_hashed(struct context*, enum tree_type, struct image, const char *title, enum collapse_states initial_state, const char *hash, int len,int seed);
     * ```
     *
     * Parameter   | Description
     * ------------|-----------------------------------------------------------
     * \param[in] ctx     | Must point to an previously initialized `context` struct
     * \param[in] type    | Value from the tree_type section to visually mark a tree node header as either a collapseable UI section or tree node
     * \param[in] img     | Image to display inside the header on the left of the label
     * \param[in] title   | Label printed in the tree header
     * \param[in] state   | Initial tree state value out of collapse_states
     * \param[in] hash    | Memory block or string to generate the ID from
     * \param[in] len     | Size of passed memory block or string in __hash__
     * \param[in] seed    | Seeding value if this function is called in a loop or default to `0`
     *
     * \returns `true(1)` if visible and fillable with widgets or `false(0)` otherwise
     */
    NK_API bool tree_image_push_hashed(struct context*, nk::tree_type, struct image, const char *title, nk::collapse_states initial_state, const char *hash, int len,int seed);

    /**
     * # # tree_pop
     * Ends a collapsabale UI section
     * ```c
     * void tree_pop(struct context*);
     * ```
     *
     * Parameter   | Description
     * ------------|-----------------------------------------------------------
     * \param[in] ctx     | Must point to an previously initialized `context` struct after calling `tree_xxx_push_xxx`
     */
    NK_API void tree_pop(struct context*);

    /**
     * # # tree_state_push
     * Start a collapsible UI section with external state management
     * ```c
     * bool tree_state_push(struct context*, enum tree_type, const char *title, enum collapse_states *state);
     * ```
     *
     * Parameter   | Description
     * ------------|-----------------------------------------------------------
     * \param[in] ctx     | Must point to an previously initialized `context` struct after calling `tree_xxx_push_xxx`
     * \param[in] type    | Value from the tree_type section to visually mark a tree node header as either a collapseable UI section or tree node
     * \param[in] title   | Label printed in the tree header
     * \param[in] state   | Persistent state to update
     *
     * \returns `true(1)` if visible and fillable with widgets or `false(0)` otherwise
     */
    NK_API bool tree_state_push(struct context*, nk::tree_type, const char *title, nk::collapse_states *state);

    /**
     * # # tree_state_image_push
     * Start a collapsible UI section with image and label header and external state management
     * ```c
     * bool tree_state_image_push(struct context*, enum tree_type, struct image, const char *title, enum collapse_states *state);
     * ```
     *
     * Parameter   | Description
     * ------------|-----------------------------------------------------------
     * \param[in] ctx     | Must point to an previously initialized `context` struct after calling `tree_xxx_push_xxx`
     * \param[in] img     | Image to display inside the header on the left of the label
     * \param[in] type    | Value from the tree_type section to visually mark a tree node header as either a collapseable UI section or tree node
     * \param[in] title   | Label printed in the tree header
     * \param[in] state   | Persistent state to update
     *
     * \returns `true(1)` if visible and fillable with widgets or `false(0)` otherwise
     */
    NK_API bool tree_state_image_push(struct context*, nk::tree_type, struct image, const char *title, nk::collapse_states *state);

    /**
     * # # tree_state_pop
     * Ends a collapsabale UI section
     * ```c
     * void tree_state_pop(struct context*);
     * ```
     *
     * Parameter   | Description
     * ------------|-----------------------------------------------------------
     * \param[in] ctx     | Must point to an previously initialized `context` struct after calling `tree_xxx_push_xxx`
     */
    NK_API void tree_state_pop(struct context*);

  #define tree_element_push(ctx, type, title, state, sel) tree_element_push_hashed(ctx, type, title, state, sel, NK_FILE_LINE,strlen(NK_FILE_LINE),__LINE__)
  #define tree_element_push_id(ctx, type, title, state, sel, id) tree_element_push_hashed(ctx, type, title, state, sel, NK_FILE_LINE,strlen(NK_FILE_LINE),id)
    NK_API bool tree_element_push_hashed(struct context*, nk::tree_type, const char *title, nk::collapse_states initial_state, bool *selected, const char *hash, int len, int seed);
    NK_API bool tree_element_image_push_hashed(struct context*, nk::tree_type, struct image, const char *title, nk::collapse_states initial_state, bool *selected, const char *hash, int len,int seed);
    NK_API void tree_element_pop(struct context*);

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
      struct context *ctx;
      std::uint32_t *scroll_pointer;
      std::uint32_t scroll_value;
    };
    NK_API bool list_view_begin(struct context*, struct list_view *out, const char *id, flag, int row_height, int row_count);
    NK_API void list_view_end(struct list_view*);
    /* =============================================================================
     *
     *                                  WIDGET
     *
     * ============================================================================= */
    enum widget_layout_states {
      NK_WIDGET_INVALID, /**< The widget cannot be seen and is completely out of view */
      NK_WIDGET_VALID,   /**< The widget is completely inside the window and can be updated and drawn */
      NK_WIDGET_ROM,     /**< The widget is partially visible and cannot be updated */
      NK_WIDGET_DISABLED /**< The widget is manually disabled and acts like NK_WIDGET_ROM */
  };
    enum widget_states {
      NK_WIDGET_STATE_MODIFIED    = NK_FLAG(1),
      NK_WIDGET_STATE_INACTIVE    = NK_FLAG(2), /**!< widget is neither active nor hovered */
      NK_WIDGET_STATE_ENTERED     = NK_FLAG(3), /**!< widget has been hovered on the current frame */
      NK_WIDGET_STATE_HOVER       = NK_FLAG(4), /**!< widget is being hovered */
      NK_WIDGET_STATE_ACTIVED     = NK_FLAG(5),/**!< widget is currently activated */
      NK_WIDGET_STATE_LEFT        = NK_FLAG(6), /**!< widget is from this frame on not hovered anymore */
      NK_WIDGET_STATE_HOVERED     = NK_WIDGET_STATE_HOVER|NK_WIDGET_STATE_MODIFIED, /**!< widget is being hovered */
      NK_WIDGET_STATE_ACTIVE      = NK_WIDGET_STATE_ACTIVED|NK_WIDGET_STATE_MODIFIED /**!< widget is currently activated */
  };
    NK_API enum widget_layout_states widget(struct rectf*, const struct context*);
    NK_API enum widget_layout_states widget_fitting(struct rectf*, const struct context*, struct vec2f);
    NK_API struct rectf widget_bounds(const struct context*);
    NK_API struct vec2f widget_position(const struct context*);
    NK_API struct vec2f widget_size(const struct context*);
    NK_API float widget_width(const struct context*);
    NK_API float widget_height(const struct context*);
    NK_API bool widget_is_hovered(const struct context*);
    NK_API bool widget_is_mouse_clicked(const struct context*, enum buttons);
    NK_API bool widget_has_mouse_click_down(const struct context*, enum buttons, bool down);
    NK_API void spacing(struct context*, int cols);
    NK_API void widget_disable_begin(struct context* ctx);
    NK_API void widget_disable_end(struct context* ctx);
    /* =============================================================================
     *
     *                                  TEXT
     *
     * ============================================================================= */
    enum text_align {
      NK_TEXT_ALIGN_LEFT        = 0x01,
      NK_TEXT_ALIGN_CENTERED    = 0x02,
      NK_TEXT_ALIGN_RIGHT       = 0x04,
      NK_TEXT_ALIGN_TOP         = 0x08,
      NK_TEXT_ALIGN_MIDDLE      = 0x10,
      NK_TEXT_ALIGN_BOTTOM      = 0x20
  };
    enum text_alignment {
      NK_TEXT_LEFT        = NK_TEXT_ALIGN_MIDDLE|NK_TEXT_ALIGN_LEFT,
      NK_TEXT_CENTERED    = NK_TEXT_ALIGN_MIDDLE|NK_TEXT_ALIGN_CENTERED,
      NK_TEXT_RIGHT       = NK_TEXT_ALIGN_MIDDLE|NK_TEXT_ALIGN_RIGHT
  };
    NK_API void text_string(struct context*, const char*, int, flag);
    NK_API void text_colored(struct context*, const char*, int, flag, struct color);
    NK_API void text_wrap(struct context*, const char*, int);
    NK_API void text_wrap_colored(struct context*, const char*, int, struct color);
    NK_API void label(struct context*, const char*, flag align);
    NK_API void label_colored(struct context*, const char*, flag align, struct color);
    NK_API void label_wrap(struct context*, const char*);
    NK_API void label_colored_wrap(struct context*, const char*, struct color);
    NK_API void image(struct context*, struct image);
    NK_API void image_color(struct context*, struct image, struct color);
  #ifdef NK_INCLUDE_STANDARD_VARARGS
    NK_API void labelf(struct context*, flags, NK_PRINTF_FORMAT_STRING const char*, ...) NK_PRINTF_VARARG_FUNC(3);
    NK_API void labelf_colored(struct context*, flags, struct color, NK_PRINTF_FORMAT_STRING const char*,...) NK_PRINTF_VARARG_FUNC(4);
    NK_API void labelf_wrap(struct context*, NK_PRINTF_FORMAT_STRING const char*,...) NK_PRINTF_VARARG_FUNC(2);
    NK_API void labelf_colored_wrap(struct context*, struct color, NK_PRINTF_FORMAT_STRING const char*,...) NK_PRINTF_VARARG_FUNC(3);
    NK_API void labelfv(struct context*, flags, NK_PRINTF_FORMAT_STRING const char*, va_list) NK_PRINTF_VALIST_FUNC(3);
    NK_API void labelfv_colored(struct context*, flags, struct color, NK_PRINTF_FORMAT_STRING const char*, va_list) NK_PRINTF_VALIST_FUNC(4);
    NK_API void labelfv_wrap(struct context*, NK_PRINTF_FORMAT_STRING const char*, va_list) NK_PRINTF_VALIST_FUNC(2);
    NK_API void labelfv_colored_wrap(struct context*, struct color, NK_PRINTF_FORMAT_STRING const char*, va_list) NK_PRINTF_VALIST_FUNC(3);
    NK_API void value_bool(struct context*, const char *prefix, int);
    NK_API void value_int(struct context*, const char *prefix, int);
    NK_API void value_uint(struct context*, const char *prefix, unsigned int);
    NK_API void value_float(struct context*, const char *prefix, float);
    NK_API void value_color_byte(struct context*, const char *prefix, struct color);
    NK_API void value_color_float(struct context*, const char *prefix, struct color);
    NK_API void value_color_hex(struct context*, const char *prefix, struct color);
  #endif
    /* =============================================================================
     *
     *                                  BUTTON
     *
     * ============================================================================= */
    NK_API bool button_text(struct context*, const char *title, int len);
    NK_API bool button_label(struct context*, const char *title);
    NK_API bool button_color(struct context*, struct color);
    NK_API bool button_symbol(struct context*, enum symbol_type);
    NK_API bool button_image(struct context*, struct image img);
    NK_API bool button_symbol_label(struct context*, enum symbol_type, const char*, flag text_alignment);
    NK_API bool button_symbol_text(struct context*, enum symbol_type, const char*, int, flag alignment);
    NK_API bool button_image_label(struct context*, struct image img, const char*, flag text_alignment);
    NK_API bool button_image_text(struct context*, struct image img, const char*, int, flag alignment);
    NK_API bool button_text_styled(struct context*, const struct style_button*, const char *title, int len);
    NK_API bool button_label_styled(struct context*, const struct style_button*, const char *title);
    NK_API bool button_symbol_styled(struct context*, const struct style_button*, enum symbol_type);
    NK_API bool button_image_styled(struct context*, const struct style_button*, struct image img);
    NK_API bool button_symbol_text_styled(struct context*,const struct style_button*, enum symbol_type, const char*, int, flag alignment);
    NK_API bool button_symbol_label_styled(struct context *ctx, const struct style_button *style, enum symbol_type symbol, const char *title, flag align);
    NK_API bool button_image_label_styled(struct context*,const struct style_button*, struct image img, const char*, flag text_alignment);
    NK_API bool button_image_text_styled(struct context*,const struct style_button*, struct image img, const char*, int, flag alignment);
    NK_API void button_set_behavior(struct context*, nk::btn_behavior);
    NK_API bool button_push_behavior(struct context*, nk::btn_behavior);
    NK_API bool button_pop_behavior(struct context*);
    /* =============================================================================
     *
     *                                  CHECKBOX
     *
     * ============================================================================= */
    NK_API bool check_label(struct context*, const char*, bool active);
    NK_API bool check_text(struct context*, const char*, int, bool active);
    NK_API bool check_text_align(struct context*, const char*, int, bool active, flag widget_alignment, flag text_alignment);
    NK_API unsigned check_flags_label(struct context*, const char*, unsigned int flags, unsigned int value);
    NK_API unsigned check_flags_text(struct context*, const char*, int, unsigned int flags, unsigned int value);
    NK_API bool checkbox_label(struct context*, const char*, bool *active);
    NK_API bool checkbox_label_align(struct context *ctx, const char *label, bool *active, flag widget_alignment, flag text_alignment);
    NK_API bool checkbox_text(struct context*, const char*, int, bool *active);
    NK_API bool checkbox_text_align(struct context *ctx, const char *text, int len, bool *active, flag widget_alignment, flag text_alignment);
    NK_API bool checkbox_flags_label(struct context*, const char*, unsigned int *flags, unsigned int value);
    NK_API bool checkbox_flags_text(struct context*, const char*, int, unsigned int *flags, unsigned int value);
    /* =============================================================================
     *
     *                                  RADIO BUTTON
     *
     * ============================================================================= */
    NK_API bool radio_label(struct context*, const char*, bool *active);
    NK_API bool radio_label_align(struct context *ctx, const char *label, bool *active, flag widget_alignment, flag text_alignment);
    NK_API bool radio_text(struct context*, const char*, int, bool *active);
    NK_API bool radio_text_align(struct context *ctx, const char *text, int len, bool *active, flag widget_alignment, flag text_alignment);
    NK_API bool option_label(struct context*, const char*, bool active);
    NK_API bool option_label_align(struct context *ctx, const char *label, bool active, flag widget_alignment, flag text_alignment);
    NK_API bool option_text(struct context*, const char*, int, bool active);
    NK_API bool option_text_align(struct context *ctx, const char *text, int len, bool is_active, flag widget_alignment, flag text_alignment);
    /* =============================================================================
     *
     *                                  SELECTABLE
     *
     * ============================================================================= */
    NK_API bool selectable_label(struct context*, const char*, flag align, bool *value);
    NK_API bool selectable_text(struct context*, const char*, int, flag align, bool *value);
    NK_API bool selectable_image_label(struct context*,struct image,  const char*, flag align, bool *value);
    NK_API bool selectable_image_text(struct context*,struct image, const char*, int, flag align, bool *value);
    NK_API bool selectable_symbol_label(struct context*,enum symbol_type,  const char*, flag align, bool *value);
    NK_API bool selectable_symbol_text(struct context*,enum symbol_type, const char*, int, flag align, bool *value);

    NK_API bool select_label(struct context*, const char*, flag align, bool value);
    NK_API bool select_text(struct context*, const char*, int, flag align, bool value);
    NK_API bool select_image_label(struct context*, struct image,const char*, flag align, bool value);
    NK_API bool select_image_text(struct context*, struct image,const char*, int, flag align, bool value);
    NK_API bool select_symbol_label(struct context*,enum symbol_type,  const char*, flag align, bool value);
    NK_API bool select_symbol_text(struct context*,enum symbol_type, const char*, int, flag align, bool value);

    /* =============================================================================
     *
     *                                  SLIDER
     *
     * ============================================================================= */
    NK_API float slide_float(struct context*, float min, float val, float max, float step);
    NK_API int slide_int(struct context*, int min, int val, int max, int step);
    NK_API bool slider_float(struct context*, float min, float *val, float max, float step);
    NK_API bool slider_int(struct context*, int min, int *val, int max, int step);

    /* =============================================================================
     *
     *                                   KNOB
     *
     * ============================================================================= */
    NK_API bool knob_float(struct context*, float min, float *val, float max, float step, nk::heading zero_direction, float dead_zone_degrees);
    NK_API bool knob_int(struct context*, int min, int *val, int max, int step, nk::heading zero_direction, float dead_zone_degrees);

    /* =============================================================================
     *
     *                                  PROGRESSBAR
     *
     * ============================================================================= */
    NK_API bool progress(struct context*, std::size_t *cur, std::size_t max, bool modifyable);
    NK_API std::size_t prog(struct context*, std::size_t cur, std::size_t max, bool modifyable);

    /* =============================================================================
     *
     *                                  COLOR PICKER
     *
     * ============================================================================= */
    NK_API struct colorf color_picker(struct context*, struct colorf, nk::color_format);
    NK_API bool color_pick(struct context*, struct colorf*, nk::color_format);
    /* =============================================================================
     *
     *                                  PROPERTIES
     *
     * =============================================================================*/
    /**
     * \page Properties
     * Properties are the main value modification widgets in Nuklear. Changing a value
     * can be achieved by dragging, adding/removing incremental steps on button click
     * or by directly typing a number.
     *
     * # Usage
     * Each property requires a unique name for identification that is also used for
     * displaying a label. If you want to use the same name multiple times make sure
     * add a '#' before your name. The '#' will not be shown but will generate a
     * unique ID. Each property also takes in a minimum and maximum value. If you want
     * to make use of the complete number range of a type just use the provided
     * type limits from `limits.h`. For example `INT_MIN` and `INT_MAX` for
     * `property_int` and `propertyi`. In additional each property takes in
     * a increment value that will be added or subtracted if either the increment
     * decrement button is clicked. Finally there is a value for increment per pixel
     * dragged that is added or subtracted from the value.
     *
     * ```c
     * int value = 0;
     * struct context ctx;
     * init_xxx(&ctx, ...);
     * while (1) {
     *     // Input
     *     Event evt;
     *     input_begin(&ctx);
     *     while (GetEvent(&evt)) {
     *         if (evt.type == MOUSE_MOVE)
     *             input_motion(&ctx, evt.motion.x, evt.motion.y);
     *         else if (evt.type == [...]) {
     *             input_xxx(...);
     *         }
     *     }
     *     input_end(&ctx);
     *     //
     *     // Window
     *     if (begin_xxx(...) {
     *         // Property
     *         layout_row_dynamic(...);
     *         property_int(ctx, "ID", INT_MIN, &value, INT_MAX, 1, 1);
     *     }
     *     end(ctx);
     *     //
     *     // Draw
     *     const struct command *cmd = 0;
     *     foreach(cmd, &ctx) {
     *     switch (cmd->type) {
     *     case NK_COMMAND_LINE:
     *         your_draw_line_function(...)
     *         break;
     *     case NK_COMMAND_RECT
     *         your_draw_rect_function(...)
     *         break;
     *     case ...:
     *         // [...]
     *     }
     *     clear(&ctx);
     * }
     * free(&ctx);
     * ```
     *
     * # Reference
     * Function            | Description
     * --------------------|-------------------------------------------
     * \ref property_int     | Integer property directly modifying a passed in value
     * \ref property_float   | Float property directly modifying a passed in value
     * \ref property_double  | Double property directly modifying a passed in value
     * \ref propertyi        | Integer property returning the modified int value
     * \ref propertyf        | Float property returning the modified float value
     * \ref propertyd        | Double property returning the modified double value
     *

     * # # property_int
     * Integer property directly modifying a passed in value
     * !!! \warning
     *     To generate a unique property ID using the same label make sure to insert
     *     a `#` at the beginning. It will not be shown but guarantees correct behavior.
     *
     * ```c
     * void property_int(struct context *ctx, const char *name, int min, int *val, int max, int step, float inc_per_pixel);
     * ```
     *
     * Parameter           | Description
     * --------------------|-----------------------------------------------------------
     * \param[in] ctx             | Must point to an previously initialized `context` struct after calling a layouting function
     * \param[in] name            | String used both as a label as well as a unique identifier
     * \param[in] min             | Minimum value not allowed to be underflown
     * \param[in] val             | Integer pointer to be modified
     * \param[in] max             | Maximum value not allowed to be overflown
     * \param[in] step            | Increment added and subtracted on increment and decrement button
     * \param[in] inc_per_pixel   | Value per pixel added or subtracted on dragging
     */
    NK_API void property_int(struct context*, const char *name, int min, int *val, int max, int step, float inc_per_pixel);

    /**
     * # # property_float
     * Float property directly modifying a passed in value
     * !!! \warning
     *     To generate a unique property ID using the same label make sure to insert
     *     a `#` at the beginning. It will not be shown but guarantees correct behavior.
     *
     * ```c
     * void property_float(struct context *ctx, const char *name, float min, float *val, float max, float step, float inc_per_pixel);
     * ```
     *
     * Parameter           | Description
     * --------------------|-----------------------------------------------------------
     * \param[in] ctx             | Must point to an previously initialized `context` struct after calling a layouting function
     * \param[in] name            | String used both as a label as well as a unique identifier
     * \param[in] min             | Minimum value not allowed to be underflown
     * \param[in] val             | Float pointer to be modified
     * \param[in] max             | Maximum value not allowed to be overflown
     * \param[in] step            | Increment added and subtracted on increment and decrement button
     * \param[in] inc_per_pixel   | Value per pixel added or subtracted on dragging
     */
    NK_API void property_float(struct context*, const char *name, float min, float *val, float max, float step, float inc_per_pixel);

    /**
     * # # property_double
     * Double property directly modifying a passed in value
     * !!! \warning
     *     To generate a unique property ID using the same label make sure to insert
     *     a `#` at the beginning. It will not be shown but guarantees correct behavior.
     *
     * ```c
     * void property_double(struct context *ctx, const char *name, double min, double *val, double max, double step, double inc_per_pixel);
     * ```
     *
     * Parameter           | Description
     * --------------------|-----------------------------------------------------------
     * \param[in] ctx             | Must point to an previously initialized `context` struct after calling a layouting function
     * \param[in] name            | String used both as a label as well as a unique identifier
     * \param[in] min             | Minimum value not allowed to be underflown
     * \param[in] val             | Double pointer to be modified
     * \param[in] max             | Maximum value not allowed to be overflown
     * \param[in] step            | Increment added and subtracted on increment and decrement button
     * \param[in] inc_per_pixel   | Value per pixel added or subtracted on dragging
     */
    NK_API void property_double(struct context*, const char *name, double min, double *val, double max, double step, float inc_per_pixel);

    /**
     * # # propertyi
     * Integer property modifying a passed in value and returning the new value
     * !!! \warning
     *     To generate a unique property ID using the same label make sure to insert
     *     a `#` at the beginning. It will not be shown but guarantees correct behavior.
     *
     * ```c
     * int propertyi(struct context *ctx, const char *name, int min, int val, int max, int step, float inc_per_pixel);
     * ```
     *
     * \param[in] ctx              Must point to an previously initialized `context` struct after calling a layouting function
     * \param[in] name             String used both as a label as well as a unique identifier
     * \param[in] min              Minimum value not allowed to be underflown
     * \param[in] val              Current integer value to be modified and returned
     * \param[in] max              Maximum value not allowed to be overflown
     * \param[in] step             Increment added and subtracted on increment and decrement button
     * \param[in] inc_per_pixel    Value per pixel added or subtracted on dragging
     *
     * \returns the new modified integer value
     */
    NK_API int propertyi(struct context*, const char *name, int min, int val, int max, int step, float inc_per_pixel);

    /**
     * # # propertyf
     * Float property modifying a passed in value and returning the new value
     * !!! \warning
     *     To generate a unique property ID using the same label make sure to insert
     *     a `#` at the beginning. It will not be shown but guarantees correct behavior.
     *
     * ```c
     * float propertyf(struct context *ctx, const char *name, float min, float val, float max, float step, float inc_per_pixel);
     * ```
     *
     * \param[in] ctx              Must point to an previously initialized `context` struct after calling a layouting function
     * \param[in] name             String used both as a label as well as a unique identifier
     * \param[in] min              Minimum value not allowed to be underflown
     * \param[in] val              Current float value to be modified and returned
     * \param[in] max              Maximum value not allowed to be overflown
     * \param[in] step             Increment added and subtracted on increment and decrement button
     * \param[in] inc_per_pixel    Value per pixel added or subtracted on dragging
     *
     * \returns the new modified float value
     */
    NK_API float propertyf(struct context*, const char *name, float min, float val, float max, float step, float inc_per_pixel);

    /**
     * # # propertyd
     * Float property modifying a passed in value and returning the new value
     * !!! \warning
     *     To generate a unique property ID using the same label make sure to insert
     *     a `#` at the beginning. It will not be shown but guarantees correct behavior.
     *
     * ```c
     * float propertyd(struct context *ctx, const char *name, double min, double val, double max, double step, double inc_per_pixel);
     * ```
     *
     * \param[in] ctx              Must point to an previously initialized `context` struct after calling a layouting function
     * \param[in] name             String used both as a label as well as a unique identifier
     * \param[in] min              Minimum value not allowed to be underflown
     * \param[in] val              Current double value to be modified and returned
     * \param[in] max              Maximum value not allowed to be overflown
     * \param[in] step             Increment added and subtracted on increment and decrement button
     * \param[in] inc_per_pixel    Value per pixel added or subtracted on dragging
     *
     * \returns the new modified double value
     */
    NK_API double propertyd(struct context*, const char *name, double min, double val, double max, double step, float inc_per_pixel);

    /* =============================================================================
     *
     *                                  TEXT EDIT
     *
     * ============================================================================= */
    enum class edit_flags : std::uint32_t {
      EDIT_DEFAULT                 = 0,
      EDIT_READ_ONLY               = NK_FLAG(0),
      EDIT_AUTO_SELECT             = NK_FLAG(1),
      EDIT_SIG_ENTER               = NK_FLAG(2),
      EDIT_ALLOW_TAB               = NK_FLAG(3),
      EDIT_NO_CURSOR               = NK_FLAG(4),
      EDIT_SELECTABLE              = NK_FLAG(5),
      EDIT_CLIPBOARD               = NK_FLAG(6),
      EDIT_CTRL_ENTER_NEWLINE      = NK_FLAG(7),
      EDIT_NO_HORIZONTAL_SCROLL    = NK_FLAG(8),
      EDIT_ALWAYS_INSERT_MODE      = NK_FLAG(9),
      EDIT_MULTILINE               = NK_FLAG(10),
      EDIT_GOTO_END_ON_ACTIVATE    = NK_FLAG(11)
  };
    enum class edit_types {
      EDIT_SIMPLE  = std::to_underlying(edit_flags::EDIT_ALWAYS_INSERT_MODE),
      EDIT_FIELD   = EDIT_SIMPLE | std::to_underlying(edit_flags::EDIT_SELECTABLE | edit_flags::EDIT_CLIPBOARD),
      EDIT_BOX     = std::to_underlying(edit_flags::EDIT_ALWAYS_INSERT_MODE | edit_flags::EDIT_SELECTABLE | edit_flags::EDIT_MULTILINE | edit_flags::EDIT_ALLOW_TAB | edit_flags::EDIT_CLIPBOARD),
      EDIT_EDITOR  = std::to_underlying(edit_flags::EDIT_SELECTABLE | edit_flags::EDIT_MULTILINE | edit_flags::EDIT_ALLOW_TAB | edit_flags::EDIT_CLIPBOARD)
  };
    enum class edit_events {
      EDIT_ACTIVE      = NK_FLAG(0), /**!< edit widget is currently being modified */
      EDIT_INACTIVE    = NK_FLAG(1), /**!< edit widget is not active and is not being modified */
      EDIT_ACTIVATED   = NK_FLAG(2), /**!< edit widget went from state inactive to state active */
      EDIT_DEACTIVATED = NK_FLAG(3), /**!< edit widget went from state active to state inactive */
      EDIT_COMMITED    = NK_FLAG(4)  /**!< edit widget has received an enter and lost focus */
  };
    NK_API flag edit_string(struct context*, flag, char *buffer, int *len, int max, plugin_filter);
    NK_API flag edit_string_zero_terminated(struct context*, flag, char *buffer, int max, plugin_filter);
    NK_API flag edit_buffer(struct context*, flag, struct text_edit*, plugin_filter);
    NK_API void edit_focus(struct context*, flag flags);
    NK_API void edit_unfocus(struct context*);
    /* =============================================================================
     *
     *                                  CHART
     *
     * ============================================================================= */
    NK_API bool chart_begin(struct context*, nk::chart_type, int num, float min, float max);
    NK_API bool chart_begin_colored(struct context*, nk::chart_type, struct color, struct color active, int num, float min, float max);
    NK_API void chart_add_slot(struct context *ctx, const nk::chart_type, int count, float min_value, float max_value);
    NK_API void chart_add_slot_colored(struct context *ctx, const nk::chart_type, struct color, struct color active, int count, float min_value, float max_value);
    NK_API flag chart_push(struct context*, float);
    NK_API flag chart_push_slot(struct context*, float, int);
    NK_API void chart_end(struct context*);
    NK_API void plot(struct context*, nk::chart_type, const float *values, int count, int offset);
    NK_API void plot_function(struct context*, nk::chart_type, void *userdata, float(*value_getter)(void* user, int index), int count, int offset);
    /* =============================================================================
     *
     *                                  POPUP
     *
     * ============================================================================= */
    NK_API bool popup_begin(struct context*, nk::popup_type, const char*, flag, struct rectf bounds);
    NK_API void popup_close(struct context*);
    NK_API void popup_end(struct context*);
    NK_API void popup_get_scroll(const struct context*, std::uint32_t *offset_x, std::uint32_t *offset_y);
    NK_API void popup_set_scroll(struct context*, std::uint32_t offset_x, std::uint32_t offset_y);
    /* =============================================================================
     *
     *                                  COMBOBOX
     *
     * ============================================================================= */
    NK_API int combo(struct context*, const char *const *items, int count, int selected, int item_height, struct vec2f size);
    NK_API int combo_separator(struct context*, const char *items_separated_by_separator, int separator, int selected, int count, int item_height, struct vec2f size);
    NK_API int combo_string(struct context*, const char *items_separated_by_zeros, int selected, int count, int item_height, struct vec2f size);
    NK_API int combo_callback(struct context*, void(*item_getter)(void*, int, const char**), void *userdata, int selected, int count, int item_height, struct vec2f size);
    NK_API void combobox(struct context*, const char *const *items, int count, int *selected, int item_height, struct vec2f size);
    NK_API void combobox_string(struct context*, const char *items_separated_by_zeros, int *selected, int count, int item_height, struct vec2f size);
    NK_API void combobox_separator(struct context*, const char *items_separated_by_separator, int separator, int *selected, int count, int item_height, struct vec2f size);
    NK_API void combobox_callback(struct context*, void(*item_getter)(void*, int, const char**), void*, int *selected, int count, int item_height, struct vec2f size);
    /* =============================================================================
     *
     *                                  ABSTRACT COMBOBOX
     *
     * ============================================================================= */
    NK_API bool combo_begin_text(struct context*, const char *selected, int, struct vec2f size);
    NK_API bool combo_begin_label(struct context*, const char *selected, struct vec2f size);
    NK_API bool combo_begin_color(struct context*, struct color color, struct vec2f size);
    NK_API bool combo_begin_symbol(struct context*,  enum symbol_type,  struct vec2f size);
    NK_API bool combo_begin_symbol_label(struct context*, const char *selected, enum symbol_type, struct vec2f size);
    NK_API bool combo_begin_symbol_text(struct context*, const char *selected, int, enum symbol_type, struct vec2f size);
    NK_API bool combo_begin_image(struct context*, struct image img,  struct vec2f size);
    NK_API bool combo_begin_image_label(struct context*, const char *selected, struct image, struct vec2f size);
    NK_API bool combo_begin_image_text(struct context*,  const char *selected, int, struct image, struct vec2f size);
    NK_API bool combo_item_label(struct context*, const char*, flag alignment);
    NK_API bool combo_item_text(struct context*, const char*,int, flag alignment);
    NK_API bool combo_item_image_label(struct context*, struct image, const char*, flag alignment);
    NK_API bool combo_item_image_text(struct context*, struct image, const char*, int,flag alignment);
    NK_API bool combo_item_symbol_label(struct context*, enum symbol_type, const char*, flag alignment);
    NK_API bool combo_item_symbol_text(struct context*, enum symbol_type, const char*, int, flag alignment);
    NK_API void combo_close(struct context*);
    NK_API void combo_end(struct context*);
    /* =============================================================================
     *
     *                                  CONTEXTUAL
     *
     * ============================================================================= */
    NK_API bool contextual_begin(struct context*, flag, struct vec2f, struct rectf trigger_bounds);
    NK_API bool contextual_item_text(struct context*, const char*, int,flag align);
    NK_API bool contextual_item_label(struct context*, const char*, flag align);
    NK_API bool contextual_item_image_label(struct context*, struct image, const char*, flag alignment);
    NK_API bool contextual_item_image_text(struct context*, struct image, const char*, int len, flag alignment);
    NK_API bool contextual_item_symbol_label(struct context*, enum symbol_type, const char*, flag alignment);
    NK_API bool contextual_item_symbol_text(struct context*, enum symbol_type, const char*, int, flag alignment);
    NK_API void contextual_close(struct context*);
    NK_API void contextual_end(struct context*);
    /* =============================================================================
     *
     *                                  TOOLTIP
     *
     * ============================================================================= */
    NK_API void tooltip(struct context*, const char*);
  #ifdef NK_INCLUDE_STANDARD_VARARGS
    NK_API void tooltipf(struct context*, NK_PRINTF_FORMAT_STRING const char*, ...) NK_PRINTF_VARARG_FUNC(2);
    NK_API void tooltipfv(struct context*, NK_PRINTF_FORMAT_STRING const char*, va_list) NK_PRINTF_VALIST_FUNC(2);
  #endif
    NK_API bool tooltip_begin(struct context*, float width);
    NK_API void tooltip_end(struct context*);
    /* =============================================================================
     *
     *                                  MENU
     *
     * ============================================================================= */
    NK_API void menubar_begin(struct context*);
    NK_API void menubar_end(struct context*);
    NK_API bool menu_begin_text(struct context*, const char* title, int title_len, flag align, struct vec2f size);
    NK_API bool menu_begin_label(struct context*, const char*, flag align, struct vec2f size);
    NK_API bool menu_begin_image(struct context*, const char*, struct image, struct vec2f size);
    NK_API bool menu_begin_image_text(struct context*, const char*, int,flag align,struct image, struct vec2f size);
    NK_API bool menu_begin_image_label(struct context*, const char*, flag align,struct image, struct vec2f size);
    NK_API bool menu_begin_symbol(struct context*, const char*, enum symbol_type, struct vec2f size);
    NK_API bool menu_begin_symbol_text(struct context*, const char*, int,flag align,enum symbol_type, struct vec2f size);
    NK_API bool menu_begin_symbol_label(struct context*, const char*, flag align,enum symbol_type, struct vec2f size);
    NK_API bool menu_item_text(struct context*, const char*, int,flag align);
    NK_API bool menu_item_label(struct context*, const char*, flag alignment);
    NK_API bool menu_item_image_label(struct context*, struct image, const char*, flag alignment);
    NK_API bool menu_item_image_text(struct context*, struct image, const char*, int len, flag alignment);
    NK_API bool menu_item_symbol_text(struct context*, enum symbol_type, const char*, int, flag alignment);
    NK_API bool menu_item_symbol_label(struct context*, enum symbol_type, const char*, flag alignment);
    NK_API void menu_close(struct context*);
    NK_API void menu_end(struct context*);
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
    NK_API void style_default(struct context*);
    NK_API void style_from_table(struct context*, const struct color*);
    NK_API void style_load_cursor(struct context*, nk::style_cursor, const struct cursor*);
    NK_API void style_load_all_cursors(struct context*, const struct cursor*);
    NK_API const char* style_get_color_by_name(nk::style_colors);
    NK_API void style_set_font(struct context*, const struct user_font*);
    NK_API bool style_set_cursor(struct context*, nk::style_cursor);
    NK_API void style_show_cursor(struct context*);
    NK_API void style_hide_cursor(struct context*);

    NK_API bool style_push_font(struct context*, const struct user_font*);
    NK_API bool style_push_float(struct context*, float*, float);
    NK_API bool style_push_vec2(struct context*, struct vec2f*, struct vec2f);
    NK_API bool style_push_style_item(struct context*, struct style_item*, struct style_item);
    NK_API bool style_push_flags(struct context*, flag*, flag);
    NK_API bool style_push_color(struct context*, struct color*, struct color);

    NK_API bool style_pop_font(struct context*);
    NK_API bool style_pop_float(struct context*);
    NK_API bool style_pop_vec2(struct context*);
    NK_API bool style_pop_style_item(struct context*);
    NK_API bool style_pop_flags(struct context*);
    NK_API bool style_pop_color(struct context*);
    /* =============================================================================
     *
     *                                  COLOR
     *
     * ============================================================================= */
    NK_API struct color rgb(int r, int g, int b);
    NK_API struct color rgb_iv(const int *rgb);
    NK_API struct color rgb_bv(const std::byte* rgb);
    NK_API struct color rgb_f(float r, float g, float b);
    NK_API struct color rgb_fv(const float *rgb);
    NK_API struct color rgb_cf(struct colorf c);
    NK_API struct color rgb_hex(const char *rgb);
    NK_API struct color rgb_factor(struct color col, float factor);

    NK_API struct color rgba(int r, int g, int b, int a);
    NK_API struct color rgba_u32(std::uint32_t);
    NK_API struct color rgba_iv(const int *rgba);
    NK_API struct color rgba_bv(const std::byte *rgba);
    NK_API struct color rgba_f(float r, float g, float b, float a);
    NK_API struct color rgba_fv(const float *rgba);
    NK_API struct color rgba_cf(struct colorf c);
    NK_API struct color rgba_hex(const char *rgb);

    NK_API struct colorf hsva_colorf(float h, float s, float v, float a);
    NK_API struct colorf hsva_colorfv(const float *c);
    NK_API void colorf_hsva_f(float *out_h, float *out_s, float *out_v, float *out_a, struct colorf in);
    NK_API void colorf_hsva_fv(float *hsva, struct colorf in);

    NK_API struct color hsv(int h, int s, int v);
    NK_API struct color hsv_iv(const int *hsv);
    NK_API struct color hsv_bv(const std::byte *hsv);
    NK_API struct color hsv_f(float h, float s, float v);
    NK_API struct color hsv_fv(const float *hsv);

    NK_API struct color hsva(int h, int s, int v, int a);
    NK_API struct color hsva_iv(const int *hsva);
    NK_API struct color hsva_bv(const std::byte *hsva);
    NK_API struct color hsva_f(float h, float s, float v, float a);
    NK_API struct color hsva_fv(const float *hsva);

    /* color (conversion nuklear --> user) */
    NK_API void color_f(float *r, float *g, float *b, float *a, struct color);
    NK_API void color_fv(float *rgba_out, struct color);
    NK_API struct colorf color_cf(struct color);
    NK_API void color_d(double *r, double *g, double *b, double *a, struct color);
    NK_API void color_dv(double *rgba_out, struct color);

    NK_API std::uint32_t color_u32(struct color);
    NK_API void color_hex_rgba(char *output, struct color);
    NK_API void color_hex_rgb(char *output, struct color);

    NK_API void color_hsv_i(int *out_h, int *out_s, int *out_v, struct color);
    NK_API void color_hsv_b(std::byte *out_h, std::byte *out_s, std::byte *out_v, struct color);
    NK_API void color_hsv_iv(int *hsv_out, struct color);
    NK_API void color_hsv_bv(std::byte *hsv_out, struct color);
    NK_API void color_hsv_f(float *out_h, float *out_s, float *out_v, struct color);
    NK_API void color_hsv_fv(float *hsv_out, struct color);

    NK_API void color_hsva_i(int *h, int *s, int *v, int *a, struct color);
    NK_API void color_hsva_b(std::byte *h, std::byte *s, std::byte *v, std::byte *a, struct color);
    NK_API void color_hsva_iv(int *hsva_out, struct color);
    NK_API void color_hsva_bv(std::byte *hsva_out, struct color);
    NK_API void color_hsva_f(float *out_h, float *out_s, float *out_v, float *out_a, struct color);
    NK_API void color_hsva_fv(float *hsva_out, struct color);
    /* =============================================================================
     *
     *                                  IMAGE
     *
     * ============================================================================= */
    NK_API resource_handle handle_ptr(void*);
    NK_API resource_handle handle_id(int);
    NK_API struct image image_handle(resource_handle);
    NK_API struct image image_ptr(void*);
    NK_API struct image image_id(int);
    NK_API bool image_is_subimage(const struct image* img);
    NK_API struct image subimage_ptr(void*, unsigned short w, unsigned short h, struct rectf sub_region);
    NK_API struct image subimage_id(int, unsigned short w, unsigned short h, struct rectf sub_region);
    NK_API struct image subimage_handle(resource_handle, unsigned short w, unsigned short h, struct rectf sub_region);
    /* =============================================================================
     *
     *                                  9-SLICE
     *
     * ============================================================================= */
    NK_API struct nine_slice nine_slice_handle(resource_handle, unsigned short l, unsigned short t, unsigned short r, unsigned short b);
    NK_API struct nine_slice nine_slice_ptr(void*, unsigned short l, unsigned short t, unsigned short r, unsigned short b);
    NK_API struct nine_slice nine_slice_id(int, unsigned short l, unsigned short t, unsigned short r, unsigned short b);
    NK_API int nine_slice_is_sub9slice(const struct nine_slice* img);
    NK_API struct nine_slice sub9slice_ptr(void*, unsigned short w, unsigned short h, struct rectf sub_region, unsigned short l, unsigned short t, unsigned short r, unsigned short b);
    NK_API struct nine_slice sub9slice_id(int, unsigned short w, unsigned short h, struct rectf sub_region, unsigned short l, unsigned short t, unsigned short r, unsigned short b);
    NK_API struct nine_slice sub9slice_handle(resource_handle, unsigned short w, unsigned short h, struct rectf sub_region, unsigned short l, unsigned short t, unsigned short r, unsigned short b);
    /* =============================================================================
     *
     *                                  MATH
     *
     * ============================================================================= */
    NK_API hash murmur_hash(const void *key, int len, hash seed);
    NK_API void triangle_from_direction(struct vec2f *result, struct rectf r, float pad_x, float pad_y, heading);

    NK_API struct vec2f vec2_from_floats(float x, float y);
    NK_API struct vec2f vec2i_from_ints(int x, int y);
    NK_API struct vec2f vec2v(const float *xy);
    NK_API struct vec2f vec2iv(const int *xy);

    NK_API struct rectf get_null_rect(void);
    NK_API struct rectf rect(float x, float y, float w, float h);
    NK_API struct rectf recti(int x, int y, int w, int h);
    NK_API struct rectf recta(struct vec2f pos, struct vec2f size);
    NK_API struct rectf rectv(const float *xywh);
    NK_API struct rectf rectiv(const int *xywh);
    NK_API struct vec2f rect_pos(struct rectf);
    NK_API struct vec2f rect_size(struct rectf);
    /* =============================================================================
     *
     *                                  STRING
     *
     * ============================================================================= */
    NK_API int strlen(const char *str);
    NK_API int stricmp(const char *s1, const char *s2);
    NK_API int stricmpn(const char *s1, const char *s2, int n);
    NK_API int strtoi(const char *str, char **endptr);
    NK_API float strtof(const char *str, char **endptr);
  #ifndef NK_STRTOD
  #define NK_STRTOD strtod
    NK_API double strtod(const char *str, char **endptr);
  #endif
    NK_API int strfilter(const char *text, const char *regexp);
    NK_API int strmatch_fuzzy_string(char const *str, char const *pattern, int *out_score);
    NK_API int strmatch_fuzzy_text(const char *txt, int txt_len, const char *pattern, int *out_score);
    /* =============================================================================
     *
     *                                  UTF-8
     *
     * ============================================================================= */
    NK_API int utf_decode(const char*, rune*, int);
    NK_API int utf_encode(rune, char*, int);
    NK_API int utf_len(const char*, int byte_len);
    NK_API const char* utf_at(const char *buffer, int length, int index, rune *unicode, int *len);
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
     *     struct font_configure cfg = font_configure(font_pixel_height);
     *     cfg.merge_mode = false or true;
     *     cfg.range = font_korean_glyph_ranges();
     *     cfg.coord_type = NK_COORD_PIXEL;
     *     font *font = font_atlas_add_from_file(&atlas, "Path/To/Your/TTF_Font.ttf", 13, &cfg);
     * ```
     */

    struct user_font_glyph;
    typedef float(*text_width_f)(resource_handle, float h, const char*, int len);
    typedef void(*query_font_glyph_f)(resource_handle handle, float font_height,
                                        struct user_font_glyph *glyph,
                                        rune codepoint, rune next_codepoint);

  #if defined(NK_INCLUDE_VERTEX_BUFFER_OUTPUT) || defined(NK_INCLUDE_SOFTWARE_FONT)
    struct user_font_glyph {
      struct vec2 uv[2];  /**!< texture coordinates */
      struct vec2 offset; /**!< offset between top left and glyph */
      float width, height;   /**!< size of the glyph  */
      float xadvance;        /**!< offset to the next glyph */
    };
  #endif

    struct user_font {
      resource_handle userdata;    /**!< user provided font handle */
      float height;          /**!< max height of the font */
      text_width_f width; /**!< font string width in pixel callback */
  #ifdef NK_INCLUDE_VERTEX_BUFFER_OUTPUT
      query_font_glyph_f query; /**!< font glyph callback to query drawing info */
      resource_handle texture;           /**!< texture handle to the used font atlas or texture */
  #endif
    };

  #ifdef NK_INCLUDE_FONT_BAKING
    enum font_coord_type {
      NK_COORD_UV,   /**!< texture coordinates inside font glyphs are clamped between 0-1 */
      NK_COORD_PIXEL /**!< texture coordinates inside font glyphs are in absolute pixel */
  };

    struct font;
    struct baked_font {
      float height;          /**!< height of the font  */
      float ascent;          /**!< font glyphs ascent and descent  */
      float descent;         /**!< font glyphs ascent and descent  */
      rune glyph_offset;  /**!< glyph array offset inside the font glyph baking output array  */
      rune glyph_count;   /**!< number of glyphs of this font inside the glyph baking array output */
      const rune *ranges; /**!< font codepoint ranges as pairs of (from/to) and 0 as last element */
    };

    struct font_config {
      font_config *next; /**!< NOTE: only used internally */
      void *ttf_blob;              /**!< pointer to loaded TTF file memory block.  * \note not needed for font_atlas_add_from_memory and font_atlas_add_from_file. */
      std::size_t ttf_size;            /**!< size of the loaded TTF file memory block * \note not needed for font_atlas_add_from_memory and font_atlas_add_from_file. */

      unsigned char ttf_data_owned_by_atlas;    /**!< used inside font atlas: default to: 0*/
      unsigned char merge_mode;                 /**!< merges this font into the last font */
      unsigned char pixel_snap;                 /**!< align every character to pixel boundary (if true set oversample (1,1)) */
      unsigned char oversample_v, oversample_h; /**!< rasterize at high quality for sub-pixel position */
      unsigned char padding[3];

      float size;                         /**!< baked pixel height of the font */
      enum font_coord_type coord_type; /**!< texture coordinate format with either pixel or UV coordinates */
      struct vec2 spacing;             /**!< extra pixel spacing between glyphs  */
      const rune *range;               /**!< list of unicode ranges (2 values per range, zero terminated) */
      struct baked_font *font;         /**!< font to setup in the baking process: NOTE: not needed for font atlas */
      rune fallback_glyph;             /**!< fallback glyph to use if a given rune is not found */
      struct font_configure *n;
      struct font_configure *p;
    };

    struct font_glyph {
      rune codepoint;
      float xadvance;
      float x0, y0, x1, y1, w, h;
      float u0, v0, u1, v1;
    };

    struct font {
      struct font *next;
      struct user_font handle;
      struct baked_font info;
      float scale;
      struct font_glyph *glyphs;
      const struct font_glyph *fallback;
      rune fallback_codepoint;
      resource_handle texture;
      font_config *config;
    };

    enum font_atlas_format {
      NK_FONT_ATLAS_ALPHA8,
      NK_FONT_ATLAS_RGBA32
  };

    struct font_atlas {
      void *pixel;
      int tex_width;
      int tex_height;

      struct allocator permanent;
      struct allocator temporary;

      struct recti custom;
      struct cursor cursors[static_cast<unsigned>(style_cursor::CURSOR_COUNT)];

      int glyph_count;
      struct font_glyph *glyphs;
      struct font *default_font;
      struct font *fonts;
      struct font_configure *config;
      int font_num;
    };

    /** some language glyph codepoint ranges */
    NK_API const rune *font_default_glyph_ranges(void);
    NK_API const rune *font_chinese_glyph_ranges(void);
    NK_API const rune *font_cyrillic_glyph_ranges(void);
    NK_API const rune *font_korean_glyph_ranges(void);

  #ifdef NK_INCLUDE_DEFAULT_ALLOCATOR
    NK_API void font_atlas_init_default(struct font_atlas*);
  #endif
    NK_API void font_atlas_init(struct font_atlas*, const struct allocator*);
    NK_API void font_atlas_init_custom(struct font_atlas*, const struct allocator *persistent, const struct allocator *transient);
    NK_API void font_atlas_begin(struct font_atlas*);
    NK_API struct font_config font_configure(float pixel_height);
    NK_API struct font *font_atlas_add(struct font_atlas*, const struct font_configure*);
  #ifdef NK_INCLUDE_DEFAULT_FONT
    NK_API struct font* font_atlas_add_default(struct font_atlas*, float height, const struct font_config*);
  #endif
    NK_API struct font* font_atlas_add_from_memory(struct font_atlas *atlas, void *memory, std::size_t size, float height, const struct font_configure *config);
  #ifdef NK_INCLUDE_STANDARD_IO
    NK_API struct font* font_atlas_add_from_file(struct font_atlas *atlas, const char *file_path, float height, const struct font_config*);
  #endif
    NK_API struct font *font_atlas_add_compressed(struct font_atlas*, void *memory, std::size_t size, float height, const struct font_configure*);
    NK_API struct font* font_atlas_add_compressed_base85(struct font_atlas*, const char *data, float height, const struct font_configure *config);
    NK_API const void* font_atlas_bake(struct font_atlas*, int *width, int *height, enum font_atlas_format);
    NK_API void font_atlas_end(struct font_atlas*, resource_handle tex, struct draw_null_texture*);
    NK_API const struct font_glyph* font_find_glyph(const struct font*, rune unicode);
    NK_API void font_atlas_cleanup(struct font_atlas *atlas);
    NK_API void font_atlas_clear(struct font_atlas*);

  #endif

    /** ==============================================================
     *
     *                          MEMORY BUFFER
     *
     * ===============================================================*/
    /**
     * \page Memory Buffer
     * A basic (double)-buffer with linear allocation and resetting as only
     * freeing policy. The buffer's main purpose is to control all memory management
     * inside the GUI toolkit and still leave memory control as much as possible in
     * the hand of the user while also making sure the library is easy to use if
     * not as much control is needed.
     * In general all memory inside this library can be provided from the user in
     * three different ways.
     *
     * The first way and the one providing most control is by just passing a fixed
     * size memory block. In this case all control lies in the hand of the user
     * since he can exactly control where the memory comes from and how much memory
     * the library should consume. Of course using the fixed size API removes the
     * ability to automatically resize a buffer if not enough memory is provided so
     * you have to take over the resizing. While being a fixed sized buffer sounds
     * quite limiting, it is very effective in this library since the actual memory
     * consumption is quite stable and has a fixed upper bound for a lot of cases.
     *
     * If you don't want to think about how much memory the library should allocate
     * at all time or have a very dynamic UI with unpredictable memory consumption
     * habits but still want control over memory allocation you can use the dynamic
     * allocator based API. The allocator consists of two callbacks for allocating
     * and freeing memory and optional userdata so you can plugin your own allocator.
     *
     * The final and easiest way can be used by defining
     * NK_INCLUDE_DEFAULT_ALLOCATOR which uses the standard library memory
     * allocation functions malloc and free and takes over complete control over
     * memory in this library.
     */

    struct memory_status {
      void *memory;
      unsigned int type;
      std::size_t size;
      std::size_t allocated;
      std::size_t needed;
      std::size_t calls;
    };

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

    struct memory_handle {void *ptr;std::size_t size;};
    struct memory_buffer {
      std::array<buffer_marker, static_cast<unsigned>(buffer_allocation_type::BUFFER_MAX)> marker; /**!< buffer marker to free a buffer to a certain offset */
      struct allocator pool;     /**!< allocator callback for dynamic buffers */
      enum allocation_type type; /**!< memory management type */
      struct memory_handle memory;      /**!< memory and size of the current memory block */
      float grow_factor;            /**!< growing factor for dynamic memory management */
      std::size_t allocated;            /**!< total amount of memory allocated */
      std::size_t needed;               /**!< totally consumed memory given that enough memory is present */
      std::size_t calls;                /**!< number of allocation calls */
      std::size_t size;                 /**!< current size of the buffer */
    };

  #ifdef NK_INCLUDE_DEFAULT_ALLOCATOR
    NK_API void buffer_init_default(struct buffer*);
  #endif
    NK_API void buffer_init(struct memory_buffer*, const struct allocator*, std::size_t size);
    NK_API void buffer_init_fixed(struct memory_buffer*, void *memory, std::size_t size);
    NK_API void buffer_info(struct memory_status*, const struct memory_buffer*);
    NK_API void buffer_push(struct memory_buffer*, enum buffer_allocation_type type, const void *memory, std::size_t size, std::size_t align);
    NK_API void buffer_mark(struct memory_buffer*, enum buffer_allocation_type type);
    NK_API void buffer_reset(struct memory_buffer*, enum buffer_allocation_type type);
    NK_API void buffer_clear(struct memory_buffer*);
    NK_API void buffer_free(struct memory_buffer*);
    NK_API void *buffer_memory(struct memory_buffer*);
    NK_API const void *buffer_memory_const(const struct memory_buffer*);
    NK_API std::size_t buffer_total(const struct memory_buffer*);

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
      struct memory_buffer buffer;
      int len; /**!< in codepoints/runes/glyphs */
    };

  #ifdef NK_INCLUDE_DEFAULT_ALLOCATOR
    NK_API void str_init_default(struct str*);
  #endif
    NK_API void str_init(struct str*, const struct allocator*, std::size_t size);
    NK_API void str_init_fixed(struct str*, void *memory, std::size_t size);
    NK_API void str_clear(struct str*);
    NK_API void str_free(struct str*);

    NK_API int str_append_text_char(struct str*, const char*, int);
    NK_API int str_append_str_char(struct str*, const char*);
    NK_API int str_append_text_utf8(struct str*, const char*, int);
    NK_API int str_append_str_utf8(struct str*, const char*);
    NK_API int str_append_text_runes(struct str*, const rune*, int);
    NK_API int str_append_str_runes(struct str*, const rune*);

    NK_API int str_insert_at_char(struct str*, int pos, const char*, int);
    NK_API int str_insert_at_rune(struct str*, int pos, const char*, int);

    NK_API int str_insert_text_char(struct str*, int pos, const char*, int);
    NK_API int str_insert_str_char(struct str*, int pos, const char*);
    NK_API int str_insert_text_utf8(struct str*, int pos, const char*, int);
    NK_API int str_insert_str_utf8(struct str*, int pos, const char*);
    NK_API int str_insert_text_runes(struct str*, int pos, const rune*, int);
    NK_API int str_insert_str_runes(struct str*, int pos, const rune*);

    NK_API void str_remove_chars(struct str*, int len);
    NK_API void str_remove_runes(struct str *str, int len);
    NK_API void str_delete_chars(struct str*, int pos, int len);
    NK_API void str_delete_runes(struct str*, int pos, int len);

    NK_API char *str_at_char(struct str*, int pos);
    NK_API char *str_at_rune(struct str*, int pos, rune *unicode, int *len);
    NK_API rune str_rune_at(const struct str*, int pos);
    NK_API const char *str_at_char_const(const struct str*, int pos);
    NK_API const char *str_at_const(const struct str*, int pos, rune *unicode, int *len);

    NK_API char *str_get(struct str*);
    NK_API const char *str_get_const(const struct str*);
    NK_API int str_len(const struct str*);
    NK_API int str_len_char(const struct str*);

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

  #ifndef NK_TEXTEDIT_UNDOSTATECOUNT
  #define NK_TEXTEDIT_UNDOSTATECOUNT     99
  #endif

  #ifndef NK_TEXTEDIT_UNDOCHARCOUNT
  #define NK_TEXTEDIT_UNDOCHARCOUNT      999
  #endif

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
      std::array<text_undo_record,NK_TEXTEDIT_UNDOSTATECOUNT> undo_rec;
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
      struct clipboard clip;
      struct str string;
      plugin_filter filter;
      struct vec2f scrollbar;

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
      struct text_undo_state undo;
    };

    /** filter function */
    NK_API bool filter_default(const struct text_edit*, rune unicode);
    NK_API bool filter_ascii(const struct text_edit*, rune unicode);
    NK_API bool filter_float(const struct text_edit*, rune unicode);
    NK_API bool filter_decimal(const struct text_edit*, rune unicode);
    NK_API bool filter_hex(const struct text_edit*, rune unicode);
    NK_API bool filter_oct(const struct text_edit*, rune unicode);
    NK_API bool filter_binary(const struct text_edit*, rune unicode);

    /** text editor */
  #ifdef NK_INCLUDE_DEFAULT_ALLOCATOR
    NK_API void textedit_init_default(struct text_edit*);
  #endif
    NK_API void textedit_init(struct text_edit*, const struct allocator*, std::size_t size);
    NK_API void textedit_init_fixed(struct text_edit*, void *memory, std::size_t size);
    NK_API void textedit_free(struct text_edit*);
    NK_API void textedit_text(struct text_edit*, const char*, int total_len);
    NK_API void textedit_delete(struct text_edit*, int where, int len);
    NK_API void textedit_delete_selection(struct text_edit*);
    NK_API void textedit_select_all(struct text_edit*);
    NK_API bool textedit_cut(struct text_edit*);
    NK_API bool textedit_paste(struct text_edit*, char const*, int len);
    NK_API void textedit_undo(struct text_edit*);
    NK_API void textedit_redo(struct text_edit*);

    /* ===============================================================
     *
     *                          DRAWING
     *
     * ===============================================================*/
    /**
     * \page Drawing
     * This library was designed to be render backend agnostic so it does
     * not draw anything to screen. Instead all drawn shapes, widgets
     * are made of, are buffered into memory and make up a command queue.
     * Each frame therefore fills the command buffer with draw commands
     * that then need to be executed by the user and his own render backend.
     * After that the command buffer needs to be cleared and a new frame can be
     * started. It is probably important to note that the command buffer is the main
     * drawing API and the optional vertex buffer API only takes this format and
     * converts it into a hardware accessible format.
     *
     * To use the command queue to draw your own widgets you can access the
     * command buffer of each window by calling `window_get_canvas` after
     * previously having called `begin`:
     *
     * ```c
     *     void draw_red_rectangle_widget(struct context *ctx)
     *     {
     *         struct command_buffer *canvas;
     *         struct input *input = &ctx->input;
     *         canvas = window_get_canvas(ctx);
     *
     *         struct rect space;
     *         enum widget_layout_states state;
     *         state = widget(&space, ctx);
     *         if (!state) return;
     *
     *         if (state != NK_WIDGET_ROM)
     *             update_your_widget_by_user_input(...);
     *         fill_rect(canvas, space, 0, rgb(255,0,0));
     *     }
     *
     *     if (begin(...)) {
     *         layout_row_dynamic(ctx, 25, 1);
     *         draw_red_rectangle_widget(ctx);
     *     }
     *     end(..)
     *
     * ```
     * Important to know if you want to create your own widgets is the `widget`
     * call. It allocates space on the panel reserved for this widget to be used,
     * but also returns the state of the widget space. If your widget is not seen and does
     * not have to be updated it is '0' and you can just return. If it only has
     * to be drawn the state will be `NK_WIDGET_ROM` otherwise you can do both
     * update and draw your widget. The reason for separating is to only draw and
     * update what is actually necessary which is crucial for performance.
     */

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
      enum command_type type;
      std::size_t next;
  #ifdef NK_INCLUDE_COMMAND_USERDATA
      resource_handle userdata;
  #endif
    };

    struct command_scissor {
      struct command header;
      short x, y;
      unsigned short w, h;
    };

    struct command_line {
      struct command header;
      unsigned short line_thickness;
      struct vec2i begin;
      struct vec2i end;
      struct color color;
    };

    struct command_curve {
      struct command header;
      unsigned short line_thickness;
      struct vec2i begin;
      struct vec2i end;
      struct vec2i ctrl[2];
      struct color color;
    };

    struct command_rect {
      struct command header;
      unsigned short rounding;
      unsigned short line_thickness;
      short x, y;
      unsigned short w, h;
      struct color color;
    };

    struct command_rect_filled {
      struct command header;
      unsigned short rounding;
      short x, y;
      unsigned short w, h;
      struct color color;
    };

    struct command_rect_multi_color {
      struct command header;
      short x, y;
      unsigned short w, h;
      struct color left;
      struct color top;
      struct color bottom;
      struct color right;
    };

    struct command_triangle {
      struct command header;
      unsigned short line_thickness;
      struct vec2i a;
      struct vec2i b;
      struct vec2i c;
      struct color color;
    };

    struct command_triangle_filled {
      struct command header;
      struct vec2i a;
      struct vec2i b;
      struct vec2i c;
      struct color color;
    };

    struct command_circle {
      struct command header;
      short x, y;
      unsigned short line_thickness;
      unsigned short w, h;
      struct color color;
    };

    struct command_circle_filled {
      struct command header;
      short x, y;
      unsigned short w, h;
      struct color color;
    };

    struct command_arc {
      struct command header;
      short cx, cy;
      unsigned short r;
      unsigned short line_thickness;
      float a[2];
      struct color color;
    };

    struct command_arc_filled {
      struct command header;
      short cx, cy;
      unsigned short r;
      float a[2];
      struct color color;
    };

    struct command_polygon {
      struct command header;
      struct color color;
      unsigned short line_thickness;
      unsigned short point_count;
      struct vec2i points[1];
    };

    struct command_polygon_filled {
      struct command header;
      struct color color;
      unsigned short point_count;
      struct vec2i points[1];
    };

    struct command_polyline {
      struct command header;
      struct color color;
      unsigned short line_thickness;
      unsigned short point_count;
      struct vec2i points[1];
    };

    struct command_image {
      struct command header;
      short x, y;
      unsigned short w, h;
      struct image img;
      struct color col;
    };

    typedef void (*command_custom_callback)(void *canvas, short x,short y,
        unsigned short w, unsigned short h, resource_handle callback_data);
    struct command_custom {
      struct command header;
      short x, y;
      unsigned short w, h;
      resource_handle callback_data;
      command_custom_callback callback;
    };

    struct command_text {
      struct command header;
      const struct user_font *font;
      struct color background;
      struct color foreground;
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
      struct memory_buffer *base;
      struct rectf clip;
      int use_clipping;
      resource_handle userdata;
      std::size_t begin, end, last;
    };

    /** shape outlines */
    NK_API void stroke_line(struct command_buffer *b, float x0, float y0, float x1, float y1, float line_thickness, struct color);
    NK_API void stroke_curve(struct command_buffer*, float, float, float, float, float, float, float, float, float line_thickness, struct color);
    NK_API void stroke_rect(struct command_buffer*, struct rectf, float rounding, float line_thickness, struct color);
    NK_API void stroke_circle(struct command_buffer*, struct rectf, float line_thickness, struct color);
    NK_API void stroke_arc(struct command_buffer*, float cx, float cy, float radius, float a_min, float a_max, float line_thickness, struct color);
    NK_API void stroke_triangle(struct command_buffer*, float, float, float, float, float, float, float line_thichness, struct color);
    NK_API void stroke_polyline(struct command_buffer*, const float *points, int point_count, float line_thickness, struct color col);
    NK_API void stroke_polygon(struct command_buffer*, const float *points, int point_count, float line_thickness, struct color);

    /** filled shades */
    NK_API void fill_rect(struct command_buffer*, struct rectf, float rounding, struct color);
    NK_API void fill_rect_multi_color(struct command_buffer*, struct rectf, struct color left, struct color top, struct color right, struct color bottom);
    NK_API void fill_circle(struct command_buffer*, struct rectf, struct color);
    NK_API void fill_arc(struct command_buffer*, float cx, float cy, float radius, float a_min, float a_max, struct color);
    NK_API void fill_triangle(struct command_buffer*, float x0, float y0, float x1, float y1, float x2, float y2, struct color);
    NK_API void fill_polygon(struct command_buffer*, const float *points, int point_count, struct color);

    /** misc */
    NK_API void draw_image(struct command_buffer*, struct rectf, const struct image*, struct color);
    NK_API void draw_nine_slice(struct command_buffer*, struct rectf, const struct nine_slice*, struct color);
    NK_API void draw_text(struct command_buffer*, struct rectf, const char *text, int len, const struct user_font*, struct color, struct color);
    NK_API void push_scissor(struct command_buffer*, struct rectf);
    NK_API void push_custom(struct command_buffer*, struct rectf, command_custom_callback, resource_handle usr);

    /* ===============================================================
     *
     *                          INPUT
     *
     * ===============================================================*/
    struct mouse_button {
      bool down;
      unsigned int clicked;
      struct vec2f clicked_pos;
    };
    struct mouse {
      struct mouse_button buttons[NK_BUTTON_MAX];
      struct vec2f pos;
  #ifdef NK_BUTTON_TRIGGER_ON_RELEASE
      struct vec2f down_pos;
  #endif
      struct vec2f prev;
      struct vec2f delta;
      struct vec2f scroll_delta;
      unsigned char grab;
      unsigned char grabbed;
      unsigned char ungrab;
    };

    struct key {
      bool down;
      unsigned int clicked;
    };
    struct keyboard {
      struct key keys[NK_KEY_MAX];
      char text[NK_INPUT_MAX];
      int text_len;
    };

    struct input {
      struct keyboard keyboard;
      struct mouse mouse;
    };

    NK_API bool input_has_mouse_click(const struct input*, enum buttons);
    NK_API bool input_has_mouse_click_in_rect(const struct input*, enum buttons, struct rectf);
    NK_API bool input_has_mouse_click_in_button_rect(const struct input*, enum buttons, struct rectf);
    NK_API bool input_has_mouse_click_down_in_rect(const struct input*, enum buttons, struct rectf, bool down);
    NK_API bool input_is_mouse_click_in_rect(const struct input*, enum buttons, struct rectf);
    NK_API bool input_is_mouse_click_down_in_rect(const struct input *i, enum buttons id, struct rectf b, bool down);
    NK_API bool input_any_mouse_click_in_rect(const struct input*, struct rectf);
    NK_API bool input_is_mouse_prev_hovering_rect(const struct input*, struct rectf);
    NK_API bool input_is_mouse_hovering_rect(const struct input*, struct rectf);
    NK_API bool input_mouse_clicked(const struct input*, enum buttons, struct rectf);
    NK_API bool input_is_mouse_down(const struct input*, enum buttons);
    NK_API bool input_is_mouse_pressed(const struct input*, enum buttons);
    NK_API bool input_is_mouse_released(const struct input*, enum buttons);
    NK_API bool input_is_key_pressed(const struct input*, enum keys);
    NK_API bool input_is_key_released(const struct input*, enum keys);
    NK_API bool input_is_key_down(const struct input*, enum keys);

    /* ===============================================================
     *
     *                          DRAW LIST
     *
     * ===============================================================*/
  #ifdef NK_INCLUDE_VERTEX_BUFFER_OUTPUT
    /**
     * \page "Draw List"
     * The optional vertex buffer draw list provides a 2D drawing context
     * with antialiasing functionality which takes basic filled or outlined shapes
     * or a path and outputs vertexes, elements and draw commands.
     * The actual draw list API is not required to be used directly while using this
     * library since converting the default library draw command output is done by
     * just calling `convert` but I decided to still make this library accessible
     * since it can be useful.
     *
     * The draw list is based on a path buffering and polygon and polyline
     * rendering API which allows a lot of ways to draw 2D content to screen.
     * In fact it is probably more powerful than needed but allows even more crazy
     * things than this library provides by default.
     */

  #ifdef unsigned int_DRAW_INDEX
    typedef std::uint32_t draw_index;
  #else
    typedef unsigned short draw_index;
  #endif
    enum draw_list_stroke {
      NK_STROKE_OPEN = false, /***< build up path has no connection back to the beginning */
      NK_STROKE_CLOSED = true /***< build up path has a connection back to the beginning */
  };

    enum draw_vertex_layout_attribute {
      NK_VERTEX_POSITION,
      NK_VERTEX_COLOR,
      NK_VERTEX_TEXCOORD,
      NK_VERTEX_ATTRIBUTE_COUNT
  };

    enum draw_vertex_layout_format {
      NK_FORMAT_SCHAR,
      NK_FORMAT_SSHORT,
      NK_FORMAT_SINT,
      NK_FORMAT_UCHAR,
      NK_FORMAT_USHORT,
      NK_FORMAT_UINT,
      NK_FORMAT_FLOAT,
      NK_FORMAT_DOUBLE,

  NK_FORMAT_COLOR_BEGIN,
      NK_FORMAT_R8G8B8 = NK_FORMAT_COLOR_BEGIN,
      NK_FORMAT_R16G15B16,
      NK_FORMAT_R32G32B32,

      NK_FORMAT_R8G8B8A8,
      NK_FORMAT_B8G8R8A8,
      NK_FORMAT_R16G15B16A16,
      NK_FORMAT_R32G32B32A32,
      NK_FORMAT_R32G32B32A32_FLOAT,
      NK_FORMAT_R32G32B32A32_DOUBLE,

      NK_FORMAT_RGB32,
      NK_FORMAT_RGBA32,
  NK_FORMAT_COLOR_END = NK_FORMAT_RGBA32,
      NK_FORMAT_COUNT
  };

  #define NK_VERTEX_LAYOUT_END NK_VERTEX_ATTRIBUTE_COUNT,NK_FORMAT_COUNT,0
    struct draw_vertex_layout_element {
      enum draw_vertex_layout_attribute attribute;
      enum draw_vertex_layout_format format;
      std::size_t offset;
    };

    struct draw_command {
      unsigned int elem_count; /**< number of elements in the current draw batch */
      struct rect clip_rect; /**< current screen clipping rectangle */
      handle texture; /**< current texture to set */
  #ifdef NK_INCLUDE_COMMAND_USERDATA
      handle userdata;
  #endif
    };

    struct draw_list {
      struct rect clip_rect;
      struct vec2 circle_vtx[12];
      struct convert_config config;

      struct buffer *buffer;
      struct buffer *vertices;
      struct buffer *elements;

      unsigned int element_count;
      unsigned int vertex_count;
      unsigned int cmd_count;
      std::size_t cmd_offset;

      unsigned int path_count;
      unsigned int path_offset;

      enum anti_aliasing line_AA;
      enum anti_aliasing shape_AA;

  #ifdef NK_INCLUDE_COMMAND_USERDATA
      handle userdata;
  #endif
    };

    /* draw list */
    NK_API void draw_list_init(struct draw_list*);
    NK_API void draw_list_setup(struct draw_list*, const struct convert_config*, struct buffer *cmds, struct buffer *vertices, struct buffer *elements, enum anti_aliasing line_aa,enum anti_aliasing shape_aa);

    /* drawing */
  #define draw_list_foreach(cmd, can, b) for((cmd)=_draw_list_begin(can, b); (cmd)!=0; (cmd)=_draw_list_next(cmd, b, can))
    NK_API const struct draw_command* _draw_list_begin(const struct draw_list*, const struct buffer*);
    NK_API const struct draw_command* _draw_list_next(const struct draw_command*, const struct buffer*, const struct draw_list*);
    NK_API const struct draw_command* _draw_list_end(const struct draw_list*, const struct buffer*);

    /* path */
    NK_API void draw_list_path_clear(struct draw_list*);
    NK_API void draw_list_path_line_to(struct draw_list*, struct vec2 pos);
    NK_API void draw_list_path_arc_to_fast(struct draw_list*, struct vec2 center, float radius, int a_min, int a_max);
    NK_API void draw_list_path_arc_to(struct draw_list*, struct vec2 center, float radius, float a_min, float a_max, unsigned int segments);
    NK_API void draw_list_path_rect_to(struct draw_list*, struct vec2 a, struct vec2 b, float rounding);
    NK_API void draw_list_path_curve_to(struct draw_list*, struct vec2 p2, struct vec2 p3, struct vec2 p4, unsigned int num_segments);
    NK_API void draw_list_path_fill(struct draw_list*, struct color);
    NK_API void draw_list_path_stroke(struct draw_list*, struct color, enum draw_list_stroke closed, float thickness);

    /* stroke */
    NK_API void draw_list_stroke_line(struct draw_list*, struct vec2 a, struct vec2 b, struct color, float thickness);
    NK_API void draw_list_stroke_rect(struct draw_list*, struct rect rect, struct color, float rounding, float thickness);
    NK_API void draw_list_stroke_triangle(struct draw_list*, struct vec2 a, struct vec2 b, struct vec2 c, struct color, float thickness);
    NK_API void draw_list_stroke_circle(struct draw_list*, struct vec2 center, float radius, struct color, unsigned int segs, float thickness);
    NK_API void draw_list_stroke_curve(struct draw_list*, struct vec2 p0, struct vec2 cp0, struct vec2 cp1, struct vec2 p1, struct color, unsigned int segments, float thickness);
    NK_API void draw_list_stroke_poly_line(struct draw_list*, const struct vec2 *pnts, const unsigned int cnt, struct color, enum draw_list_stroke, float thickness, enum anti_aliasing);

    /* fill */
    NK_API void draw_list_fill_rect(struct draw_list*, struct rect rect, struct color, float rounding);
    NK_API void draw_list_fill_rect_multi_color(struct draw_list*, struct rect rect, struct color left, struct color top, struct color right, struct color bottom);
    NK_API void draw_list_fill_triangle(struct draw_list*, struct vec2 a, struct vec2 b, struct vec2 c, struct color);
    NK_API void draw_list_fill_circle(struct draw_list*, struct vec2 center, float radius, struct color col, unsigned int segs);
    NK_API void draw_list_fill_poly_convex(struct draw_list*, const struct vec2 *points, const unsigned int count, struct color, enum anti_aliasing);

    /* misc */
    NK_API void draw_list_add_image(struct draw_list*, struct image texture, struct rect rect, struct color);
    NK_API void draw_list_add_text(struct draw_list*, const struct user_font*, struct rect, const char *text, int len, float font_height, struct color);
  #ifdef NK_INCLUDE_COMMAND_USERDATA
    NK_API void draw_list_push_userdata(struct draw_list*, handle userdata);
  #endif

  #endif

    /* ===============================================================
     *
     *                          GUI
     *
     * ===============================================================*/
    enum class style_item_type {
      STYLE_ITEM_COLOR,
      STYLE_ITEM_IMAGE,
      STYLE_ITEM_NINE_SLICE
  };

    union style_item_data {
      struct color color;
      struct image image;
      struct nine_slice slice;
    };

    struct style_item {
      enum style_item_type type;
      union style_item_data data;
    };

    struct style_text {
      struct color color;
      struct vec2f padding;
      float color_factor;
      float disabled_factor;
    };

    struct style_button {
      /* background */
      struct style_item normal;
      struct style_item hover;
      struct style_item active;
      struct color border_color;
      float color_factor_background;

      /* text */
      struct color text_background;
      struct color text_normal;
      struct color text_hover;
      struct color text_active;
      flag text_alignment;
      float color_factor_text;

      /* properties */
      float border;
      float rounding;
      struct vec2f padding;
      struct vec2f image_padding;
      struct vec2f touch_padding;
      float disabled_factor;

      /* optional user callbacks */
      resource_handle userdata;
      void(*draw_begin)(struct command_buffer*, resource_handle userdata);
      void(*draw_end)(struct command_buffer*, resource_handle userdata);
    };

    struct style_toggle {
      /* background */
      struct style_item normal;
      struct style_item hover;
      struct style_item active;
      struct color border_color;

      /* cursor */
      struct style_item cursor_normal;
      struct style_item cursor_hover;

      /* text */
      struct color text_normal;
      struct color text_hover;
      struct color text_active;
      struct color text_background;
      flag text_alignment;

      /* properties */
      struct vec2f padding;
      struct vec2f touch_padding;
      float spacing;
      float border;
      float color_factor;
      float disabled_factor;

      /* optional user callbacks */
      resource_handle userdata;
      void(*draw_begin)(struct command_buffer*, resource_handle);
      void(*draw_end)(struct command_buffer*, resource_handle);
    };

    struct style_selectable {
      /* background (inactive) */
      struct style_item normal;
      struct style_item hover;
      struct style_item pressed;

      /* background (active) */
      struct style_item normal_active;
      struct style_item hover_active;
      struct style_item pressed_active;

      /* text color (inactive) */
      struct color text_normal;
      struct color text_hover;
      struct color text_pressed;

      /* text color (active) */
      struct color text_normal_active;
      struct color text_hover_active;
      struct color text_pressed_active;
      struct color text_background;
      flag text_alignment;

      /* properties */
      float rounding;
      struct vec2f padding;
      struct vec2f touch_padding;
      struct vec2f image_padding;
      float color_factor;
      float disabled_factor;

      /* optional user callbacks */
      resource_handle userdata;
      void(*draw_begin)(struct command_buffer*, resource_handle);
      void(*draw_end)(struct command_buffer*, resource_handle);
    };

    struct style_slider {
      /* background */
      struct style_item normal;
      struct style_item hover;
      struct style_item active;
      struct color border_color;

      /* background bar */
      struct color bar_normal;
      struct color bar_hover;
      struct color bar_active;
      struct color bar_filled;

      /* cursor */
      struct style_item cursor_normal;
      struct style_item cursor_hover;
      struct style_item cursor_active;

      /* properties */
      float border;
      float rounding;
      float bar_height;
      struct vec2f padding;
      struct vec2f spacing;
      struct vec2f cursor_size;
      float color_factor;
      float disabled_factor;

      /* optional buttons */
      int show_buttons;
      struct style_button inc_button;
      struct style_button dec_button;
      enum symbol_type inc_symbol;
      enum symbol_type dec_symbol;

      /* optional user callbacks */
      resource_handle userdata;
      void(*draw_begin)(struct command_buffer*, resource_handle);
      void(*draw_end)(struct command_buffer*, resource_handle);
    };

    struct style_knob {
      /* background */
      struct style_item normal;
      struct style_item hover;
      struct style_item active;
      struct color border_color;

      /* knob */
      struct color knob_normal;
      struct color knob_hover;
      struct color knob_active;
      struct color knob_border_color;

      /* cursor */
      struct color cursor_normal;
      struct color cursor_hover;
      struct color cursor_active;

      /* properties */
      float border;
      float knob_border;
      struct vec2f padding;
      struct vec2f spacing;
      float cursor_width;
      float color_factor;
      float disabled_factor;

      /* optional user callbacks */
      resource_handle userdata;
      void(*draw_begin)(struct command_buffer*, resource_handle);
      void(*draw_end)(struct command_buffer*, resource_handle);
    };

    struct style_progress {
      /* background */
      struct style_item normal;
      struct style_item hover;
      struct style_item active;
      struct color border_color;

      /* cursor */
      struct style_item cursor_normal;
      struct style_item cursor_hover;
      struct style_item cursor_active;
      struct color cursor_border_color;

      /* properties */
      float rounding;
      float border;
      float cursor_border;
      float cursor_rounding;
      struct vec2f padding;
      float color_factor;
      float disabled_factor;

      /* optional user callbacks */
      resource_handle userdata;
      void(*draw_begin)(struct command_buffer*, resource_handle);
      void(*draw_end)(struct command_buffer*, resource_handle);
    };

    struct style_scrollbar {
      /* background */
      struct style_item normal;
      struct style_item hover;
      struct style_item active;
      struct color border_color;

      /* cursor */
      struct style_item cursor_normal;
      struct style_item cursor_hover;
      struct style_item cursor_active;
      struct color cursor_border_color;

      /* properties */
      float border;
      float rounding;
      float border_cursor;
      float rounding_cursor;
      struct vec2f padding;
      float color_factor;
      float disabled_factor;

      /* optional buttons */
      int show_buttons;
      struct style_button inc_button;
      struct style_button dec_button;
      enum symbol_type inc_symbol;
      enum symbol_type dec_symbol;

      /* optional user callbacks */
      resource_handle userdata;
      void(*draw_begin)(struct command_buffer*, resource_handle);
      void(*draw_end)(struct command_buffer*, resource_handle);
    };

    struct style_edit {
      /* background */
      struct style_item normal;
      struct style_item hover;
      struct style_item active;
      struct color border_color;
      struct style_scrollbar scrollbar;

      /* cursor  */
      struct color cursor_normal;
      struct color cursor_hover;
      struct color cursor_text_normal;
      struct color cursor_text_hover;

      /* text (unselected) */
      struct color text_normal;
      struct color text_hover;
      struct color text_active;

      /* text (selected) */
      struct color selected_normal;
      struct color selected_hover;
      struct color selected_text_normal;
      struct color selected_text_hover;

      /* properties */
      float border;
      float rounding;
      float cursor_size;
      struct vec2f scrollbar_size;
      struct vec2f padding;
      float row_padding;
      float color_factor;
      float disabled_factor;
    };

    struct style_property {
      /* background */
      struct style_item normal;
      struct style_item hover;
      struct style_item active;
      struct color border_color;

      /* text */
      struct color label_normal;
      struct color label_hover;
      struct color label_active;

      /* symbols */
      enum symbol_type sym_left;
      enum symbol_type sym_right;

      /* properties */
      float border;
      float rounding;
      struct vec2f padding;
      float color_factor;
      float disabled_factor;

      struct style_edit edit;
      struct style_button inc_button;
      struct style_button dec_button;

      /* optional user callbacks */
      resource_handle userdata;
      void(*draw_begin)(struct command_buffer*, resource_handle);
      void(*draw_end)(struct command_buffer*, resource_handle);
    };

    struct style_chart {
      /* colors */
      struct style_item background;
      struct color border_color;
      struct color selected_color;
      struct color color;

      /* properties */
      float border;
      float rounding;
      struct vec2f padding;
      float color_factor;
      float disabled_factor;
      bool show_markers;
    };

    struct style_combo {
      /* background */
      struct style_item normal;
      struct style_item hover;
      struct style_item active;
      struct color border_color;

      /* label */
      struct color label_normal;
      struct color label_hover;
      struct color label_active;

      /* symbol */
      struct color symbol_normal;
      struct color symbol_hover;
      struct color symbol_active;

      /* button */
      struct style_button button;
      enum symbol_type sym_normal;
      enum symbol_type sym_hover;
      enum symbol_type sym_active;

      /* properties */
      float border;
      float rounding;
      struct vec2f content_padding;
      struct vec2f button_padding;
      struct vec2f spacing;
      float color_factor;
      float disabled_factor;
    };

    struct style_tab {
      /* background */
      struct style_item background;
      struct color border_color;
      struct color text;

      /* button */
      struct style_button tab_maximize_button;
      struct style_button tab_minimize_button;
      struct style_button node_maximize_button;
      struct style_button node_minimize_button;
      enum symbol_type sym_minimize;
      enum symbol_type sym_maximize;

      /* properties */
      float border;
      float rounding;
      float indent;
      struct vec2f padding;
      struct vec2f spacing;
      float color_factor;
      float disabled_factor;
    };

    enum class style_header_align {
      HEADER_LEFT,
      HEADER_RIGHT
  };
    struct style_window_header {
      /* background */
      struct style_item normal;
      struct style_item hover;
      struct style_item active;

      /* button */
      struct style_button close_button;
      struct style_button minimize_button;
      enum symbol_type close_symbol;
      enum symbol_type minimize_symbol;
      enum symbol_type maximize_symbol;

      /* title */
      struct color label_normal;
      struct color label_hover;
      struct color label_active;

      /* properties */
      enum style_header_align align;
      struct vec2f padding;
      struct vec2f label_padding;
      struct vec2f spacing;
    };

    struct style_window {
      struct style_window_header header;
      struct style_item fixed_background;
      struct color background;

      struct color border_color;
      struct color popup_border_color;
      struct color combo_border_color;
      struct color contextual_border_color;
      struct color menu_border_color;
      struct color group_border_color;
      struct color tooltip_border_color;
      struct style_item scaler;

      float border;
      float combo_border;
      float contextual_border;
      float menu_border;
      float group_border;
      float tooltip_border;
      float popup_border;
      float min_row_height_padding;

      float rounding;
      struct vec2f spacing;
      struct vec2f scrollbar_size;
      struct vec2f min_size;

      struct vec2f padding;
      struct vec2f group_padding;
      struct vec2f popup_padding;
      struct vec2f combo_padding;
      struct vec2f contextual_padding;
      struct vec2f menu_padding;
      struct vec2f tooltip_padding;
    };

    struct style {
      const struct user_font *font;
      const struct cursor *cursors[static_cast<std::size_t>(nk::style_cursor::CURSOR_COUNT)];
      const struct cursor *cursor_active;
      struct cursor *cursor_last;
      int cursor_visible;

      struct style_text text;
      struct style_button button;
      struct style_button contextual_button;
      struct style_button menu_button;
      struct style_toggle option;
      struct style_toggle checkbox;
      struct style_selectable selectable;
      struct style_slider slider;
      struct style_knob knob;
      struct style_progress progress;
      struct style_property property;
      struct style_edit edit;
      struct style_chart chart;
      struct style_scrollbar scrollh;
      struct style_scrollbar scrollv;
      struct style_tab tab;
      struct style_combo combo;
      struct style_window window;
    };

    NK_API struct style_item style_item_color(struct color);
    NK_API struct style_item style_item_image(struct image img);
    NK_API struct style_item style_item_nine_slice(struct nine_slice slice);
    NK_API struct style_item style_item_hide(void);

    /*==============================================================
     *                          PANEL
     * =============================================================*/
  #ifndef NK_MAX_LAYOUT_ROW_TEMPLATE_COLUMNS
  #define NK_MAX_LAYOUT_ROW_TEMPLATE_COLUMNS 16
  #endif
  #ifndef NK_CHART_MAX_SLOT
  #define NK_CHART_MAX_SLOT 4
  #endif

    enum class panel_type {
      PANEL_NONE       = 0,
      PANEL_WINDOW     = NK_FLAG(0),
      PANEL_GROUP      = NK_FLAG(1),
      PANEL_POPUP      = NK_FLAG(2),
      PANEL_CONTEXTUAL = NK_FLAG(4),
      PANEL_COMBO      = NK_FLAG(5),
      PANEL_MENU       = NK_FLAG(6),
      PANEL_TOOLTIP    = NK_FLAG(7)
  };
    enum class panel_set {
      PANEL_SET_NONBLOCK = std::to_underlying(panel_type::PANEL_CONTEXTUAL | panel_type::PANEL_COMBO | panel_type::PANEL_MENU | panel_type::PANEL_TOOLTIP),
      PANEL_SET_POPUP = PANEL_SET_NONBLOCK | std::to_underlying(panel_type::PANEL_POPUP),
      PANEL_SET_SUB = std::to_underlying(panel_type::PANEL_GROUP) | PANEL_SET_POPUP
    };

    struct chart_slot {
      chart_type type;
      struct color color;
      struct color highlight;
      float min, max, range;
      int count;
      struct vec2f last;
      int index;
      bool show_markers;
    };

    struct chart {
      int slot;
      float x, y, w, h;
      struct chart_slot slots[NK_CHART_MAX_SLOT];
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
      enum panel_row_layout_type type;
      int index;
      float height;
      float min_height;
      int columns;
      const float *ratio;
      float item_width;
      float item_height;
      float item_offset;
      float filled;
      struct rectf item;
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
      struct scroll offset;
    };

    struct panel {
      enum panel_type type;
      flag flags;
      struct rectf bounds;
      std::uint32_t *offset_x;
      std::uint32_t *offset_y;
      float at_x, at_y, max_x;
      float footer_height;
      float header_height;
      float border;
      unsigned int has_scrolling;
      struct rectf clip;
      struct menu_state menu;
      struct row_layout row;
      struct chart chart;
      struct command_buffer *buffer;
      struct panel *parent;
    };

    /*==============================================================
     *                          WINDOW
     * =============================================================*/
  #ifndef NK_WINDOW_MAX_NAME
  #define NK_WINDOW_MAX_NAME 64
  #endif

    struct table;
    enum class window_flags {
      WINDOW_PRIVATE       = NK_FLAG(11),
      WINDOW_DYNAMIC       = WINDOW_PRIVATE,                  /**< special window type growing up in height while being filled to a certain maximum height */
      WINDOW_ROM           = NK_FLAG(12),                        /**< sets window widgets into a read only mode and does not allow input changes */
      WINDOW_NOT_INTERACTIVE = WINDOW_ROM |
        std::to_underlying(panel_flags::WINDOW_NO_INPUT), /**< prevents all interaction caused by input to either window or widgets inside */
      WINDOW_HIDDEN        = NK_FLAG(13),                        /**< Hides window and stops any window interaction and drawing */
      WINDOW_CLOSED        = NK_FLAG(14),                        /**< Directly closes and frees the window at the end of the frame */
      WINDOW_MINIMIZED     = NK_FLAG(15),                        /**< marks the window as minimized */
      WINDOW_REMOVE_ROM    = NK_FLAG(16)                         /**< Removes read only mode at the end of the window */
  };

    struct popup_state {
      struct window *win;
      enum panel_type type;
      struct popup_buffer buf;
      hash name;
      bool active;
      unsigned combo_count;
      unsigned con_count, con_old;
      unsigned active_con;
      struct rectf header;
    };

    struct edit_state {
      hash name;
      unsigned int seq;
      unsigned int old;
      int active, prev;
      int cursor;
      int sel_start;
      int sel_end;
      struct scroll scrollbar;
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

    struct window {
      unsigned int seq;
      hash name;
      char name_string[NK_WINDOW_MAX_NAME];
      flag flags;

      struct rectf bounds;
      struct scroll scrollbar;
      struct command_buffer buffer;
      struct panel *layout;
      float scrollbar_hiding_timer;

      /* persistent widget state */
      struct property_state property;
      struct popup_state popup;
      struct edit_state edit;
      unsigned int scrolled;
      bool widgets_disabled;

      struct table *tables;
      unsigned int table_count;

      /* window list hooks */
      struct window *next;
      struct window *prev;
      struct window *parent;
    };

    /*==============================================================
     *                          STACK
     * =============================================================*/
    /**
     * \page Stack
     * # Stack
     * The style modifier stack can be used to temporarily change a
     * property inside `style`. For example if you want a special
     * red button you can temporarily push the old button color onto a stack
     * draw the button with a red color and then you just pop the old color
     * back from the stack:
     *
     *     style_push_style_item(ctx, &ctx->style.button.normal, style_item_color(rgb(255,0,0)));
     *     style_push_style_item(ctx, &ctx->style.button.hover, style_item_color(rgb(255,0,0)));
     *     style_push_style_item(ctx, &ctx->style.button.active, style_item_color(rgb(255,0,0)));
     *     style_push_vec2(ctx, &cx->style.button.padding, vec2(2,2));
     *
     *     button(...);
     *
     *     style_pop_style_item(ctx);
     *     style_pop_style_item(ctx);
     *     style_pop_style_item(ctx);
     *     style_pop_vec2(ctx);
     *
     * Nuklear has a stack for style_items, float properties, vector properties,
     * flag, colors, fonts and for btn_behavior. Each has it's own fixed size stack
     * which can be changed at compile time.
     */

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

  #define float float

    struct config_stack_style_item_element { struct style_item *address; struct style_item old_value; };
    struct config_stack_float_element { float *address; float old_value; };
    struct config_stack_vec2_element { struct vec2f *address; struct vec2f old_value; };
    struct config_stack_flags_element { flag *address; flag old_value; };
    struct config_stack_color_element { struct color *address; struct color old_value; };
    struct config_stack_user_font_element { const struct user_font* *address; const struct user_font* old_value; };
    struct config_stack_button_behavior_element {
      btn_behavior *address;
      btn_behavior old_value;
    };

    struct config_stack_style_item { int head; std::array<config_stack_style_item_element, 16> elements; };
    struct config_stack_float { int head; std::array<config_stack_float_element, 32> elements; };
    struct config_stack_vec2 { int head; std::array<config_stack_vec2_element, 16> elements; };
    struct config_stack_flags { int head; std::array<config_stack_flags_element, 32> elements; };
    struct config_stack_color { int head; std::array<config_stack_color_element, 32> elements; };
    struct config_stack_user_font { int head; std::array<config_stack_user_font_element, 8> elements; };
    struct config_stack_button_behavior { int head; std::array<config_stack_button_behavior_element, 8> elements; };

    struct configuration_stacks {
      config_stack_style_item style_items;
      config_stack_float floats;
      config_stack_vec2 vectors;
      config_stack_flags flags;
      config_stack_color colors;
      config_stack_user_font fonts;
      config_stack_button_behavior button_behaviors;
    };
    /*==============================================================
     *                          CONTEXT
     * =============================================================*/
  #define NK_VALUE_PAGE_CAPACITY \
  (((NK_MAX(sizeof(struct window),sizeof(struct panel)) / sizeof(std::uint32_t))) / 2)

    struct table {
      unsigned int seq;
      unsigned int size;
      hash keys[NK_VALUE_PAGE_CAPACITY];
      std::uint32_t values[NK_VALUE_PAGE_CAPACITY];
      struct table *next, *prev;
    };

    union page_data {
      struct table tbl;
      struct panel pan;
      struct window win;
    };

    struct page_element {
      union page_data data;
      struct page_element *next;
      struct page_element *prev;
    };

    struct page {
      unsigned int size;
      struct page *next;
      struct page_element win[1];
    };

    struct pool {
      struct allocator alloc;
      enum allocation_type type;
      unsigned int page_count;
      struct page *pages;
      struct page_element *freelist;
      unsigned capacity;
      std::size_t size;
      std::size_t cap;
    };

    struct context {
      /* public: can be accessed freely */
      struct input input;
      struct style style;
      struct memory_buffer memory;
      struct clipboard clip;
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
      struct text_edit text_edit;
      /** draw buffer used for overlay drawing operation like cursor */
      struct command_buffer overlay;

      /** windows */
      int build;
      int use_pool;
      struct pool pool;
      struct window *begin;
      struct window *end;
      struct window *active;
      struct window *current;
      struct page_element *freelist;
      unsigned int count;
      unsigned int seq;
    };

    /* ==============================================================
     *                          MATH
     * =============================================================== */
  #define NK_PI 3.141592654f
  #define NK_PI_HALF 1.570796326f
  #define NK_UTF_INVALID 0xFFFD
  #define NK_MAX_FLOAT_PRECISION 2

  #define NK_UNUSED(x) ((void)(x))
  #define NK_SATURATE(x) (NK_MAX(0, NK_MIN(1.0f, x)))
  #define NK_LEN(a) (sizeof(a)/sizeof(a)[0])
  #define NK_ABS(a) (((a) < 0) ? -(a) : (a))
  #define NK_BETWEEN(x, a, b) ((a) <= (x) && (x) < (b))
  #define NK_INBOX(px, py, x, y, w, h)\
  (NK_BETWEEN(px,x,x+w) && NK_BETWEEN(py,y,y+h))
  #define intERSECT(x0, y0, w0, h0, x1, y1, w1, h1) \
  ((x1 < (x0 + w0)) && (x0 < (x1 + w1)) && \
  (y1 < (y0 + h0)) && (y0 < (y1 + h1)))
  #define NK_CONTAINS(x, y, w, h, bx, by, bw, bh)\
  (NK_INBOX(x,y, bx, by, bw, bh) && NK_INBOX(x+w,y+h, bx, by, bw, bh))

  #define vec2_sub(a, b) vec2((a).x - (b).x, (a).y - (b).y)
  #define vec2_add(a, b) vec2((a).x + (b).x, (a).y + (b).y)
  #define vec2_len_sqr(a) ((a).x*(a).x+(a).y*(a).y)
  #define vec2_muls(a, t) vec2((a).x * (t), (a).y * (t))

  #define ptr_add(t, p, i) ((t*)((void*)((std::byte*)(p) + (i))))
  #define ptr_add_const(t, p, i) ((const t*)((const void*)((const std::byte*)(p) + (i))))
  #define zero_struct(s) zero(&s, sizeof(s))

    /* ==============================================================
     *                          ALIGNMENT
     * =============================================================== */
    /* Pointer to Integer type conversion for pointer alignment */
  #if defined(__PTRDIFF_TYPE__) /* This case should work for GCC*/
  # define NK_UINT_TO_PTR(x) ((void*)(__PTRDIFF_TYPE__)(x))
  # define NK_PTR_TO_UINT(x) ((std::size_t)(__PTRDIFF_TYPE__)(x))
  #elif !defined(__GNUC__) /* works for compilers other than LLVM */
  # define NK_UINT_TO_PTR(x) ((void*)&((char*)0)[x])
  # define NK_PTR_TO_UINT(x) ((std::size_t)(((char*)x)-(char*)0))
  #elif defined(NK_USE_FIXED_TYPES) /* used if we have <stdint.h> */
  # define NK_UINT_TO_PTR(x) ((void*)(uintptr_t)(x))
  # define NK_PTR_TO_UINT(x) ((uintptr_t)(x))
  #else /* generates warning but works */
  # define NK_UINT_TO_PTR(x) ((void*)(x))
  # define NK_PTR_TO_UINT(x) ((std::size_t)(x))
  #endif

  #define NK_ALIGN_PTR(x, mask)\
  (NK_UINT_TO_PTR((NK_PTR_TO_UINT((std::byte*)(x) + (mask-1)) & ~(mask-1))))
  #define NK_ALIGN_PTR_BACK(x, mask)\
  (NK_UINT_TO_PTR((NK_PTR_TO_UINT((std::byte*)(x)) & ~(mask-1))))

  #if ((defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)) && !defined(EMSCRIPTEN)
  #define NK_OFFSETOF(st,m) (__builtin_offsetof(st,m))
  #else
  #define NK_OFFSETOF(st,m) ((std::uintptr_t)&(((st*)0)->m))
  #endif


    /*template<typename T> struct alignof;
    template<typename T, int size_diff> struct helper{enum {value = size_diff};};
    template<typename T> struct helper<T,0>{enum {value = alignof<T>::value};};
    template<typename T> struct alignof{struct Big {T x; char c;}; enum {
      diff = sizeof(Big) - sizeof(T), value = helper<Big, diff>::value};};*/
  #define NK_ALIGNOF(t) (alignof<t>::value)

  #define NK_CONTAINER_OF(ptr,type,member)\
  (type*)((void*)((char*)(1 ? (ptr): &((type*)0)->member) - NK_OFFSETOF(type, member)))
}
#endif /* NK_NUKLEAR_H_ */
