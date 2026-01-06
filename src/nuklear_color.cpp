#include <nk/nuklear.hpp>
#include <algorithm>

namespace nk {
  /* ==============================================================
   *
   *                          COLOR
   *
   * ===============================================================*/
  INTERN int
  parse_hex(const char* p, const int length) {
    int i = 0;
    int len = 0;
    while (len < length) {
      i <<= 4;
      if (p[len] >= 'a' && p[len] <= 'f')
        i += ((p[len] - 'a') + 10);
      else if (p[len] >= 'A' && p[len] <= 'F')
        i += ((p[len] - 'A') + 10);
      else
        i += (p[len] - '0');
      len++;
    }
    return i;
  }
  NK_API color
  rgb_factor(color col, float factor) {
    if (factor == 1.0f)
      return col;
    col.r = (std::uint8_t) (col.r * factor);
    col.g = (std::uint8_t) (col.g * factor);
    col.b = (std::uint8_t) (col.b * factor);
    return col;
  }
  NK_API color
  rgba(const int r, const int g, const int b, const int a) {
    color ret;
    ret.r = (std::uint8_t) std::clamp(0, r, 255);
    ret.g = (std::uint8_t) std::clamp(0, g, 255);
    ret.b = (std::uint8_t) std::clamp(0, b, 255);
    ret.a = (std::uint8_t) std::clamp(0, a, 255);
    return ret;
  }
  NK_API color
  rgb_hex(const char* rgb) {
    color col;
    const char* c = rgb;
    if (*c == '#')
      c++;
    col.r = (std::uint8_t) parse_hex(c, 2);
    col.g = (std::uint8_t) parse_hex(c + 2, 2);
    col.b = (std::uint8_t) parse_hex(c + 4, 2);
    col.a = 255;
    return col;
  }
  NK_API color
  rgba_hex(const char* rgb) {
    color col;
    const char* c = rgb;
    if (*c == '#')
      c++;
    col.r = (std::uint8_t) parse_hex(c, 2);
    col.g = (std::uint8_t) parse_hex(c + 2, 2);
    col.b = (std::uint8_t) parse_hex(c + 4, 2);
    col.a = (std::uint8_t) parse_hex(c + 6, 2);
    return col;
  }
  NK_API void
  color_hex_rgba(char* output, const color col) {
#define NK_TO_HEX(i) ((i) <= 9 ? '0' + (i) : 'A' - 10 + (i))
    output[0] = (char) NK_TO_HEX((col.r & 0xF0) >> 4);
    output[1] = (char) NK_TO_HEX((col.r & 0x0F));
    output[2] = (char) NK_TO_HEX((col.g & 0xF0) >> 4);
    output[3] = (char) NK_TO_HEX((col.g & 0x0F));
    output[4] = (char) NK_TO_HEX((col.b & 0xF0) >> 4);
    output[5] = (char) NK_TO_HEX((col.b & 0x0F));
    output[6] = (char) NK_TO_HEX((col.a & 0xF0) >> 4);
    output[7] = (char) NK_TO_HEX((col.a & 0x0F));
    output[8] = '\0';
#undef NK_TO_HEX
  }
  NK_API void
  color_hex_rgb(char* output, const color col) {
#define NK_TO_HEX(i) ((i) <= 9 ? '0' + (i) : 'A' - 10 + (i))
    output[0] = (char) NK_TO_HEX((col.r & 0xF0) >> 4);
    output[1] = (char) NK_TO_HEX((col.r & 0x0F));
    output[2] = (char) NK_TO_HEX((col.g & 0xF0) >> 4);
    output[3] = (char) NK_TO_HEX((col.g & 0x0F));
    output[4] = (char) NK_TO_HEX((col.b & 0xF0) >> 4);
    output[5] = (char) NK_TO_HEX((col.b & 0x0F));
    output[6] = '\0';
#undef NK_TO_HEX
  }
  NK_API color
  rgba_iv(const int* c) {
    return rgba(c[0], c[1], c[2], c[3]);
  }
  NK_API color
  rgba_bv(const std::uint8_t* c) {
    return rgba(c[0], c[1], c[2], c[3]);
  }
  NK_API color
  rgb(const int r, const int g, const int b) {
    color ret;
    ret.r = (std::uint8_t) std::clamp(0, r, 255);
    ret.g = (std::uint8_t) std::clamp(0, g, 255);
    ret.b = (std::uint8_t) std::clamp(0, b, 255);
    ret.a = (std::uint8_t) 255;
    return ret;
  }
  NK_API color
  rgb_iv(const int* c) {
    return rgb(c[0], c[1], c[2]);
  }
  NK_API color
  rgb_bv(const std::uint8_t* c) {
    return rgb(c[0], c[1], c[2]);
  }
  NK_API color
  rgba_u32(const unsigned int in) {
    color ret;
    ret.r = (in & 0xFF);
    ret.g = ((in >> 8) & 0xFF);
    ret.b = ((in >> 16) & 0xFF);
    ret.a = (std::uint8_t) ((in >> 24) & 0xFF);
    return ret;
  }
  NK_API color
  rgba_f(float r, float g, float b, float a) {
    color ret;
    ret.r = (std::uint8_t) (NK_SATURATE(r) * 255.0f);
    ret.g = (std::uint8_t) (NK_SATURATE(g) * 255.0f);
    ret.b = (std::uint8_t) (NK_SATURATE(b) * 255.0f);
    ret.a = (std::uint8_t) (NK_SATURATE(a) * 255.0f);
    return ret;
  }
  NK_API color
  rgba_fv(const float* c) {
    return rgba_f(c[0], c[1], c[2], c[3]);
  }
  NK_API color
  rgba_cf(const colorf c) {
    return rgba_f(c.r, c.g, c.b, c.a);
  }
  NK_API color
  rgb_f(float r, float g, float b) {
    color ret;
    ret.r = (std::uint8_t) (NK_SATURATE(r) * 255.0f);
    ret.g = (std::uint8_t) (NK_SATURATE(g) * 255.0f);
    ret.b = (std::uint8_t) (NK_SATURATE(b) * 255.0f);
    ret.a = 255;
    return ret;
  }
  NK_API color
  rgb_fv(const float* c) {
    return rgb_f(c[0], c[1], c[2]);
  }
  NK_API color
  rgb_cf(const colorf c) {
    return rgb_f(c.r, c.g, c.b);
  }
  NK_API color
  hsv(const int h, const int s, const int v) {
    return hsva(h, s, v, 255);
  }
  NK_API color
  hsv_iv(const int* c) {
    return hsv(c[0], c[1], c[2]);
  }
  NK_API color
  hsv_bv(const std::uint8_t* c) {
    return hsv(c[0], c[1], c[2]);
  }
  NK_API color
  hsv_f(float h, float s, float v) {
    return hsva_f(h, s, v, 1.0f);
  }
  NK_API color
  hsv_fv(const float* c) {
    return hsv_f(c[0], c[1], c[2]);
  }
  NK_API color
  hsva(const int h, const int s, const int v, const int a) {
    float hf = ((float) std::clamp(0, h, 255)) / 255.0f;
    float sf = ((float) std::clamp(0, s, 255)) / 255.0f;
    float vf = ((float) std::clamp(0, v, 255)) / 255.0f;
    float af = ((float) std::clamp(0, a, 255)) / 255.0f;
    return hsva_f(hf, sf, vf, af);
  }
  NK_API color
  hsva_iv(const int* c) {
    return hsva(c[0], c[1], c[2], c[3]);
  }
  NK_API color
  hsva_bv(const std::uint8_t* c) {
    return hsva(c[0], c[1], c[2], c[3]);
  }
  NK_API colorf
  hsva_colorf(float h, float s, float v, float a) {
    colorf out = {0, 0, 0, 0};
    if (s <= 0.0f) {
      out.r = v;
      out.g = v;
      out.b = v;
      out.a = a;
      return out;
    }
    h = h / (60.0f / 360.0f);
    const int i = (int) h;
    float f = h - (float) i;
    float p = v * (1.0f - s);
    float q = v * (1.0f - (s * f));
    float t = v * (1.0f - s * (1.0f - f));

    switch (i) {
      case 0:
      default:
        out.r = v;
        out.g = t;
        out.b = p;
        break;
      case 1:
        out.r = q;
        out.g = v;
        out.b = p;
        break;
      case 2:
        out.r = p;
        out.g = v;
        out.b = t;
        break;
      case 3:
        out.r = p;
        out.g = q;
        out.b = v;
        break;
      case 4:
        out.r = t;
        out.g = p;
        out.b = v;
        break;
      case 5:
        out.r = v;
        out.g = p;
        out.b = q;
        break;
    }
    out.a = a;
    return out;
  }
  NK_API colorf
  hsva_colorfv(const float* c) {
    return hsva_colorf(c[0], c[1], c[2], c[3]);
  }
  NK_API color
  hsva_f(float h, float s, float v, float a) {
    const colorf c = hsva_colorf(h, s, v, a);
    return rgba_f(c.r, c.g, c.b, c.a);
  }
  NK_API color
  hsva_fv(const float* c) {
    return hsva_f(c[0], c[1], c[2], c[3]);
  }
  NK_API unsigned int
  color_u32(const color in) {
    unsigned int out = (unsigned int) in.r;
    out |= ((unsigned int) in.g << 8);
    out |= ((unsigned int) in.b << 16);
    out |= ((unsigned int) in.a << 24);
    return out;
  }
  NK_API void
  color_f(float* r, float* g, float* b, float* a, const color in) {
    NK_STORAGE const float s = 1.0f / 255.0f;
    *r = (float) in.r * s;
    *g = (float) in.g * s;
    *b = (float) in.b * s;
    *a = (float) in.a * s;
  }
  NK_API void
  color_fv(float* c, const color in) {
    color_f(&c[0], &c[1], &c[2], &c[3], in);
  }
  NK_API colorf
  color_cf(const color in) {
    colorf o;
    color_f(&o.r, &o.g, &o.b, &o.a, in);
    return o;
  }
  NK_API void
  color_d(double* r, double* g, double* b, double* a, const color in) {
    NK_STORAGE const double s = 1.0 / 255.0;
    *r = (double) in.r * s;
    *g = (double) in.g * s;
    *b = (double) in.b * s;
    *a = (double) in.a * s;
  }
  NK_API void
  color_dv(double* c, const color in) {
    color_d(&c[0], &c[1], &c[2], &c[3], in);
  }
  NK_API void
  color_hsv_f(float* out_h, float* out_s, float* out_v, const color in) {
    float a;
    color_hsva_f(out_h, out_s, out_v, &a, in);
  }
  NK_API void
  color_hsv_fv(float* out, const color in) {
    float a;
    color_hsva_f(&out[0], &out[1], &out[2], &a, in);
  }
  NK_API void
  colorf_hsva_f(float* out_h, float* out_s,
                float* out_v, float* out_a, colorf in) {
    float K = 0.0f;
    if (in.g < in.b) {
      const float t = in.g;
      in.g = in.b;
      in.b = t;
      K = -1.f;
    }
    if (in.r < in.g) {
      const float t = in.r;
      in.r = in.g;
      in.g = t;
      K = -2.f / 6.0f - K;
    }
    float chroma = in.r - ((in.g < in.b) ? in.g : in.b);
    *out_h = NK_ABS(K + (in.g - in.b) / (6.0f * chroma + 1e-20f));
    *out_s = chroma / (in.r + 1e-20f);
    *out_v = in.r;
    *out_a = in.a;
  }
  NK_API void
  colorf_hsva_fv(float* hsva, const colorf in) {
    colorf_hsva_f(&hsva[0], &hsva[1], &hsva[2], &hsva[3], in);
  }
  NK_API void
  color_hsva_f(float* out_h, float* out_s,
               float* out_v, float* out_a, const color in) {
    colorf col;
    color_f(&col.r, &col.g, &col.b, &col.a, in);
    colorf_hsva_f(out_h, out_s, out_v, out_a, col);
  }
  NK_API void
  color_hsva_fv(float* out, const color in) {
    color_hsva_f(&out[0], &out[1], &out[2], &out[3], in);
  }
  NK_API void
  color_hsva_i(int* out_h, int* out_s, int* out_v,
               int* out_a, const color in) {
    float h, s, v, a;
    color_hsva_f(&h, &s, &v, &a, in);
    *out_h = (std::uint8_t) (h * 255.0f);
    *out_s = (std::uint8_t) (s * 255.0f);
    *out_v = (std::uint8_t) (v * 255.0f);
    *out_a = (std::uint8_t) (a * 255.0f);
  }
  NK_API void
  color_hsva_iv(int* out, const color in) {
    color_hsva_i(&out[0], &out[1], &out[2], &out[3], in);
  }
  NK_API void
  color_hsva_bv(std::uint8_t* out, const color in) {
    int tmp[4];
    color_hsva_i(&tmp[0], &tmp[1], &tmp[2], &tmp[3], in);
    out[0] = (std::uint8_t) tmp[0];
    out[1] = (std::uint8_t) tmp[1];
    out[2] = (std::uint8_t) tmp[2];
    out[3] = (std::uint8_t) tmp[3];
  }
  NK_API void
  color_hsva_b(std::uint8_t* h, std::uint8_t* s, std::uint8_t* v, std::uint8_t* a, const color in) {
    int tmp[4];
    color_hsva_i(&tmp[0], &tmp[1], &tmp[2], &tmp[3], in);
    *h = static_cast<std::uint8_t>(tmp[0]);
    *s = static_cast<std::uint8_t>(tmp[1]);
    *v = static_cast<std::uint8_t>(tmp[2]);
    *a = static_cast<std::uint8_t>(tmp[3]);
  }
  NK_API void
  color_hsv_i(int* out_h, int* out_s, int* out_v, const color in) {
    int a;
    color_hsva_i(out_h, out_s, out_v, &a, in);
  }
  NK_API void
  color_hsv_b(std::uint8_t* out_h, std::uint8_t* out_s, std::uint8_t* out_v, const color in) {
    int tmp[4];
    color_hsva_i(&tmp[0], &tmp[1], &tmp[2], &tmp[3], in);
    *out_h = static_cast<std::uint8_t>(tmp[0]);
    *out_s = static_cast<std::uint8_t>(tmp[1]);
    *out_v = static_cast<std::uint8_t>(tmp[2]);
  }
  NK_API void
  color_hsv_iv(int* out, const color in) {
    color_hsv_i(&out[0], &out[1], &out[2], in);
  }
  NK_API void
  color_hsv_bv(std::uint8_t* out, const color in) {
    int tmp[4];
    color_hsv_i(&tmp[0], &tmp[1], &tmp[2], in);
    out[0] = (std::uint8_t) tmp[0];
    out[1] = (std::uint8_t) tmp[1];
    out[2] = (std::uint8_t) tmp[2];
  }
} // namespace nk
