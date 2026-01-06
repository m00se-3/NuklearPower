#ifndef NK_POWER_INPUT_HPP
#define NK_POWER_INPUT_HPP

#include <internal/nuklear_internal.hpp>

namespace nk {
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
  NK_API void input_begin(context*);

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
  NK_API void input_motion(context*, int x, int y);

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
  NK_API void input_scroll(context*, vec2f val);

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
  NK_API void input_char(context*, char);

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
  NK_API void input_glyph(context*, const glyph);

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
  NK_API void input_unicode(context*, rune);

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
  NK_API void input_end(context*);

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
  NK_API const struct command* _begin(context*);

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
  NK_API const command* _next(context*, const command*);

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
#define foreach(c, ctx) for ((c) = _begin(ctx); (c) != 0; (c) = _next(ctx, c))

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
  NK_API flag convert(struct context*, struct buffer* cmds, struct buffer* vertices, struct buffer* elements, const struct convert_config*);

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

#define draw_foreach(cmd, ctx, b) for ((cmd) = _draw_begin(ctx, b); (cmd) != 0; (cmd) = _draw_next(cmd, b, ctx))
#endif

  /* ===============================================================
   *
   *                          INPUT
   *
   * ===============================================================*/
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
  NK_API bool input_has_mouse_click(const input*, buttons);
  NK_API bool input_has_mouse_click_in_rect(const input*, buttons, rectf);
  NK_API bool input_has_mouse_click_in_button_rect(const input*, buttons, rectf);
  NK_API bool input_has_mouse_click_down_in_rect(const input*, buttons, rectf, bool down);
  NK_API bool input_is_mouse_click_in_rect(const input*, buttons, rectf);
  NK_API bool input_is_mouse_click_down_in_rect(const input* i, buttons id, rectf b, bool down);
  NK_API bool input_any_mouse_click_in_rect(const input*, rectf);
  NK_API bool input_is_mouse_prev_hovering_rect(const input*, rectf);
  NK_API bool input_is_mouse_hovering_rect(const input*, rectf);
  NK_API bool input_mouse_clicked(const input*, buttons, rectf);
  NK_API bool input_is_mouse_down(const input*, buttons);
  NK_API bool input_is_mouse_pressed(const input*, buttons);
  NK_API bool input_is_mouse_released(const input*, buttons);
  NK_API bool input_is_key_pressed(const input*, keys);
  NK_API bool input_is_key_released(const input*, keys);
  NK_API bool input_is_key_down(const input*, keys);

}

#endif