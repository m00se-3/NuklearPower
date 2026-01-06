#ifndef NK_POWER_STRING_HPP
#define NK_POWER_STRING_HPP

#include <internal/nuklear_internal.hpp>

namespace nk {

  /* =============================================================================
   *
   *                                  STRING
   *
   * ============================================================================= */
  NK_API int strlen(const char* str);
  NK_API int stricmp(const char* s1, const char* s2);
  NK_API int stricmpn(const char* s1, const char* s2, int n);
  NK_API int strtoi(const char* str, char** endptr);
  NK_API float strtof(const char* str, char** endptr);
#ifndef NK_STRTOD
#define NK_STRTOD strtod
  NK_API double strtod(const char* str, char** endptr);
#endif
  NK_API int strfilter(const char* text, const char* regexp);
  NK_API int strmatch_fuzzy_string(char const* str, char const* pattern, int* out_score);
  NK_API int strmatch_fuzzy_text(const char* txt, int txt_len, const char* pattern, int* out_score);
  /* =============================================================================
   *
   *                                  UTF-8
   *
   * ============================================================================= */
  NK_API int utf_decode(const char*, rune*, int);
  NK_API int utf_encode(rune, char*, int);
  NK_API int utf_len(const char*, int byte_len);
  NK_API const char* utf_at(const char* buffer, int length, int index, rune* unicode, int* len);


#ifdef NK_INCLUDE_DEFAULT_ALLOCATOR
  NK_API void str_init_default(struct str*);
#endif
  NK_API void str_init(str*, const allocator*, std::size_t size);
  NK_API void str_init_fixed(str*, void* memory, std::size_t size);
  NK_API void str_clear(str*);
  NK_API void str_free(str*);

  NK_API int str_append_text_char(str*, const char*, int);
  NK_API int str_append_str_char(str*, const char*);
  NK_API int str_append_text_utf8(str*, const char*, int);
  NK_API int str_append_str_utf8(str*, const char*);
  NK_API int str_append_text_runes(str*, const rune*, int);
  NK_API int str_append_str_runes(str*, const rune*);

  NK_API int str_insert_at_char(str*, int pos, const char*, int);
  NK_API int str_insert_at_rune(str*, int pos, const char*, int);

  NK_API int str_insert_text_char(str*, int pos, const char*, int);
  NK_API int str_insert_str_char(str*, int pos, const char*);
  NK_API int str_insert_text_utf8(str*, int pos, const char*, int);
  NK_API int str_insert_str_utf8(str*, int pos, const char*);
  NK_API int str_insert_text_runes(str*, int pos, const rune*, int);
  NK_API int str_insert_str_runes(str*, int pos, const rune*);

  NK_API void str_remove_chars(str*, int len);
  NK_API void str_remove_runes(str* str, int len);
  NK_API void str_delete_chars(str*, int pos, int len);
  NK_API void str_delete_runes(str*, int pos, int len);

  NK_API char* str_at_char(str*, int pos);
  NK_API char* str_at_rune(str*, int pos, rune* unicode, int* len);
  NK_API rune str_rune_at(const str*, int pos);
  NK_API const char* str_at_char_const(const str*, int pos);
  NK_API const char* str_at_const(const str*, int pos, rune* unicode, int* len);

  NK_API char* str_get(str*);
  NK_API const char* str_get_const(const str*);
  NK_API int str_len(const str*);
  NK_API int str_len_char(const str*);

}

#endif