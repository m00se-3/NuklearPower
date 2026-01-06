#include <nk/nuklear.hpp>

namespace nk {
  /* ===============================================================
   *
   *                              UTF-8
   *
   * ===============================================================*/
  NK_GLOBAL const std::uint8_t utfbyte[NK_UTF_SIZE + 1] = {0x80, 0, 0xC0, 0xE0, 0xF0};
  NK_GLOBAL const std::uint8_t utfmask[NK_UTF_SIZE + 1] = {0xC0, 0x80, 0xE0, 0xF0, 0xF8};
  NK_GLOBAL const std::uint32_t utfmin[NK_UTF_SIZE + 1] = {0, 0, 0x80, 0x800, 0x10000};
  NK_GLOBAL const std::uint32_t utfmax[NK_UTF_SIZE + 1] = {0x10FFFF, 0x7F, 0x7FF, 0xFFFF, 0x10FFFF};

  INTERN int
  utf_validate(rune* u, int i) {
    NK_ASSERT(u);
    if (!u)
      return 0;
    if (!NK_BETWEEN(*u, utfmin[i], utfmax[i]) ||
        NK_BETWEEN(*u, 0xD800, 0xDFFF))
      *u = NK_UTF_INVALID;
    for (i = 1; *u > utfmax[i]; ++i)
      ;
    return i;
  }
  INTERN rune
  utf_decode_byte(const char c, int* i) {
    NK_ASSERT(i);
    if (!i)
      return 0;
    for (*i = 0; *i < (int) NK_LEN(utfmask); ++(*i)) {
      if (((std::uint8_t) c & utfmask[*i]) == utfbyte[*i])
        return (std::uint8_t) (c & ~utfmask[*i]);
    }
    return 0;
  }
  NK_API int
  utf_decode(const char* c, rune* u, const int clen) {
    int i, j, len, type = 0;

    NK_ASSERT(c);
    NK_ASSERT(u);

    if (!c || !u)
      return 0;
    if (!clen)
      return 0;
    *u = NK_UTF_INVALID;

    rune udecoded = utf_decode_byte(c[0], &len);
    if (!NK_BETWEEN(len, 1, NK_UTF_SIZE))
      return 1;

    for (i = 1, j = 1; i < clen && j < len; ++i, ++j) {
      udecoded = (udecoded << 6) | utf_decode_byte(c[i], &type);
      if (type != 0)
        return j;
    }
    if (j < len)
      return 0;
    *u = udecoded;
    utf_validate(u, len);
    return len;
  }
  INTERN char
  utf_encode_byte(const rune u, const int i) {
    return (char) ((utfbyte[i]) | ((std::uint8_t) u & ~utfmask[i]));
  }
  NK_API int
  utf_encode(rune u, char* c, const int clen) {
    const int len = utf_validate(&u, 0);
    if (clen < len || !len || len > NK_UTF_SIZE)
      return 0;

    for (int i = len - 1; i != 0; --i) {
      c[i] = utf_encode_byte(u, 0);
      u >>= 6;
    }
    c[0] = utf_encode_byte(u, len);
    return len;
  }
  NK_API int
  utf_len(const char* str, const int len) {
    int glyphs = 0;
    int src_len = 0;
    rune unicode;

    NK_ASSERT(str);
    if (!str || !len)
      return 0;

    const char* text = str;
    const int text_len = len;
    int glyph_len = utf_decode(text, &unicode, text_len);
    while (glyph_len && src_len < len) {
      glyphs++;
      src_len = src_len + glyph_len;
      glyph_len = utf_decode(text + src_len, &unicode, text_len - src_len);
    }
    return glyphs;
  }
  NK_API const char*
  utf_at(const char* buffer, const int length, const int index,
         rune* unicode, int* len) {
    int i = 0;
    int src_len = 0;
    int glyph_len = 0;

    NK_ASSERT(buffer);
    NK_ASSERT(unicode);
    NK_ASSERT(len);

    if (!buffer || !unicode || !len)
      return 0;
    if (index < 0) {
      *unicode = NK_UTF_INVALID;
      *len = 0;
      return 0;
    }

    const char* text = buffer;
    const int text_len = length;
    glyph_len = utf_decode(text, unicode, text_len);
    while (glyph_len) {
      if (i == index) {
        *len = glyph_len;
        break;
      }

      i++;
      src_len = src_len + glyph_len;
      glyph_len = utf_decode(text + src_len, unicode, text_len - src_len);
    }
    if (i != index)
      return 0;
    return buffer + src_len;
  }
} // namespace nk
