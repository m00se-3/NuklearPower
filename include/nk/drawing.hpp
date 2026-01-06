#ifndef NK_POWER_DRAWING_HPP
#define NK_POWER_DRAWING_HPP

#include <internal/nuklear_internal.hpp>

namespace nk {

  /* ===============================================================
   *
   *                          DRAWING
   *
   * ===============================================================*/
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


  /** shape outlines */
  NK_API void stroke_line(command_buffer* b, float x0, float y0, float x1, float y1, float line_thickness, color);
  NK_API void stroke_curve(command_buffer*, float, float, float, float, float, float, float, float, float line_thickness, color);
  NK_API void stroke_rect(command_buffer*, rectf, float rounding, float line_thickness, color);
  NK_API void stroke_circle(command_buffer*, rectf, float line_thickness, color);
  NK_API void stroke_arc(command_buffer*, float cx, float cy, float radius, float a_min, float a_max, float line_thickness, color);
  NK_API void stroke_triangle(command_buffer*, float, float, float, float, float, float, float line_thichness, color);
  NK_API void stroke_polyline(command_buffer*, const float* points, int point_count, float line_thickness, color col);
  NK_API void stroke_polygon(command_buffer*, const float* points, int point_count, float line_thickness, color);

  /** filled shades */
  NK_API void fill_rect(command_buffer*, rectf, float rounding, color);
  NK_API void fill_rect_multi_color(command_buffer*, rectf, color left, color top, color right, color bottom);
  NK_API void fill_circle(command_buffer*, rectf, color);
  NK_API void fill_arc(command_buffer*, float cx, float cy, float radius, float a_min, float a_max, color);
  NK_API void fill_triangle(command_buffer*, float x0, float y0, float x1, float y1, float x2, float y2, color);
  NK_API void fill_polygon(command_buffer*, const float* points, int point_count, color);

  /** misc */
  NK_API void draw_image(command_buffer*, rectf, const struct image*, color);
  NK_API void draw_nine_slice(command_buffer*, rectf, const nine_slice*, color);
  NK_API void draw_text(command_buffer*, rectf, const char* text, int len, const user_font*, color, color);
  NK_API void push_scissor(command_buffer*, rectf);
  NK_API void push_custom(command_buffer*, rectf, command_custom_callback, resource_handle usr);

}

#endif