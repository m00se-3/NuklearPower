#include <cstring>
#include "nuklear.h"
#include "nuklear_internal.h"

namespace nk {
  /* ===============================================================
   *
   *                              UTIL
   *
   * ===============================================================*/
  INTERN int str_match_here(const char* regexp, const char* text);
  INTERN int str_match_star(int c, const char* regexp, const char* text);
  NK_LIB bool is_lower(int c) { return (c >= 'a' && c <= 'z') || (c >= 0xE0 && c <= 0xFF); }
  NK_LIB bool is_upper(int c) { return (c >= 'A' && c <= 'Z') || (c >= 0xC0 && c <= 0xDF); }
  NK_LIB int to_upper(int c) { return (c >= 'a' && c <= 'z') ? (c - ('a' - 'A')) : c; }
  NK_LIB int to_lower(int c) { return (c >= 'A' && c <= 'Z') ? (c - ('a' + 'A')) : c; }

  NK_LIB void
  zero(void* ptr, std::size_t size) {
    NK_ASSERT(ptr);
    std::memset(ptr, 0, size);
  }
  NK_API int
  strlen(const char* str) {
    int siz = 0;
    NK_ASSERT(str);
    while ((str != nullptr) && *str++ != '\0')
      siz++;
    return siz;
  }
  NK_API int
  strtoi(const char* str, char** endptr) {
    int neg = 1;
    const char* p = str;
    int value = 0;

    NK_ASSERT(str);
    if (!str)
      return 0;

    /* skip whitespace */
    while (*p == ' ')
      p++;
    if (*p == '-') {
      neg = -1;
      p++;
    }
    while (*p && *p >= '0' && *p <= '9') {
      value = value * 10 + (int) (*p - '0');
      p++;
    }
    if (endptr)
      *endptr = (char*) p;
    return neg * value;
  }
  NK_API double
  strtod(const char* str, char** endptr) {
    double m;
    double neg = 1.0;
    char* p = (char*) str;
    double value = 0;
    double number = 0;

    NK_ASSERT(str);
    if (!str)
      return 0;

    /* skip whitespace */
    while (*p == ' ')
      p++;
    if (*p == '-') {
      neg = -1.0;
      p++;
    }

    while (*p && *p != '.' && *p != 'e') {
      value = value * 10.0 + (double) (*p - '0');
      p++;
    }

    if (*p == '.') {
      p++;
      for (m = 0.1; *p && *p != 'e'; p++) {
        value = value + (double) (*p - '0') * m;
        m *= 0.1;
      }
    }
    if (*p == 'e') {
      int i, pow, div;
      p++;
      if (*p == '-') {
        div = true;
        p++;
      } else if (*p == '+') {
        div = false;
        p++;
      } else
        div = false;

      for (pow = 0; *p; p++)
        pow = pow * 10 + (int) (*p - '0');

      for (m = 1.0, i = 0; i < pow; i++)
        m *= 10.0;

      if (div)
        value /= m;
      else
        value *= m;
    }
    number = value * neg;
    if (endptr)
      *endptr = p;
    return number;
  }
  NK_API float
  strtof(const char* str, char** endptr) {
    float float_value;
    double double_value;
    double_value = NK_STRTOD(str, endptr);
    float_value = (float) double_value;
    return float_value;
  }
  NK_API int
  stricmp(const char* s1, const char* s2) {
    int c1, c2, d;
    do {
      c1 = *s1++;
      c2 = *s2++;
      d = c1 - c2;
      while (d) {
        if (c1 <= 'Z' && c1 >= 'A') {
          d += ('a' - 'A');
          if (!d)
            break;
        }
        if (c2 <= 'Z' && c2 >= 'A') {
          d -= ('a' - 'A');
          if (!d)
            break;
        }
        return ((d >= 0) << 1) - 1;
      }
    } while (c1);
    return 0;
  }
  NK_API int
  stricmpn(const char* s1, const char* s2, int n) {
    int c1, c2, d;
    NK_ASSERT(n >= 0);
    do {
      c1 = *s1++;
      c2 = *s2++;
      if (!n--)
        return 0;

      d = c1 - c2;
      while (d) {
        if (c1 <= 'Z' && c1 >= 'A') {
          d += ('a' - 'A');
          if (!d)
            break;
        }
        if (c2 <= 'Z' && c2 >= 'A') {
          d -= ('a' - 'A');
          if (!d)
            break;
        }
        return ((d >= 0) << 1) - 1;
      }
    } while (c1);
    return 0;
  }
  INTERN int
  str_match_here(const char* regexp, const char* text) {
    if (regexp[0] == '\0')
      return 1;
    if (regexp[1] == '*')
      return str_match_star(regexp[0], regexp + 2, text);
    if (regexp[0] == '$' && regexp[1] == '\0')
      return *text == '\0';
    if (*text != '\0' && (regexp[0] == '.' || regexp[0] == *text))
      return str_match_here(regexp + 1, text + 1);
    return 0;
  }
  INTERN int
  str_match_star(int c, const char* regexp, const char* text) {
    do { /* a '* matches zero or more instances */
      if (str_match_here(regexp, text))
        return 1;
    } while (*text != '\0' && (*text++ == c || c == '.'));
    return 0;
  }
  NK_API int
  strfilter(const char* text, const char* regexp) {
    /*
    c    matches any literal character c
    .    matches any single character
    ^    matches the beginning of the input string
    $    matches the end of the input string
    *    matches zero or more occurrences of the previous character*/
    if (regexp[0] == '^')
      return str_match_here(regexp + 1, text);
    do { /* must look even if string is empty */
      if (str_match_here(regexp, text))
        return 1;
    } while (*text++ != '\0');
    return 0;
  }
  NK_API int
  strmatch_fuzzy_text(const char* str, int str_len,
                      const char* pattern, int* out_score) {
    /* Returns true if each character in pattern is found sequentially within str
     * if found then out_score is also set. Score value has no intrinsic meaning.
     * Range varies with pattern. Can only compare scores with same search pattern. */

    /* bonus for adjacent matches */
#define NK_ADJACENCY_BONUS 5
    /* bonus if match occurs after a separator */
#define NK_SEPARATOR_BONUS 10
    /* bonus if match is uppercase and prev is lower */
#define NK_CAMEL_BONUS 10
    /* penalty applied for every letter in str before the first match */
#define NK_LEADING_LETTER_PENALTY (-3)
    /* maximum penalty for leading letters */
#define NK_MAX_LEADING_LETTER_PENALTY (-9)
    /* penalty for every letter that doesn't matter */
#define NK_UNMATCHED_LETTER_PENALTY (-1)

    /* loop variables */
    int score = 0;
    char const* pattern_iter = pattern;
    int str_iter = 0;
    int prev_matched = false;
    int prev_lower = false;
    /* true so if first letter match gets separator bonus*/
    int prev_separator = true;

    /* use "best" matched letter if multiple string letters match the pattern */
    char const* best_letter = 0;
    int best_letter_score = 0;

    /* loop over strings */
    NK_ASSERT(str);
    NK_ASSERT(pattern);
    if (!str || !str_len || !pattern)
      return 0;
    while (str_iter < str_len) {
      const char pattern_letter = *pattern_iter;
      const char str_letter = str[str_iter];

      int next_match = *pattern_iter != '\0' &&
                       to_lower(pattern_letter) == to_lower(str_letter);
      int rematch = best_letter && to_upper(*best_letter) == to_upper(str_letter);

      int advanced = next_match && best_letter;
      int pattern_repeat = best_letter && *pattern_iter != '\0';
      pattern_repeat = pattern_repeat &&
                       to_lower(*best_letter) == to_lower(pattern_letter);

      if (advanced || pattern_repeat) {
        score += best_letter_score;
        best_letter = 0;
        best_letter_score = 0;
      }

      if (next_match || rematch) {
        int new_score = 0;
        /* Apply penalty for each letter before the first pattern match */
        if (pattern_iter == pattern) {
          int count = (int) (&str[str_iter] - str);
          int penalty = NK_LEADING_LETTER_PENALTY * count;
          if (penalty < NK_MAX_LEADING_LETTER_PENALTY)
            penalty = NK_MAX_LEADING_LETTER_PENALTY;

          score += penalty;
        }

        /* apply bonus for consecutive bonuses */
        if (prev_matched)
          new_score += NK_ADJACENCY_BONUS;

        /* apply bonus for matches after a separator */
        if (prev_separator)
          new_score += NK_SEPARATOR_BONUS;

        /* apply bonus across camel case boundaries */
        if (prev_lower && is_upper(str_letter))
          new_score += NK_CAMEL_BONUS;

        /* update pattern iter IFF the next pattern letter was matched */
        if (next_match)
          ++pattern_iter;

        /* update best letter in str which may be for a "next" letter or a rematch */
        if (new_score >= best_letter_score) {
          /* apply penalty for now skipped letter */
          if (best_letter != 0)
            score += NK_UNMATCHED_LETTER_PENALTY;

          best_letter = &str[str_iter];
          best_letter_score = new_score;
        }
        prev_matched = true;
      } else {
        score += NK_UNMATCHED_LETTER_PENALTY;
        prev_matched = false;
      }

      /* separators should be more easily defined */
      prev_lower = is_lower(str_letter) != 0;
      prev_separator = str_letter == '_' || str_letter == ' ';

      ++str_iter;
    }

    /* apply score for last match */
    if (best_letter)
      score += best_letter_score;

    /* did not match full pattern */
    if (*pattern_iter != '\0')
      return false;

    if (out_score)
      *out_score = score;
    return true;
  }
  NK_API int
  strmatch_fuzzy_string(char const* str, char const* pattern, int* out_score) {
    return strmatch_fuzzy_text(str, strlen(str), pattern, out_score);
  }
  NK_LIB int
  string_float_limit(char* string, int prec) {
    int dot = 0;
    char* c = string;
    while (*c) {
      if (*c == '.') {
        dot = 1;
        c++;
        continue;
      }
      if (dot == (prec + 1)) {
        *c = 0;
        break;
      }
      if (dot > 0)
        dot++;
      c++;
    }
    return (int) (c - string);
  }
  INTERN void
  strrev_ascii(char* s) {
    int len = strlen(s);
    int end = len / 2;
    int i = 0;
    char t;
    for (; i < end; ++i) {
      t = s[i];
      s[i] = s[len - 1 - i];
      s[len - 1 - i] = t;
    }
  }
  NK_LIB char*
  itoa(char* s, long n) {
    long i = 0;
    if (n == 0) {
      s[i++] = '0';
      s[i] = 0;
      return s;
    }
    if (n < 0) {
      s[i++] = '-';
      n = -n;
    }
    while (n > 0) {
      s[i++] = (char) ('0' + (n % 10));
      n /= 10;
    }
    s[i] = 0;
    if (s[0] == '-')
      ++s;

    strrev_ascii(s);
    return s;
  }
#ifndef NK_DTOA
#define NK_DTOA dtoa
  NK_LIB char*
  dtoa(char* s, double n) {
    int useExp = 0;
    int digit = 0, m = 0, m1 = 0;
    char* c = s;
    int neg = 0;

    NK_ASSERT(s);
    if (!s)
      return 0;

    if (n == 0.0) {
      s[0] = '0';
      s[1] = '\0';
      return s;
    }

    neg = (n < 0);
    if (neg)
      n = -n;

    /* calculate magnitude */
    m = log10(n);
    useExp = (m >= 14 || (neg && m >= 9) || m <= -9);
    if (neg)
      *(c++) = '-';

    /* set up for scientific notation */
    if (useExp) {
      if (m < 0)
        m -= 1;
      n = n / (double) pow(10.0, m);
      m1 = m;
      m = 0;
    }
    if (m < 1.0) {
      m = 0;
    }

    /* convert the number */
    while (n > NK_FLOAT_PRECISION || m >= 0) {
      double weight = pow(10.0, m);
      if (weight > 0) {
        double t = (double) n / weight;
        digit = ifloord(t);
        n -= ((double) digit * weight);
        *(c++) = (char) ('0' + (char) digit);
      }
      if (m == 0 && n > 0)
        *(c++) = '.';
      m--;
    }

    if (useExp) {
      /* convert the exponent */
      int i, j;
      *(c++) = 'e';
      if (m1 > 0) {
        *(c++) = '+';
      } else {
        *(c++) = '-';
        m1 = -m1;
      }
      m = 0;
      while (m1 > 0) {
        *(c++) = (char) ('0' + (char) (m1 % 10));
        m1 /= 10;
        m++;
      }
      c -= m;
      for (i = 0, j = m - 1; i < j; i++, j--) {
        /* swap without temporary */
        c[i] ^= c[j];
        c[j] ^= c[i];
        c[i] ^= c[j];
      }
      c += m;
    }
    *(c) = '\0';
    return s;
  }
#endif
#ifdef NK_INCLUDE_STANDARD_VARARGS
#ifndef NK_INCLUDE_STANDARD_IO
  INTERN int
  vsnprintf(char* buf, int buf_size, const char* fmt, va_list args) {
    enum arg_type {
      NK_ARG_TYPE_CHAR,
      NK_ARG_TYPE_SHORT,
      NK_ARG_TYPE_DEFAULT,
      NK_ARG_TYPE_LONG
    };
    enum arg_flags {
      NK_ARG_FLAG_LEFT = 0x01,
      NK_ARG_FLAG_PLUS = 0x02,
      NK_ARG_FLAG_SPACE = 0x04,
      NK_ARG_FLAG_NUM = 0x10,
      NK_ARG_FLAG_ZERO = 0x20
    };

    char number_buffer[NK_MAX_NUMBER_BUFFER];
    enum arg_type arg_type = NK_ARG_TYPE_DEFAULT;
    int precision = NK_DEFAULT;
    int width = NK_DEFAULT;
    flag flag = 0;

    int len = 0;
    int result = -1;
    const char* iter = fmt;

    NK_ASSERT(buf);
    NK_ASSERT(buf_size);
    if (!buf || !buf_size || !fmt)
      return 0;
    for (iter = fmt; *iter && len < buf_size; iter++) {
      /* copy all non-format characters */
      while (*iter && (*iter != '%') && (len < buf_size))
        buf[len++] = *iter++;
      if (!(*iter) || len >= buf_size)
        break;
      iter++;

      /* flag arguments */
      while (*iter) {
        if (*iter == '-')
          flag |= NK_ARG_FLAG_LEFT;
        else if (*iter == '+')
          flag |= NK_ARG_FLAG_PLUS;
        else if (*iter == ' ')
          flag |= NK_ARG_FLAG_SPACE;
        else if (*iter == '#')
          flag |= NK_ARG_FLAG_NUM;
        else if (*iter == '0')
          flag |= NK_ARG_FLAG_ZERO;
        else
          break;
        iter++;
      }

      /* width argument */
      width = NK_DEFAULT;
      if (*iter >= '1' && *iter <= '9') {
        char* end;
        width = strtoi(iter, &end);
        if (end == iter)
          width = -1;
        else
          iter = end;
      } else if (*iter == '*') {
        width = va_arg(args, int);
        iter++;
      }

      /* precision argument */
      precision = NK_DEFAULT;
      if (*iter == '.') {
        iter++;
        if (*iter == '*') {
          precision = va_arg(args, int);
          iter++;
        } else {
          char* end;
          precision = strtoi(iter, &end);
          if (end == iter)
            precision = -1;
          else
            iter = end;
        }
      }

      /* length modifier */
      if (*iter == 'h') {
        if (*(iter + 1) == 'h') {
          arg_type = NK_ARG_TYPE_CHAR;
          iter++;
        } else
          arg_type = NK_ARG_TYPE_SHORT;
        iter++;
      } else if (*iter == 'l') {
        arg_type = NK_ARG_TYPE_LONG;
        iter++;
      } else
        arg_type = NK_ARG_TYPE_DEFAULT;

      /* specifier */
      if (*iter == '%') {
        NK_ASSERT(arg_type == NK_ARG_TYPE_DEFAULT);
        NK_ASSERT(precision == NK_DEFAULT);
        NK_ASSERT(width == NK_DEFAULT);
        if (len < buf_size)
          buf[len++] = '%';
      } else if (*iter == 's') {
        /* string  */
        const char* str = va_arg(args, const char*);
        NK_ASSERT(str != buf && "buffer and argument are not allowed to overlap!");
        NK_ASSERT(arg_type == NK_ARG_TYPE_DEFAULT);
        NK_ASSERT(precision == NK_DEFAULT);
        NK_ASSERT(width == NK_DEFAULT);
        if (str == buf)
          return -1;
        while (str && *str && len < buf_size)
          buf[len++] = *str++;
      } else if (*iter == 'n') {
        /* current length callback */
        signed int* n = va_arg(args, int*);
        NK_ASSERT(arg_type == NK_ARG_TYPE_DEFAULT);
        NK_ASSERT(precision == NK_DEFAULT);
        NK_ASSERT(width == NK_DEFAULT);
        if (n)
          *n = len;
      } else if (*iter == 'c' || *iter == 'i' || *iter == 'd') {
        /* signed integer */
        long value = 0;
        const char* num_iter;
        int num_len, num_print, padding;
        int cur_precision = NK_MAX(precision, 1);
        int cur_width = NK_MAX(width, 0);

        /* retrieve correct value type */
        if (arg_type == NK_ARG_TYPE_CHAR)
          value = (signed char) va_arg(args, int);
        else if (arg_type == NK_ARG_TYPE_SHORT)
          value = (signed short) va_arg(args, int);
        else if (arg_type == NK_ARG_TYPE_LONG)
          value = va_arg(args, signed long);
        else if (*iter == 'c')
          value = (unsigned char) va_arg(args, int);
        else
          value = va_arg(args, signed int);

        /* convert number to string */
        itoa(number_buffer, value);
        num_len = strlen(number_buffer);
        padding = NK_MAX(cur_width - NK_MAX(cur_precision, num_len), 0);
        if ((flag & NK_ARG_FLAG_PLUS) || (flag & NK_ARG_FLAG_SPACE))
          padding = NK_MAX(padding - 1, 0);

        /* fill left padding up to a total of `width` characters */
        if (!(flag & NK_ARG_FLAG_LEFT)) {
          while (padding-- > 0 && (len < buf_size)) {
            if ((flag & NK_ARG_FLAG_ZERO) && (precision == NK_DEFAULT))
              buf[len++] = '0';
            else
              buf[len++] = ' ';
          }
        }

        /* copy string value representation into buffer */
        if ((flag & NK_ARG_FLAG_PLUS) && value >= 0 && len < buf_size)
          buf[len++] = '+';
        else if ((flag & NK_ARG_FLAG_SPACE) && value >= 0 && len < buf_size)
          buf[len++] = ' ';

        /* fill up to precision number of digits with '0' */
        num_print = NK_MAX(cur_precision, num_len);
        while (precision && (num_print > num_len) && (len < buf_size)) {
          buf[len++] = '0';
          num_print--;
        }

        /* copy string value representation into buffer */
        num_iter = number_buffer;
        while (precision && *num_iter && len < buf_size)
          buf[len++] = *num_iter++;

        /* fill right padding up to width characters */
        if (flag & NK_ARG_FLAG_LEFT) {
          while ((padding-- > 0) && (len < buf_size))
            buf[len++] = ' ';
        }
      } else if (*iter == 'o' || *iter == 'x' || *iter == 'X' || *iter == 'u') {
        /* unsigned integer */
        unsigned long value = 0;
        int num_len = 0, num_print, padding = 0;
        int cur_precision = NK_MAX(precision, 1);
        int cur_width = NK_MAX(width, 0);
        unsigned int base = (*iter == 'o') ? 8 : (*iter == 'u') ? 10
                                                                : 16;

        /* print oct/hex/dec value */
        const char* upper_output_format = "0123456789ABCDEF";
        const char* lower_output_format = "0123456789abcdef";
        const char* output_format = (*iter == 'x') ? lower_output_format : upper_output_format;

        /* retrieve correct value type */
        if (arg_type == NK_ARG_TYPE_CHAR)
          value = (unsigned char) va_arg(args, int);
        else if (arg_type == NK_ARG_TYPE_SHORT)
          value = (unsigned short) va_arg(args, int);
        else if (arg_type == NK_ARG_TYPE_LONG)
          value = va_arg(args, unsigned long);
        else
          value = va_arg(args, unsigned int);

        do {
          /* convert decimal number into hex/oct number */
          int digit = output_format[value % base];
          if (num_len < NK_MAX_NUMBER_BUFFER)
            number_buffer[num_len++] = (char) digit;
          value /= base;
        } while (value > 0);

        num_print = NK_MAX(cur_precision, num_len);
        padding = NK_MAX(cur_width - NK_MAX(cur_precision, num_len), 0);
        if (flag & NK_ARG_FLAG_NUM)
          padding = NK_MAX(padding - 1, 0);

        /* fill left padding up to a total of `width` characters */
        if (!(flag & NK_ARG_FLAG_LEFT)) {
          while ((padding-- > 0) && (len < buf_size)) {
            if ((flag & NK_ARG_FLAG_ZERO) && (precision == NK_DEFAULT))
              buf[len++] = '0';
            else
              buf[len++] = ' ';
          }
        }

        /* fill up to precision number of digits */
        if (num_print && (flag & NK_ARG_FLAG_NUM)) {
          if ((*iter == 'o') && (len < buf_size)) {
            buf[len++] = '0';
          } else if ((*iter == 'x') && ((len + 1) < buf_size)) {
            buf[len++] = '0';
            buf[len++] = 'x';
          } else if ((*iter == 'X') && ((len + 1) < buf_size)) {
            buf[len++] = '0';
            buf[len++] = 'X';
          }
        }
        while (precision && (num_print > num_len) && (len < buf_size)) {
          buf[len++] = '0';
          num_print--;
        }

        /* reverse number direction */
        while (num_len > 0) {
          if (precision && (len < buf_size))
            buf[len++] = number_buffer[num_len - 1];
          num_len--;
        }

        /* fill right padding up to width characters */
        if (flag & NK_ARG_FLAG_LEFT) {
          while ((padding-- > 0) && (len < buf_size))
            buf[len++] = ' ';
        }
      } else if (*iter == 'f') {
        /* floating point */
        const char* num_iter;
        int cur_precision = (precision < 0) ? 6 : precision;
        int prefix, cur_width = NK_MAX(width, 0);
        double value = va_arg(args, double);
        int num_len = 0, frac_len = 0, dot = 0;
        int padding = 0;

        NK_ASSERT(arg_type == NK_ARG_TYPE_DEFAULT);
        NK_DTOA(number_buffer, value);
        num_len = strlen(number_buffer);

        /* calculate padding */
        num_iter = number_buffer;
        while (*num_iter && *num_iter != '.')
          num_iter++;

        prefix = (*num_iter == '.') ? (int) (num_iter - number_buffer) + 1 : 0;
        padding = NK_MAX(cur_width - (prefix + NK_MIN(cur_precision, num_len - prefix)), 0);
        if ((flag & NK_ARG_FLAG_PLUS) || (flag & NK_ARG_FLAG_SPACE))
          padding = NK_MAX(padding - 1, 0);

        /* fill left padding up to a total of `width` characters */
        if (!(flag & NK_ARG_FLAG_LEFT)) {
          while (padding-- > 0 && (len < buf_size)) {
            if (flag & NK_ARG_FLAG_ZERO)
              buf[len++] = '0';
            else
              buf[len++] = ' ';
          }
        }

        /* copy string value representation into buffer */
        num_iter = number_buffer;
        if ((flag & NK_ARG_FLAG_PLUS) && (value >= 0) && (len < buf_size))
          buf[len++] = '+';
        else if ((flag & NK_ARG_FLAG_SPACE) && (value >= 0) && (len < buf_size))
          buf[len++] = ' ';
        while (*num_iter) {
          if (dot)
            frac_len++;
          if (len < buf_size)
            buf[len++] = *num_iter;
          if (*num_iter == '.')
            dot = 1;
          if (frac_len >= cur_precision)
            break;
          num_iter++;
        }

        /* fill number up to precision */
        while (frac_len < cur_precision) {
          if (!dot && len < buf_size) {
            buf[len++] = '.';
            dot = 1;
          }
          if (len < buf_size)
            buf[len++] = '0';
          frac_len++;
        }

        /* fill right padding up to width characters */
        if (flag & NK_ARG_FLAG_LEFT) {
          while ((padding-- > 0) && (len < buf_size))
            buf[len++] = ' ';
        }
      } else {
        /* Specifier not supported: g,G,e,E,p,z */
        NK_ASSERT(0 && "specifier is not supported!");
        return result;
      }
    }
    buf[(len >= buf_size) ? (buf_size - 1) : len] = 0;
    result = (len >= buf_size) ? -1 : len;
    return result;
  }
#endif
  NK_LIB int
  strfmt(char* buf, int buf_size, const char* fmt, va_list args) {
    int result = -1;
    NK_ASSERT(buf);
    NK_ASSERT(buf_size);
    if (!buf || !buf_size || !fmt)
      return 0;
#ifdef NK_INCLUDE_STANDARD_IO
    result = NK_VSNPRINTF(buf, (std::size_t) buf_size, fmt, args);
    result = (result >= buf_size) ? -1 : result;
    buf[buf_size - 1] = 0;
#else
    result = vsnprintf(buf, buf_size, fmt, args);
#endif
    return result;
  }
#endif
  NK_API hash
  murmur_hash(const void* key, int len, hash seed) {
    /* 32-Bit MurmurHash3: https://code.google.com/p/smhasher/wiki/MurmurHash3*/
#define NK_ROTL(x, r) ((x) << (r) | ((x) >> (32 - r)))

    std::uint32_t h1 = seed;
    std::uint32_t k1;
    const auto* data = (const std::uint32_t*) key;
    const std::uint32_t* keyptr = data;
    std::uint32_t* k1ptr;
    const int nblocks = len / 4;

    const std::uint32_t c1 = 0xcc9e2d51;
    const std::uint32_t c2 = 0x1b873593;
    int i;

    /* body */
    if (key == nullptr)
      return 0;
    for (i = 0; i < nblocks; ++i, ++keyptr) {
      k1ptr = &k1;
      k1ptr[0] = keyptr[0];
      k1ptr[1] = keyptr[1];
      k1ptr[2] = keyptr[2];
      k1ptr[3] = keyptr[3];

      k1 *= c1;
      k1 = NK_ROTL(k1, 15);
      k1 *= c2;

      h1 ^= k1;
      h1 = NK_ROTL(h1, 13);
      h1 = h1 * 5 + 0xe6546b64;
    }

    /* tail */
    const auto* tail = data + (len / 4);
    k1 = 0;
    switch (len & 3) {
      case 3:
        k1 ^= tail[2] << 16u; /* fallthrough */
      case 2:
        k1 ^= tail[1] << 8u; /* fallthrough */
      case 1:
        k1 ^= tail[0];
        k1 *= c1;
        k1 = NK_ROTL(k1, 15);
        k1 *= c2;
        h1 ^= k1;
        break;
      default:
        break;
    }

    /* finalization */
    h1 ^= (std::uint32_t) len;
    /* fmix32 */
    h1 ^= h1 >> 16;
    h1 *= 0x85ebca6b;
    h1 ^= h1 >> 13;
    h1 *= 0xc2b2ae35;
    h1 ^= h1 >> 16;

#undef NK_ROTL
    return h1;
  }
#ifdef NK_INCLUDE_STANDARD_IO
  NK_LIB char*
  file_load(const char* path, std::size_t* siz, const struct allocator* alloc) {
    char* buf;
    FILE* fd;
    long ret;

    NK_ASSERT(path);
    NK_ASSERT(siz);
    NK_ASSERT(alloc);
    if (!path || !siz || !alloc)
      return 0;

    fd = fopen(path, "rb");
    if (!fd)
      return 0;
    fseek(fd, 0, SEEK_END);
    ret = ftell(fd);
    if (ret < 0) {
      fclose(fd);
      return 0;
    }
    *siz = (std::size_t) ret;
    fseek(fd, 0, SEEK_SET);
    buf = (char*) alloc->alloc(alloc->userdata, 0, *siz);
    NK_ASSERT(buf);
    if (!buf) {
      fclose(fd);
      return 0;
    }
    *siz = (std::size_t) fread(buf, 1, *siz, fd);
    fclose(fd);
    return buf;
  }
#endif
  NK_LIB int
  text_clamp(const struct user_font* font, const char* text,
             int text_len, float space, int* glyphs, float* text_width,
             rune* sep_list, int sep_count) {
    int i = 0;
    int glyph_len = 0;
    float last_width = 0;
    rune unicode = 0;
    float width = 0;
    int len = 0;
    int g = 0;
    float s;

    int sep_len = 0;
    int sep_g = 0;
    float sep_width = 0;
    sep_count = NK_MAX(sep_count, 0);

    glyph_len = utf_decode(text, &unicode, text_len);
    while (glyph_len && (width < space) && (len < text_len)) {
      len += glyph_len;
      s = font->width(font->userdata, font->height, text, len);
      for (i = 0; i < sep_count; ++i) {
        if (unicode != sep_list[i])
          continue;
        sep_width = last_width = width;
        sep_g = g + 1;
        sep_len = len;
        break;
      }
      if (i == sep_count) {
        last_width = sep_width = width;
        sep_g = g + 1;
      }
      width = s;
      glyph_len = utf_decode(&text[len], &unicode, text_len - len);
      g++;
    }
    if (len >= text_len) {
      *glyphs = g;
      *text_width = last_width;
      return len;
    } else {
      *glyphs = sep_g;
      *text_width = sep_width;
      return (!sep_len) ? len : sep_len;
    }
  }
  NK_LIB vec2f
  text_calculate_text_bounds(const struct user_font* font,
                             const char* begin, int byte_len, float row_height, const char** remaining,
                             vec2f* out_offset, int* glyphs, int op) {
    float line_height = row_height;
    vec2f text_size = vec2_from_floats(0.0f, 0.0f);
    float line_width = 0.0f;

    float glyph_width;
    int glyph_len = 0;
    rune unicode = 0;
    int text_len = 0;
    if (!begin || byte_len <= 0 || !font)
      return vec2_from_floats(0.0f, row_height);

    glyph_len = utf_decode(begin, &unicode, byte_len);
    if (!glyph_len)
      return text_size;
    glyph_width = font->width(font->userdata, font->height, begin, glyph_len);

    *glyphs = 0;
    while ((text_len < byte_len) && glyph_len) {
      if (unicode == '\n') {
        text_size.x = NK_MAX(text_size.x, line_width);
        text_size.y += line_height;
        line_width = 0;
        *glyphs += 1;
        if (op == NK_STOP_ON_NEW_LINE)
          break;

        text_len++;
        glyph_len = utf_decode(begin + text_len, &unicode, byte_len - text_len);
        continue;
      }

      if (unicode == '\r') {
        text_len++;
        *glyphs += 1;
        glyph_len = utf_decode(begin + text_len, &unicode, byte_len - text_len);
        continue;
      }

      *glyphs = *glyphs + 1;
      text_len += glyph_len;
      line_width += (float) glyph_width;
      glyph_len = utf_decode(begin + text_len, &unicode, byte_len - text_len);
      glyph_width = font->width(font->userdata, font->height, begin + text_len, glyph_len);
      continue;
    }

    if (text_size.x < line_width)
      text_size.x = line_width;
    if (out_offset)
      *out_offset = vec2_from_floats(line_width, text_size.y + line_height);
    if (line_width > 0 || text_size.y == 0.0f)
      text_size.y += line_height;
    if (remaining)
      *remaining = begin + text_len;
    return text_size;
  }
} // namespace nk
