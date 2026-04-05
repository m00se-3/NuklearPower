#ifndef NK_POWER_STRING_HPP
#define NK_POWER_STRING_HPP

#include <internal/nuklear_internal.hpp>

namespace nk {

  /* =============================================================================
   *
   *                                  STRING
   *
   * ============================================================================= */
  int strlen(const char* str);
  int stricmp(const char* s1, const char* s2);
  int stricmpn(const char* s1, const char* s2, int n);
  int strtoi(const char* str, char** endptr);
  float strtof(const char* str, char** endptr);
#ifndef NK_STRTOD
#define NK_STRTOD strtod
  double strtod(const char* str, char** endptr);
#endif
  int strfilter(const char* text, const char* regexp);
  int strmatch_fuzzy_string(char const* str, char const* pattern, int* out_score);
  int strmatch_fuzzy_text(const char* txt, int txt_len, const char* pattern, int* out_score);
  /* =============================================================================
   *
   *                                  UTF-8
   *
   * ============================================================================= */
  int utf_decode(const char*, rune*, int);
  int utf_encode(rune, char*, int);
  int utf_len(const char*, int byte_len);
  const char* utf_at(const char* buffer, int length, int index, rune* unicode, int* len);


#ifdef NK_INCLUDE_DEFAULT_ALLOCATOR
  void str_init_default(struct str*);
#endif
  void str_init(str*, const allocator*, std::size_t size);
  void str_init_fixed(str*, void* memory, std::size_t size);
  void str_clear(str*);
  void str_free(str*);

  int str_append_text_char(str*, const char*, int);
  int str_append_str_char(str*, const char*);
  int str_append_text_utf8(str*, const char*, int);
  int str_append_str_utf8(str*, const char*);
  int str_append_text_runes(str*, const rune*, int);
  int str_append_str_runes(str*, const rune*);

  int str_insert_at_char(str*, int pos, const char*, int);
  int str_insert_at_rune(str*, int pos, const char*, int);

  int str_insert_text_char(str*, int pos, const char*, int);
  int str_insert_str_char(str*, int pos, const char*);
  int str_insert_text_utf8(str*, int pos, const char*, int);
  int str_insert_str_utf8(str*, int pos, const char*);
  int str_insert_text_runes(str*, int pos, const rune*, int);
  int str_insert_str_runes(str*, int pos, const rune*);

  void str_remove_chars(str*, int len);
  void str_remove_runes(str* str, int len);
  void str_delete_chars(str*, int pos, int len);
  void str_delete_runes(str*, int pos, int len);

  char* str_at_char(str*, int pos);
  char* str_at_rune(str*, int pos, rune* unicode, int* len);
  rune str_rune_at(const str*, int pos);
  const char* str_at_char_const(const str*, int pos);
  const char* str_at_const(const str*, int pos, rune* unicode, int* len);

  char* str_get(str*);
  const char* str_get_const(const str*);
  int str_len(const str*);
  int str_len_char(const str*);

}

#endif