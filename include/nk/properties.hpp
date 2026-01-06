#ifndef NK_POWER_PROPERTIES_HPP
#define NK_POWER_PROPERTIES_HPP

#include <internal/nuklear_internal.hpp>

namespace nk {

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
  NK_API void property_int(context*, const char* name, int min, int* val, int max, int step, float inc_per_pixel);

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
  NK_API void property_float(context*, const char* name, float min, float* val, float max, float step, float inc_per_pixel);

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
  NK_API void property_double(context*, const char* name, double min, double* val, double max, double step, float inc_per_pixel);

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
  NK_API int propertyi(context*, const char* name, int min, int val, int max, int step, float inc_per_pixel);

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
  NK_API float propertyf(context*, const char* name, float min, float val, float max, float step, float inc_per_pixel);

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
  NK_API double propertyd(context*, const char* name, double min, double val, double max, double step, float inc_per_pixel);

  NK_LIB property_variant property_variant_int(int value, int min_value, int max_value, int step);
  NK_LIB property_variant property_variant_float(float value, float min_value, float max_value, float step);
  NK_LIB property_variant property_variant_double(double value, double min_value, double max_value, double step);

  NK_LIB void drag_behavior(flag* state, const input* in, rectf drag, property_variant* variant, float inc_per_pixel);
  NK_LIB void property_behavior(flag* ws, const input* in, rectf property, rectf label, rectf edit, rectf empty, int* state, property_variant* variant, float inc_per_pixel);
  NK_LIB void draw_property(command_buffer* out, const style_property* style, const rectf* bounds, const rectf* label, flag state, const char* name, int len, const user_font* font);
  NK_LIB void do_property(flag* ws, command_buffer* out, rectf property, const char* name, property_variant* variant, float inc_per_pixel, char* buffer, int* len, int* state, int* cursor, int* select_begin, int* select_end, const style_property* style, property_filter filter, input* in, const user_font* font, text_edit* text_edit, btn_behavior behavior);
  NK_LIB void property(context* ctx, const char* name, property_variant* variant, float inc_per_pixel, const property_filter filter);

}

#endif