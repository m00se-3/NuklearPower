#ifndef NK_POWER_GUI_HPP
#define NK_POWER_GUI_HPP

#include <internal/nuklear_internal.hpp>

namespace nk {

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

#ifdef NK_UINT_DRAW_INDEX
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

#define NK_VERTEX_LAYOUT_END NK_VERTEX_ATTRIBUTE_COUNT, NK_FORMAT_COUNT, 0

  /* draw list */
  NK_API void draw_list_init(struct draw_list*);
  NK_API void draw_list_setup(struct draw_list*, const struct convert_config*, struct buffer* cmds, struct buffer* vertices, struct buffer* elements, enum anti_aliasing line_aa, enum anti_aliasing shape_aa);

  /* drawing */
#define draw_list_foreach(cmd, can, b) for ((cmd) = _draw_list_begin(can, b); (cmd) != 0; (cmd) = _draw_list_next(cmd, b, can))
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
  NK_API void draw_list_stroke_poly_line(struct draw_list*, const struct vec2* pnts, const unsigned int cnt, struct color, enum draw_list_stroke, float thickness, enum anti_aliasing);

  /* fill */
  NK_API void draw_list_fill_rect(struct draw_list*, struct rect rect, struct color, float rounding);
  NK_API void draw_list_fill_rect_multi_color(struct draw_list*, struct rect rect, struct color left, struct color top, struct color right, struct color bottom);
  NK_API void draw_list_fill_triangle(struct draw_list*, struct vec2 a, struct vec2 b, struct vec2 c, struct color);
  NK_API void draw_list_fill_circle(struct draw_list*, struct vec2 center, float radius, struct color col, unsigned int segs);
  NK_API void draw_list_fill_poly_convex(struct draw_list*, const struct vec2* points, const unsigned int count, struct color, enum anti_aliasing);

  /* misc */
  NK_API void draw_list_add_image(struct draw_list*, struct image texture, struct rect rect, struct color);
  NK_API void draw_list_add_text(struct draw_list*, const struct user_font*, struct rect, const char* text, int len, float font_height, struct color);
#ifdef NK_INCLUDE_COMMAND_USERDATA
  NK_API void draw_list_push_userdata(struct draw_list*, resource_handle userdata);
#endif

#endif

  /* ===============================================================
   *
   *                          GUI
   *
   * ===============================================================*/
  NK_API style_item style_item_color(color);
  NK_API style_item style_item_image(struct image img);
  NK_API style_item style_item_nine_slice(nine_slice slice);
  NK_API style_item style_item_hide(void);


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
  NK_API bool init_fixed(context*, void* memory, std::size_t size, const user_font*);

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
  NK_API bool init_custom(context*, memory_buffer* cmds, memory_buffer* pool, const user_font*);

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
  NK_API void clear(context*);

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
  NK_API void free(context*);

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
  NK_API void set_user_data(struct context*, resource_handle handle);
#endif

  /* =============================================================================
   *
   *                                  MATH
   *
   * ============================================================================= */
  NK_API hash murmur_hash(const void* key, int len, hash seed);
  NK_API void triangle_from_direction(vec2f* result, rectf r, float pad_x, float pad_y, heading);

  NK_API vec2f vec2_from_floats(float x, float y);
  NK_API vec2f vec2i_from_ints(int x, int y);
  NK_API vec2f vec2v(const float* xy);
  NK_API vec2f vec2iv(const int* xy);

  NK_API rectf get_null_rect(void);
  NK_API rectf rect(float x, float y, float w, float h);
  NK_API rectf recti(int x, int y, int w, int h);
  NK_API rectf recta(vec2f pos, vec2f size);
  NK_API rectf rectv(const float* xywh);
  NK_API rectf rectiv(const int* xywh);
  NK_API vec2f rect_pos(rectf);
  NK_API vec2f rect_size(rectf);


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

}

#endif