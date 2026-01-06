#ifndef NK_POWER_GROUP_HPP
#define NK_POWER_GROUP_HPP

#include <internal/nuklear_internal.hpp>

namespace nk {

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
  NK_API bool group_begin(context*, const char* title, flag);

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
  NK_API bool group_begin_titled(context*, const char* name, const char* title, flag);

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
  NK_API void group_end(context*);

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
  NK_API bool group_scrolled_offset_begin(context*, std::uint32_t* x_offset, std::uint32_t* y_offset, const char* title, flag flags);

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
  NK_API bool group_scrolled_begin(context*, scroll* off, const char* title, flag);

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
  NK_API void group_scrolled_end(context*);

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
  NK_API void group_get_scroll(context*, const char* id, std::uint32_t* x_offset, std::uint32_t* y_offset);

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
  NK_API void group_set_scroll(context*, const char* id, std::uint32_t x_offset, std::uint32_t y_offset);

}

#endif