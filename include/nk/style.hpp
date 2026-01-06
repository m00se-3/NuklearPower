#ifndef NK_POWER_STYLE_HPP
#define NK_POWER_STYLE_HPP

#include <internal/nuklear_internal.hpp>

namespace nk {
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


  NK_API void style_default(context*);
  NK_API void style_from_table(context*, const color*);
  NK_API void style_load_cursor(context*, nk::style_cursor, const cursor*);
  NK_API void style_load_all_cursors(context*, const cursor*);
  NK_API const char* style_get_color_by_name(nk::style_colors);
  NK_API void style_set_font(context*, const user_font*);
  NK_API bool style_set_cursor(context*, nk::style_cursor);
  NK_API void style_show_cursor(context*);
  NK_API void style_hide_cursor(context*);

  NK_API bool style_push_font(context*, const user_font*);
  NK_API bool style_push_float(context*, float*, float);
  NK_API bool style_push_vec2(context*, vec2f*, vec2f);
  NK_API bool style_push_style_item(context*, style_item*, style_item);
  NK_API bool style_push_flags(context*, flag*, flag);
  NK_API bool style_push_color(context*, color*, color);

  NK_API bool style_pop_font(context*);
  NK_API bool style_pop_float(context*);
  NK_API bool style_pop_vec2(context*);
  NK_API bool style_pop_style_item(context*);
  NK_API bool style_pop_flags(context*);
  NK_API bool style_pop_color(context*);
  /* =============================================================================
   *
   *                                  COLOR
   *
   * ============================================================================= */
  NK_API color rgb(int r, int g, int b);
  NK_API color rgb_iv(const int* rgb);
  NK_API color rgb_bv(const std::byte* rgb);
  NK_API color rgb_f(float r, float g, float b);
  NK_API color rgb_fv(const float* rgb);
  NK_API color rgb_cf(colorf c);
  NK_API color rgb_hex(const char* rgb);
  NK_API color rgb_factor(color col, float factor);

  NK_API color rgba(int r, int g, int b, int a);
  NK_API color rgba_u32(std::uint32_t);
  NK_API color rgba_iv(const int* rgba);
  NK_API color rgba_bv(const std::byte* rgba);
  NK_API color rgba_f(float r, float g, float b, float a);
  NK_API color rgba_fv(const float* rgba);
  NK_API color rgba_cf(colorf c);
  NK_API color rgba_hex(const char* rgb);

  NK_API colorf hsva_colorf(float h, float s, float v, float a);
  NK_API colorf hsva_colorfv(const float* c);
  NK_API void colorf_hsva_f(float* out_h, float* out_s, float* out_v, float* out_a, colorf in);
  NK_API void colorf_hsva_fv(float* hsva, colorf in);

  NK_API color hsv(int h, int s, int v);
  NK_API color hsv_iv(const int* hsv);
  NK_API color hsv_bv(const std::byte* hsv);
  NK_API color hsv_f(float h, float s, float v);
  NK_API color hsv_fv(const float* hsv);

  NK_API color hsva(int h, int s, int v, int a);
  NK_API color hsva_iv(const int* hsva);
  NK_API color hsva_bv(const std::byte* hsva);
  NK_API color hsva_f(float h, float s, float v, float a);
  NK_API color hsva_fv(const float* hsva);

  /* color (conversion nuklear --> user) */
  NK_API void color_f(float* r, float* g, float* b, float* a, color);
  NK_API void color_fv(float* rgba_out, color);
  NK_API colorf color_cf(color);
  NK_API void color_d(double* r, double* g, double* b, double* a, color);
  NK_API void color_dv(double* rgba_out, color);

  NK_API std::uint32_t color_u32(color);
  NK_API void color_hex_rgba(char* output, color);
  NK_API void color_hex_rgb(char* output, color);

  NK_API void color_hsv_i(int* out_h, int* out_s, int* out_v, color);
  NK_API void color_hsv_b(std::byte* out_h, std::byte* out_s, std::byte* out_v, color);
  NK_API void color_hsv_iv(int* hsv_out, color);
  NK_API void color_hsv_bv(std::byte* hsv_out, color);
  NK_API void color_hsv_f(float* out_h, float* out_s, float* out_v, color);
  NK_API void color_hsv_fv(float* hsv_out, color);

  NK_API void color_hsva_i(int* h, int* s, int* v, int* a, color);
  NK_API void color_hsva_b(std::byte* h, std::byte* s, std::byte* v, std::byte* a, color);
  NK_API void color_hsva_iv(int* hsva_out, color);
  NK_API void color_hsva_bv(std::byte* hsva_out, color);
  NK_API void color_hsva_f(float* out_h, float* out_s, float* out_v, float* out_a, color);
  NK_API void color_hsva_fv(float* hsva_out, color);
}

#endif