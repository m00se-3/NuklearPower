#ifndef NK_POWER_LAYOUT_HPP
#define NK_POWER_LAYOUT_HPP

#include <internal/nuklear_internal.hpp>

namespace nk {

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
  NK_API void layout_set_min_row_height(context*, float height);

  /**
   * Reset the currently used minimum row height back to `font_height + text_padding + padding`
   * ```c
   * void layout_reset_min_row_height(struct context*);
   * ```
   *
   * \param[in] ctx     | Must point to an previously initialized `context` struct after call `begin_xxx`
   */
  NK_API void layout_reset_min_row_height(context*);

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
  NK_API rectf layout_widget_bounds(const context* ctx);

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
  NK_API float layout_ratio_from_pixel(const context* ctx, float pixel_width);

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
  NK_API void layout_row_dynamic(context* ctx, float height, int cols);

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
  NK_API void layout_row_static(context* ctx, float height, int item_width, int cols);

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
  NK_API void layout_row_begin(context* ctx, nk::layout_format fmt, float row_height, int cols);

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
  NK_API void layout_row_push(context*, float value);

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
  NK_API void layout_row_end(context*);

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
  NK_API void layout_row(context*, nk::layout_format, float height, int cols, const float* ratio);

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
  NK_API void layout_row_template_begin(context*, float row_height);

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
  NK_API void layout_row_template_push_dynamic(context*);

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
  NK_API void layout_row_template_push_variable(context*, float min_width);

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
  NK_API void layout_row_template_push_static(context*, float width);

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
  NK_API void layout_row_template_end(context*);

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
  NK_API void layout_space_begin(context*, nk::layout_format, float height, int widget_count);

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
  NK_API void layout_space_push(context*, rectf bounds);

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
  NK_API void layout_space_end(context*);

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
  NK_API rectf layout_space_bounds(const context* ctx);

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
  NK_API vec2f layout_space_to_screen(const context* ctx, vec2f vec);

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
  NK_API vec2f layout_space_to_local(const context* ctx, vec2f vec);

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
  NK_API rectf layout_space_rect_to_screen(const context* ctx, rectf bounds);

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
  NK_API rectf layout_space_rect_to_local(const context* ctx, rectf bounds);

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
  NK_API void spacer(context* ctx);

  /* layout */
  NK_LIB float layout_row_calculate_usable_space(const style* style, panel_type::value_type type, float total_space, int columns);
  NK_LIB void panel_layout(const context* ctx, window* win, float height, int cols);
  NK_LIB void row_layout(context* ctx, layout_format fmt, float height, int cols, int width);
  NK_LIB void panel_alloc_row(const context* ctx, window* win);
  NK_LIB void layout_widget_space(rectf* bounds, const context* ctx, window* win, int modify);
  NK_LIB void panel_alloc_space(rectf* bounds, const context* ctx);
  NK_LIB void layout_peek(rectf* bounds, const context* ctx);



}

#endif