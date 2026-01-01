#include <cstring>
#include "nuklear.h"
#include "nuklear_internal.h"

namespace nk {
  /* ===============================================================
   *
   *                              STRING
   *
   * ===============================================================*/
#ifdef NK_INCLUDE_DEFAULT_ALLOCATOR
  NK_API void
  str_init_default(struct str* str) {
    struct allocator alloc;
    alloc.userdata.ptr = 0;
    alloc.alloc = malloc;
    alloc.free = mfree;
    buffer_init(&str->buffer, &alloc, 32);
    str->len = 0;
  }
#endif

  NK_API void
  str_init(struct str* str, const struct allocator* alloc, std::size_t size) {
    buffer_init(&str->buffer, alloc, size);
    str->len = 0;
  }
  NK_API void
  str_init_fixed(struct str* str, void* memory, std::size_t size) {
    buffer_init_fixed(&str->buffer, memory, size);
    str->len = 0;
  }
  NK_API int
  str_append_text_char(struct str* s, const char* str, int len) {
    char* mem;
    NK_ASSERT(s);
    NK_ASSERT(str);
    if (!s || !str || !len)
      return 0;
    mem = (char*) buffer_alloc(&s->buffer, buffer_allocation_type::BUFFER_FRONT, static_cast<std::size_t>(len) * sizeof(char), 0);
    if (!mem)
      return 0;
    std::memcpy(mem, str, static_cast<std::size_t>(len) * sizeof(char));
    s->len += utf_len(str, len);
    return len;
  }
  NK_API int
  str_append_str_char(struct str* s, const char* str) {
    return str_append_text_char(s, str, strlen(str));
  }
  NK_API int
  str_append_text_utf8(struct str* str, const char* text, int len) {
    int i = 0;
    int byte_len = 0;
    rune unicode;
    if (!str || !text || !len)
      return 0;
    for (i = 0; i < len; ++i)
      byte_len += utf_decode(text + byte_len, &unicode, 4);
    str_append_text_char(str, text, byte_len);
    return len;
  }
  NK_API int
  str_append_str_utf8(struct str* str, const char* text) {
    int byte_len = 0;
    int num_runes = 0;
    int glyph_len = 0;
    rune unicode;
    if (!str || !text)
      return 0;

    glyph_len = byte_len = utf_decode(text + byte_len, &unicode, 4);
    while (unicode != '\0' && glyph_len) {
      glyph_len = utf_decode(text + byte_len, &unicode, 4);
      byte_len += glyph_len;
      num_runes++;
    }
    str_append_text_char(str, text, byte_len);
    return num_runes;
  }
  NK_API int
  str_append_text_runes(struct str* str, const rune* text, int len) {
    int i = 0;
    int byte_len = 0;
    glyph glyph;

    NK_ASSERT(str);
    if (!str || !text || !len)
      return 0;
    for (i = 0; i < len; ++i) {
      byte_len = utf_encode(text[i], glyph, NK_UTF_SIZE);
      if (!byte_len)
        break;
      str_append_text_char(str, glyph, byte_len);
    }
    return len;
  }
  NK_API int
  str_append_str_runes(struct str* str, const rune* runes) {
    int i = 0;
    glyph glyph;
    int byte_len;
    NK_ASSERT(str);
    if (!str || !runes)
      return 0;
    while (runes[i] != '\0') {
      byte_len = utf_encode(runes[i], glyph, NK_UTF_SIZE);
      str_append_text_char(str, glyph, byte_len);
      i++;
    }
    return i;
  }
  NK_API int
  str_insert_at_char(struct str* s, int pos, const char* str, int len) {
    int i;
    void* mem;
    char* src;
    char* dst;

    int copylen;
    NK_ASSERT(s);
    NK_ASSERT(str);
    NK_ASSERT(len >= 0);
    if (!s || !str || !len || (std::size_t) pos > s->buffer.allocated)
      return 0;
    if ((s->buffer.allocated + (std::size_t) len >= s->buffer.memory.size) &&
        (s->buffer.type == allocation_type::BUFFER_FIXED))
      return 0;

    copylen = (int) s->buffer.allocated - pos;
    if (!copylen) {
      str_append_text_char(s, str, len);
      return 1;
    }
    mem = buffer_alloc(&s->buffer, buffer_allocation_type::BUFFER_FRONT, (std::size_t) len * sizeof(char), 0);
    if (!mem)
      return 0;

    /* memmove */
    NK_ASSERT(((int) pos + (int) len + ((int) copylen - 1)) >= 0);
    NK_ASSERT(((int) pos + ((int) copylen - 1)) >= 0);
    dst = ptr_add(char, s->buffer.memory.ptr, pos + len + (copylen - 1));
    src = ptr_add(char, s->buffer.memory.ptr, pos + (copylen - 1));
    for (i = 0; i < copylen; ++i)
      *dst-- = *src--;
    mem = ptr_add(void, s->buffer.memory.ptr, pos);
    std::memcpy(mem, str, (std::size_t) len * sizeof(char));
    s->len = utf_len((char*) s->buffer.memory.ptr, (int) s->buffer.allocated);
    return 1;
  }
  NK_API int
  str_insert_at_rune(struct str* str, int pos, const char* cstr, int len) {
    int glyph_len;
    rune unicode;
    const char* begin;
    const char* buffer;

    NK_ASSERT(str);
    NK_ASSERT(cstr);
    NK_ASSERT(len);
    if (!str || !cstr || !len)
      return 0;
    begin = str_at_rune(str, pos, &unicode, &glyph_len);
    if (!str->len)
      return str_append_text_char(str, cstr, len);
    buffer = str_get_const(str);
    if (!begin)
      return 0;
    return str_insert_at_char(str, (int) (begin - buffer), cstr, len);
  }
  NK_API int
  str_insert_text_char(struct str* str, int pos, const char* text, int len) {
    return str_insert_text_utf8(str, pos, text, len);
  }
  NK_API int
  str_insert_str_char(struct str* str, int pos, const char* text) {
    return str_insert_text_utf8(str, pos, text, strlen(text));
  }
  NK_API int
  str_insert_text_utf8(struct str* str, int pos, const char* text, int len) {
    int i = 0;
    int byte_len = 0;
    rune unicode;

    NK_ASSERT(str);
    NK_ASSERT(text);
    if (!str || !text || !len)
      return 0;
    for (i = 0; i < len; ++i)
      byte_len += utf_decode(text + byte_len, &unicode, 4);
    str_insert_at_rune(str, pos, text, byte_len);
    return len;
  }
  NK_API int
  str_insert_str_utf8(struct str* str, int pos, const char* text) {
    int byte_len = 0;
    int num_runes = 0;
    int glyph_len = 0;
    rune unicode;
    if (!str || !text)
      return 0;

    glyph_len = byte_len = utf_decode(text + byte_len, &unicode, 4);
    while (unicode != '\0' && glyph_len) {
      glyph_len = utf_decode(text + byte_len, &unicode, 4);
      byte_len += glyph_len;
      num_runes++;
    }
    str_insert_at_rune(str, pos, text, byte_len);
    return num_runes;
  }
  NK_API int
  str_insert_text_runes(struct str* str, int pos, const rune* runes, int len) {
    int i = 0;
    int byte_len = 0;
    glyph glyph;

    NK_ASSERT(str);
    if (!str || !runes || !len)
      return 0;
    for (i = 0; i < len; ++i) {
      byte_len = utf_encode(runes[i], glyph, NK_UTF_SIZE);
      if (!byte_len)
        break;
      str_insert_at_rune(str, pos + i, glyph, byte_len);
    }
    return len;
  }
  NK_API int
  str_insert_str_runes(struct str* str, int pos, const rune* runes) {
    int i = 0;
    glyph glyph;
    int byte_len;
    NK_ASSERT(str);
    if (!str || !runes)
      return 0;
    while (runes[i] != '\0') {
      byte_len = utf_encode(runes[i], glyph, NK_UTF_SIZE);
      str_insert_at_rune(str, pos + i, glyph, byte_len);
      i++;
    }
    return i;
  }
  NK_API void
  str_remove_chars(struct str* s, int len) {
    NK_ASSERT(s);
    NK_ASSERT(len >= 0);
    if (!s || len < 0 || (std::size_t) len > s->buffer.allocated)
      return;
    NK_ASSERT(((int) s->buffer.allocated - (int) len) >= 0);
    s->buffer.allocated -= (std::size_t) len;
    s->len = utf_len((char*) s->buffer.memory.ptr, (int) s->buffer.allocated);
  }
  NK_API void
  str_remove_runes(struct str* str, int len) {
    int index;
    const char* begin;
    const char* end;
    rune unicode;

    NK_ASSERT(str);
    NK_ASSERT(len >= 0);
    if (!str || len < 0)
      return;
    if (len >= str->len) {
      str->len = 0;
      return;
    }

    index = str->len - len;
    begin = str_at_rune(str, index, &unicode, &len);
    end = (const char*) str->buffer.memory.ptr + str->buffer.allocated;
    str_remove_chars(str, (int) (end - begin) + 1);
  }
  NK_API void
  str_delete_chars(struct str* s, int pos, int len) {
    NK_ASSERT(s);
    if (!s || !len || (std::size_t) pos > s->buffer.allocated ||
        (std::size_t) (pos + len) > s->buffer.allocated)
      return;

    if ((std::size_t) (pos + len) < s->buffer.allocated) {
      /* memmove */
      char* dst = ptr_add(char, s->buffer.memory.ptr, pos);
      char* src = ptr_add(char, s->buffer.memory.ptr, pos + len);
      std::memcpy(dst, src, s->buffer.allocated - (std::size_t) (pos + len));
      NK_ASSERT(((int) s->buffer.allocated - (int) len) >= 0);
      s->buffer.allocated -= (std::size_t) len;
    } else
      str_remove_chars(s, len);
    s->len = utf_len((char*) s->buffer.memory.ptr, (int) s->buffer.allocated);
  }
  NK_API void
  str_delete_runes(struct str* s, int pos, int len) {
    char* temp;
    rune unicode;
    char* begin;
    char* end;
    int unused;

    NK_ASSERT(s);
    NK_ASSERT(s->len >= pos + len);
    if (s->len < pos + len)
      len = NK_CLAMP(0, (s->len - pos), s->len);
    if (!len)
      return;

    temp = (char*) s->buffer.memory.ptr;
    begin = str_at_rune(s, pos, &unicode, &unused);
    if (!begin)
      return;
    s->buffer.memory.ptr = begin;
    end = str_at_rune(s, len, &unicode, &unused);
    s->buffer.memory.ptr = temp;
    if (!end)
      return;
    str_delete_chars(s, (int) (begin - temp), (int) (end - begin));
  }
  NK_API char*
  str_at_char(struct str* s, int pos) {
    NK_ASSERT(s);
    if (!s || pos > (int) s->buffer.allocated)
      return 0;
    return ptr_add(char, s->buffer.memory.ptr, pos);
  }
  NK_API char*
  str_at_rune(struct str* str, int pos, rune* unicode, int* len) {
    int i = 0;
    int src_len = 0;
    int glyph_len = 0;
    char* text;
    int text_len;

    NK_ASSERT(str);
    NK_ASSERT(unicode);
    NK_ASSERT(len);

    if (!str || !unicode || !len)
      return 0;
    if (pos < 0) {
      *unicode = 0;
      *len = 0;
      return 0;
    }

    text = (char*) str->buffer.memory.ptr;
    text_len = (int) str->buffer.allocated;
    glyph_len = utf_decode(text, unicode, text_len);
    while (glyph_len) {
      if (i == pos) {
        *len = glyph_len;
        break;
      }

      i++;
      src_len = src_len + glyph_len;
      glyph_len = utf_decode(text + src_len, unicode, text_len - src_len);
    }
    if (i != pos)
      return 0;
    return text + src_len;
  }
  NK_API const char*
  str_at_char_const(const struct str* s, int pos) {
    NK_ASSERT(s);
    if (!s || pos > (int) s->buffer.allocated)
      return 0;
    return ptr_add(char, s->buffer.memory.ptr, pos);
  }
  NK_API const char*
  str_at_const(const struct str* str, int pos, rune* unicode, int* len) {
    int i = 0;
    int src_len = 0;
    int glyph_len = 0;
    char* text;
    int text_len;

    NK_ASSERT(str);
    NK_ASSERT(unicode);
    NK_ASSERT(len);

    if (!str || !unicode || !len)
      return 0;
    if (pos < 0) {
      *unicode = 0;
      *len = 0;
      return 0;
    }

    text = (char*) str->buffer.memory.ptr;
    text_len = (int) str->buffer.allocated;
    glyph_len = utf_decode(text, unicode, text_len);
    while (glyph_len) {
      if (i == pos) {
        *len = glyph_len;
        break;
      }

      i++;
      src_len = src_len + glyph_len;
      glyph_len = utf_decode(text + src_len, unicode, text_len - src_len);
    }
    if (i != pos)
      return 0;
    return text + src_len;
  }
  NK_API rune
  str_rune_at(const struct str* str, int pos) {
    int len;
    rune unicode = 0;
    str_at_const(str, pos, &unicode, &len);
    return unicode;
  }
  NK_API char*
  str_get(struct str* s) {
    NK_ASSERT(s);
    if (!s || !s->len || !s->buffer.allocated)
      return 0;
    return (char*) s->buffer.memory.ptr;
  }
  NK_API const char*
  str_get_const(const struct str* s) {
    NK_ASSERT(s);
    if (!s || !s->len || !s->buffer.allocated)
      return 0;
    return (const char*) s->buffer.memory.ptr;
  }
  NK_API int
  str_len(const struct str* s) {
    NK_ASSERT(s);
    if (!s || !s->len || !s->buffer.allocated)
      return 0;
    return s->len;
  }
  NK_API int
  str_len_char(const struct str* s) {
    NK_ASSERT(s);
    if (!s || !s->len || !s->buffer.allocated)
      return 0;
    return (int) s->buffer.allocated;
  }
  NK_API void
  str_clear(struct str* str) {
    NK_ASSERT(str);
    buffer_clear(&str->buffer);
    str->len = 0;
  }
  NK_API void
  str_free(struct str* str) {
    NK_ASSERT(str);
    buffer_free(&str->buffer);
    str->len = 0;
  }
} // namespace nk
