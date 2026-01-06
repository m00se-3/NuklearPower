#ifndef NK_POWER_WINDOW_HPP
#define NK_POWER_WINDOW_HPP

#include <internal/nuklear_internal.hpp>

namespace nk {

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
  NK_API bool begin(context* ctx, const char* title, rectf bounds, flag flags);

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
  NK_API bool begin_titled(context* ctx, const char* name, const char* title, rectf bounds, flag flags);

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
  NK_API void end(context* ctx);

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
  NK_API struct window* window_find(const context* ctx, const char* name);

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
  NK_API rectf window_get_bounds(const context* ctx);

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
  NK_API vec2f window_get_position(const context* ctx);

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
  NK_API vec2f window_get_size(const context* ctx);

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
  NK_API float window_get_width(const context* ctx);

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
  NK_API float window_get_height(const context* ctx);

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
  NK_API panel* window_get_panel(const context* ctx);

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
  NK_API rectf window_get_content_region(const context* ctx);

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
  NK_API vec2f window_get_content_region_min(const context* ctx);

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
  NK_API vec2f window_get_content_region_max(const context* ctx);

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
  NK_API vec2f window_get_content_region_size(const context* ctx);

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
  NK_API command_buffer* window_get_canvas(const context* ctx);

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
  NK_API void window_get_scroll(const context* ctx, unsigned int* offset_x, unsigned int* offset_y);

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
  NK_API bool window_has_focus(const context* ctx);

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
  NK_API bool window_is_hovered(const context* ctx);

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
  NK_API bool window_is_collapsed(const context* ctx, const char* name);

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
  NK_API bool window_is_closed(const context* ctx, const char* name);

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
  NK_API bool window_is_hidden(const context* ctx, const char* name);

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
  NK_API bool window_is_active(const context* ctx, const char* name);

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
  NK_API bool window_is_any_hovered(const context* ctx);

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
  NK_API bool item_is_any_active(const context* ctx);

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
  NK_API void window_set_bounds(context* ctx, const char* name, rectf bounds);

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
  NK_API void window_set_position(context* ctx, const char* name, vec2f pos);

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
  NK_API void window_set_size(context* ctx, const char* name, vec2f size);

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
  NK_API void window_set_focus(context* ctx, const char* name);

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
  NK_API void window_set_scroll(context* ctx, std::uint32_t offset_x, std::uint32_t offset_y);

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
  NK_API void window_close(context* ctx, const char* name);

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
  NK_API void window_collapse(context* ctx, const char* name, nk::collapse_states state);

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
  NK_API void window_collapse_if(context* ctx, const char* name, nk::collapse_states state, int cond);

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
  NK_API void window_show(context* ctx, const char* name, nk::show_states state);

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
  NK_API void window_show_if(context* ctx, const char* name, nk::show_states state, int cond);

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
  NK_API void rule_horizontal(context* ctx, color color, bool rounding);

  NK_LIB bool is_lower(int c);
  NK_LIB bool is_upper(int c);
  NK_LIB int to_upper(int c);
  NK_LIB int to_lower(int c);

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
  NK_LIB void* malloc(resource_handle unused, void* old, std::size_t size);
  NK_LIB void mfree(resource_handle unused, void* ptr);
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


  NK_LIB void* create_window(context* ctx);
  NK_LIB void remove_window(context*, window*);
  NK_LIB void free_window(context* ctx, window* win);
  NK_LIB window* find_window(const context* ctx, hash hash, const char* name);
  NK_LIB void insert_window(context* ctx, window* win, window_insert_location loc);

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
}

#endif