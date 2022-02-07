/*
 * The following file consists of two separate parts.
 *
 * picojson (a third party library)
 *   The original source code has not been altered.
 *   Copyright: 2009-2010 Cybozu Labs, Inc., 2011-2014 Kazuho Oku
 *   License: BSD-2
 *   Repository: https://github.com/kazuho/picojson
 * ecfeed
 *   Copyright: 2020 EcFeed AS.
 *   License: EPL-1.0
 *   Repository: https://github.com/ecfeed/ecfeed.cpp
 */

// -------------------------------------------------------------------------------------------------------------

/*
 * Copyright 2009-2010 Cybozu Labs, Inc.
 * Copyright 2011-2014 Kazuho Oku
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef picojson_h
#define picojson_h

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstddef>
#include <iostream>
#include <iterator>
#include <limits>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>
#include <utility>

// for isnan/isinf
#if __cplusplus >= 201103L
#include <cmath>
#else
extern "C" {
#ifdef _MSC_VER
#include <float.h>
#elif defined(__INTEL_COMPILER)
#include <mathimf.h>
#else
#include <math.h>
#endif
}
#endif

#ifndef PICOJSON_USE_RVALUE_REFERENCE
#if (defined(__cpp_rvalue_references) && __cpp_rvalue_references >= 200610) || (defined(_MSC_VER) && _MSC_VER >= 1600)
#define PICOJSON_USE_RVALUE_REFERENCE 1
#else
#define PICOJSON_USE_RVALUE_REFERENCE 0
#endif
#endif // PICOJSON_USE_RVALUE_REFERENCE

#ifndef PICOJSON_NOEXCEPT
#if PICOJSON_USE_RVALUE_REFERENCE
#define PICOJSON_NOEXCEPT noexcept
#else
#define PICOJSON_NOEXCEPT throw()
#endif
#endif

// experimental support for int64_t (see README.mkdn for detail)
#ifdef PICOJSON_USE_INT64
#define __STDC_FORMAT_MACROS
#include <cerrno>
#if __cplusplus >= 201103L
#include <cinttypes>
#else
extern "C" {
#include <inttypes.h>
}
#endif
#endif

// to disable the use of localeconv(3), set PICOJSON_USE_LOCALE to 0
#ifndef PICOJSON_USE_LOCALE
#define PICOJSON_USE_LOCALE 1
#endif
#if PICOJSON_USE_LOCALE
extern "C" {
#include <locale.h>
}
#endif

#ifndef PICOJSON_ASSERT
#define PICOJSON_ASSERT(e)                                                                                                         \
  do {                                                                                                                             \
    if (!(e))                                                                                                                      \
      throw std::runtime_error(#e);                                                                                                \
  } while (0)
#endif

#ifdef _MSC_VER
#define SNPRINTF _snprintf_s
#pragma warning(push)
#pragma warning(disable : 4244) // conversion from int to char
#pragma warning(disable : 4127) // conditional expression is constant
#pragma warning(disable : 4702) // unreachable code
#pragma warning(disable : 4706) // assignment within conditional expression
#else
#define SNPRINTF snprintf
#endif

namespace picojson {

enum {
  null_type,
  boolean_type,
  number_type,
  string_type,
  array_type,
  object_type
#ifdef PICOJSON_USE_INT64
  ,
  int64_type
#endif
};

enum { INDENT_WIDTH = 2 };

struct null {};

class value {
public:
  typedef std::vector<value> array;
  typedef std::map<std::string, value> object;
  union _storage {
    bool boolean_;
    double number_;
#ifdef PICOJSON_USE_INT64
    int64_t int64_;
#endif
    std::string *string_;
    array *array_;
    object *object_;
  };

protected:
  int type_;
  _storage u_;

public:
  value();
  value(int type, bool);
  explicit value(bool b);
#ifdef PICOJSON_USE_INT64
  explicit value(int64_t i);
#endif
  explicit value(double n);
  explicit value(const std::string &s);
  explicit value(const array &a);
  explicit value(const object &o);
#if PICOJSON_USE_RVALUE_REFERENCE
  explicit value(std::string &&s);
  explicit value(array &&a);
  explicit value(object &&o);
#endif
  explicit value(const char *s);
  value(const char *s, size_t len);
  ~value();
  value(const value &x);
  value &operator=(const value &x);
#if PICOJSON_USE_RVALUE_REFERENCE
  value(value &&x) PICOJSON_NOEXCEPT;
  value &operator=(value &&x) PICOJSON_NOEXCEPT;
#endif
  void swap(value &x) PICOJSON_NOEXCEPT;
  template <typename T> bool is() const;
  template <typename T> const T &get() const;
  template <typename T> T &get();
  template <typename T> void set(const T &);
#if PICOJSON_USE_RVALUE_REFERENCE
  template <typename T> void set(T &&);
#endif
  bool evaluate_as_boolean() const;
  const value &get(const size_t idx) const;
  const value &get(const std::string &key) const;
  value &get(const size_t idx);
  value &get(const std::string &key);

  bool contains(const size_t idx) const;
  bool contains(const std::string &key) const;
  std::string to_str() const;
  template <typename Iter> void serialize(Iter os, bool prettify = false) const;
  std::string serialize(bool prettify = false) const;

private:
  template <typename T> value(const T *); // intentionally defined to block implicit conversion of pointer to bool
  template <typename Iter> static void _indent(Iter os, int indent);
  template <typename Iter> void _serialize(Iter os, int indent) const;
  std::string _serialize(int indent) const;
  void clear();
};

typedef value::array array;
typedef value::object object;

inline value::value() : type_(null_type), u_() {
}

inline value::value(int type, bool) : type_(type), u_() {
  switch (type) {
#define INIT(p, v)                                                                                                                 \
  case p##type:                                                                                                                    \
    u_.p = v;                                                                                                                      \
    break
    INIT(boolean_, false);
    INIT(number_, 0.0);
#ifdef PICOJSON_USE_INT64
    INIT(int64_, 0);
#endif
    INIT(string_, new std::string());
    INIT(array_, new array());
    INIT(object_, new object());
#undef INIT
  default:
    break;
  }
}

inline value::value(bool b) : type_(boolean_type), u_() {
  u_.boolean_ = b;
}

#ifdef PICOJSON_USE_INT64
inline value::value(int64_t i) : type_(int64_type), u_() {
  u_.int64_ = i;
}
#endif

inline value::value(double n) : type_(number_type), u_() {
  if (
#ifdef _MSC_VER
      !_finite(n)
#elif __cplusplus >= 201103L
      std::isnan(n) || std::isinf(n)
#else
      isnan(n) || isinf(n)
#endif
          ) {
    throw std::overflow_error("");
  }
  u_.number_ = n;
}

inline value::value(const std::string &s) : type_(string_type), u_() {
  u_.string_ = new std::string(s);
}

inline value::value(const array &a) : type_(array_type), u_() {
  u_.array_ = new array(a);
}

inline value::value(const object &o) : type_(object_type), u_() {
  u_.object_ = new object(o);
}

#if PICOJSON_USE_RVALUE_REFERENCE
inline value::value(std::string &&s) : type_(string_type), u_() {
  u_.string_ = new std::string(std::move(s));
}

inline value::value(array &&a) : type_(array_type), u_() {
  u_.array_ = new array(std::move(a));
}

inline value::value(object &&o) : type_(object_type), u_() {
  u_.object_ = new object(std::move(o));
}
#endif

inline value::value(const char *s) : type_(string_type), u_() {
  u_.string_ = new std::string(s);
}

inline value::value(const char *s, size_t len) : type_(string_type), u_() {
  u_.string_ = new std::string(s, len);
}

inline void value::clear() {
  switch (type_) {
#define DEINIT(p)                                                                                                                  \
  case p##type:                                                                                                                    \
    delete u_.p;                                                                                                                   \
    break
    DEINIT(string_);
    DEINIT(array_);
    DEINIT(object_);
#undef DEINIT
  default:
    break;
  }
}

inline value::~value() {
  clear();
}

inline value::value(const value &x) : type_(x.type_), u_() {
  switch (type_) {
#define INIT(p, v)                                                                                                                 \
  case p##type:                                                                                                                    \
    u_.p = v;                                                                                                                      \
    break
    INIT(string_, new std::string(*x.u_.string_));
    INIT(array_, new array(*x.u_.array_));
    INIT(object_, new object(*x.u_.object_));
#undef INIT
  default:
    u_ = x.u_;
    break;
  }
}

inline value &value::operator=(const value &x) {
  if (this != &x) {
    value t(x);
    swap(t);
  }
  return *this;
}

#if PICOJSON_USE_RVALUE_REFERENCE
inline value::value(value &&x) PICOJSON_NOEXCEPT : type_(null_type), u_() {
  swap(x);
}
inline value &value::operator=(value &&x) PICOJSON_NOEXCEPT {
  swap(x);
  return *this;
}
#endif
inline void value::swap(value &x) PICOJSON_NOEXCEPT {
  std::swap(type_, x.type_);
  std::swap(u_, x.u_);
}

#define IS(ctype, jtype)                                                                                                           \
  template <> inline bool value::is<ctype>() const {                                                                               \
    return type_ == jtype##_type;                                                                                                  \
  }
IS(null, null)
IS(bool, boolean)
#ifdef PICOJSON_USE_INT64
IS(int64_t, int64)
#endif
IS(std::string, string)
IS(array, array)
IS(object, object)
#undef IS
template <> inline bool value::is<double>() const {
  return type_ == number_type
#ifdef PICOJSON_USE_INT64
         || type_ == int64_type
#endif
      ;
}

#define GET(ctype, var)                                                                                                            \
  template <> inline const ctype &value::get<ctype>() const {                                                                      \
    PICOJSON_ASSERT("type mismatch! call is<type>() before get<type>()" && is<ctype>());                                           \
    return var;                                                                                                                    \
  }                                                                                                                                \
  template <> inline ctype &value::get<ctype>() {                                                                                  \
    PICOJSON_ASSERT("type mismatch! call is<type>() before get<type>()" && is<ctype>());                                           \
    return var;                                                                                                                    \
  }
GET(bool, u_.boolean_)
GET(std::string, *u_.string_)
GET(array, *u_.array_)
GET(object, *u_.object_)
#ifdef PICOJSON_USE_INT64
GET(double,
    (type_ == int64_type && (const_cast<value *>(this)->type_ = number_type, const_cast<value *>(this)->u_.number_ = u_.int64_),
     u_.number_))
GET(int64_t, u_.int64_)
#else
GET(double, u_.number_)
#endif
#undef GET

#define SET(ctype, jtype, setter)                                                                                                  \
  template <> inline void value::set<ctype>(const ctype &_val) {                                                                   \
    clear();                                                                                                                       \
    type_ = jtype##_type;                                                                                                          \
    setter                                                                                                                         \
  }
SET(bool, boolean, u_.boolean_ = _val;)
SET(std::string, string, u_.string_ = new std::string(_val);)
SET(array, array, u_.array_ = new array(_val);)
SET(object, object, u_.object_ = new object(_val);)
SET(double, number, u_.number_ = _val;)
#ifdef PICOJSON_USE_INT64
SET(int64_t, int64, u_.int64_ = _val;)
#endif
#undef SET

#if PICOJSON_USE_RVALUE_REFERENCE
#define MOVESET(ctype, jtype, setter)                                                                                              \
  template <> inline void value::set<ctype>(ctype && _val) {                                                                       \
    clear();                                                                                                                       \
    type_ = jtype##_type;                                                                                                          \
    setter                                                                                                                         \
  }
MOVESET(std::string, string, u_.string_ = new std::string(std::move(_val));)
MOVESET(array, array, u_.array_ = new array(std::move(_val));)
MOVESET(object, object, u_.object_ = new object(std::move(_val));)
#undef MOVESET
#endif

inline bool value::evaluate_as_boolean() const {
  switch (type_) {
  case null_type:
    return false;
  case boolean_type:
    return u_.boolean_;
  case number_type:
    return u_.number_ != 0;
#ifdef PICOJSON_USE_INT64
  case int64_type:
    return u_.int64_ != 0;
#endif
  case string_type:
    return !u_.string_->empty();
  default:
    return true;
  }
}

inline const value &value::get(const size_t idx) const {
  static value s_null;
  PICOJSON_ASSERT(is<array>());
  return idx < u_.array_->size() ? (*u_.array_)[idx] : s_null;
}

inline value &value::get(const size_t idx) {
  static value s_null;
  PICOJSON_ASSERT(is<array>());
  return idx < u_.array_->size() ? (*u_.array_)[idx] : s_null;
}

inline const value &value::get(const std::string &key) const {
  static value s_null;
  PICOJSON_ASSERT(is<object>());
  object::const_iterator i = u_.object_->find(key);
  return i != u_.object_->end() ? i->second : s_null;
}

inline value &value::get(const std::string &key) {
  static value s_null;
  PICOJSON_ASSERT(is<object>());
  object::iterator i = u_.object_->find(key);
  return i != u_.object_->end() ? i->second : s_null;
}

inline bool value::contains(const size_t idx) const {
  PICOJSON_ASSERT(is<array>());
  return idx < u_.array_->size();
}

inline bool value::contains(const std::string &key) const {
  PICOJSON_ASSERT(is<object>());
  object::const_iterator i = u_.object_->find(key);
  return i != u_.object_->end();
}

inline std::string value::to_str() const {
  switch (type_) {
  case null_type:
    return "null";
  case boolean_type:
    return u_.boolean_ ? "true" : "false";
#ifdef PICOJSON_USE_INT64
  case int64_type: {
    char buf[sizeof("-9223372036854775808")];
    SNPRINTF(buf, sizeof(buf), "%" PRId64, u_.int64_);
    return buf;
  }
#endif
  case number_type: {
    char buf[256];
    double tmp;
    SNPRINTF(buf, sizeof(buf), fabs(u_.number_) < (1ULL << 53) && modf(u_.number_, &tmp) == 0 ? "%.f" : "%.17g", u_.number_);
#if PICOJSON_USE_LOCALE
    char *decimal_point = localeconv()->decimal_point;
    if (strcmp(decimal_point, ".") != 0) {
      size_t decimal_point_len = strlen(decimal_point);
      for (char *p = buf; *p != '\0'; ++p) {
        if (strncmp(p, decimal_point, decimal_point_len) == 0) {
          return std::string(buf, p) + "." + (p + decimal_point_len);
        }
      }
    }
#endif
    return buf;
  }
  case string_type:
    return *u_.string_;
  case array_type:
    return "array";
  case object_type:
    return "object";
  default:
    PICOJSON_ASSERT(0);
#ifdef _MSC_VER
    __assume(0);
#endif
  }
  return std::string();
}

template <typename Iter> void copy(const std::string &s, Iter oi) {
  std::copy(s.begin(), s.end(), oi);
}

template <typename Iter> struct serialize_str_char {
  Iter oi;
  void operator()(char c) {
    switch (c) {
#define MAP(val, sym)                                                                                                              \
  case val:                                                                                                                        \
    copy(sym, oi);                                                                                                                 \
    break
      MAP('"', "\\\"");
      MAP('\\', "\\\\");
      MAP('/', "\\/");
      MAP('\b', "\\b");
      MAP('\f', "\\f");
      MAP('\n', "\\n");
      MAP('\r', "\\r");
      MAP('\t', "\\t");
#undef MAP
    default:
      if (static_cast<unsigned char>(c) < 0x20 || c == 0x7f) {
        char buf[7];
        SNPRINTF(buf, sizeof(buf), "\\u%04x", c & 0xff);
        copy(buf, buf + 6, oi);
      } else {
        *oi++ = c;
      }
      break;
    }
  }
};

template <typename Iter> void serialize_str(const std::string &s, Iter oi) {
  *oi++ = '"';
  serialize_str_char<Iter> process_char = {oi};
  std::for_each(s.begin(), s.end(), process_char);
  *oi++ = '"';
}

template <typename Iter> void value::serialize(Iter oi, bool prettify) const {
  return _serialize(oi, prettify ? 0 : -1);
}

inline std::string value::serialize(bool prettify) const {
  return _serialize(prettify ? 0 : -1);
}

template <typename Iter> void value::_indent(Iter oi, int indent) {
  *oi++ = '\n';
  for (int i = 0; i < indent * INDENT_WIDTH; ++i) {
    *oi++ = ' ';
  }
}

template <typename Iter> void value::_serialize(Iter oi, int indent) const {
  switch (type_) {
  case string_type:
    serialize_str(*u_.string_, oi);
    break;
  case array_type: {
    *oi++ = '[';
    if (indent != -1) {
      ++indent;
    }
    for (array::const_iterator i = u_.array_->begin(); i != u_.array_->end(); ++i) {
      if (i != u_.array_->begin()) {
        *oi++ = ',';
      }
      if (indent != -1) {
        _indent(oi, indent);
      }
      i->_serialize(oi, indent);
    }
    if (indent != -1) {
      --indent;
      if (!u_.array_->empty()) {
        _indent(oi, indent);
      }
    }
    *oi++ = ']';
    break;
  }
  case object_type: {
    *oi++ = '{';
    if (indent != -1) {
      ++indent;
    }
    for (object::const_iterator i = u_.object_->begin(); i != u_.object_->end(); ++i) {
      if (i != u_.object_->begin()) {
        *oi++ = ',';
      }
      if (indent != -1) {
        _indent(oi, indent);
      }
      serialize_str(i->first, oi);
      *oi++ = ':';
      if (indent != -1) {
        *oi++ = ' ';
      }
      i->second._serialize(oi, indent);
    }
    if (indent != -1) {
      --indent;
      if (!u_.object_->empty()) {
        _indent(oi, indent);
      }
    }
    *oi++ = '}';
    break;
  }
  default:
    copy(to_str(), oi);
    break;
  }
  if (indent == 0) {
    *oi++ = '\n';
  }
}

inline std::string value::_serialize(int indent) const {
  std::string s;
  _serialize(std::back_inserter(s), indent);
  return s;
}

template <typename Iter> class input {
protected:
  Iter cur_, end_;
  bool consumed_;
  int line_;

public:
  input(const Iter &first, const Iter &last) : cur_(first), end_(last), consumed_(false), line_(1) {
  }
  int getc() {
    if (consumed_) {
      if (*cur_ == '\n') {
        ++line_;
      }
      ++cur_;
    }
    if (cur_ == end_) {
      consumed_ = false;
      return -1;
    }
    consumed_ = true;
    return *cur_ & 0xff;
  }
  void ungetc() {
    consumed_ = false;
  }
  Iter cur() const {
    if (consumed_) {
      input<Iter> *self = const_cast<input<Iter> *>(this);
      self->consumed_ = false;
      ++self->cur_;
    }
    return cur_;
  }
  int line() const {
    return line_;
  }
  void skip_ws() {
    while (1) {
      int ch = getc();
      if (!(ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r')) {
        ungetc();
        break;
      }
    }
  }
  bool expect(const int expected) {
    skip_ws();
    if (getc() != expected) {
      ungetc();
      return false;
    }
    return true;
  }
  bool match(const std::string &pattern) {
    for (std::string::const_iterator pi(pattern.begin()); pi != pattern.end(); ++pi) {
      if (getc() != *pi) {
        ungetc();
        return false;
      }
    }
    return true;
  }
};

template <typename Iter> inline int _parse_quadhex(input<Iter> &in) {
  int uni_ch = 0, hex;
  for (int i = 0; i < 4; i++) {
    if ((hex = in.getc()) == -1) {
      return -1;
    }
    if ('0' <= hex && hex <= '9') {
      hex -= '0';
    } else if ('A' <= hex && hex <= 'F') {
      hex -= 'A' - 0xa;
    } else if ('a' <= hex && hex <= 'f') {
      hex -= 'a' - 0xa;
    } else {
      in.ungetc();
      return -1;
    }
    uni_ch = uni_ch * 16 + hex;
  }
  return uni_ch;
}

template <typename String, typename Iter> inline bool _parse_codepoint(String &out, input<Iter> &in) {
  int uni_ch;
  if ((uni_ch = _parse_quadhex(in)) == -1) {
    return false;
  }
  if (0xd800 <= uni_ch && uni_ch <= 0xdfff) {
    if (0xdc00 <= uni_ch) {
      // a second 16-bit of a surrogate pair appeared
      return false;
    }
    // first 16-bit of surrogate pair, get the next one
    if (in.getc() != '\\' || in.getc() != 'u') {
      in.ungetc();
      return false;
    }
    int second = _parse_quadhex(in);
    if (!(0xdc00 <= second && second <= 0xdfff)) {
      return false;
    }
    uni_ch = ((uni_ch - 0xd800) << 10) | ((second - 0xdc00) & 0x3ff);
    uni_ch += 0x10000;
  }
  if (uni_ch < 0x80) {
    out.push_back(static_cast<char>(uni_ch));
  } else {
    if (uni_ch < 0x800) {
      out.push_back(static_cast<char>(0xc0 | (uni_ch >> 6)));
    } else {
      if (uni_ch < 0x10000) {
        out.push_back(static_cast<char>(0xe0 | (uni_ch >> 12)));
      } else {
        out.push_back(static_cast<char>(0xf0 | (uni_ch >> 18)));
        out.push_back(static_cast<char>(0x80 | ((uni_ch >> 12) & 0x3f)));
      }
      out.push_back(static_cast<char>(0x80 | ((uni_ch >> 6) & 0x3f)));
    }
    out.push_back(static_cast<char>(0x80 | (uni_ch & 0x3f)));
  }
  return true;
}

template <typename String, typename Iter> inline bool _parse_string(String &out, input<Iter> &in) {
  while (1) {
    int ch = in.getc();
    if (ch < ' ') {
      in.ungetc();
      return false;
    } else if (ch == '"') {
      return true;
    } else if (ch == '\\') {
      if ((ch = in.getc()) == -1) {
        return false;
      }
      switch (ch) {
#define MAP(sym, val)                                                                                                              \
  case sym:                                                                                                                        \
    out.push_back(val);                                                                                                            \
    break
        MAP('"', '\"');
        MAP('\\', '\\');
        MAP('/', '/');
        MAP('b', '\b');
        MAP('f', '\f');
        MAP('n', '\n');
        MAP('r', '\r');
        MAP('t', '\t');
#undef MAP
      case 'u':
        if (!_parse_codepoint(out, in)) {
          return false;
        }
        break;
      default:
        return false;
      }
    } else {
      out.push_back(static_cast<char>(ch));
    }
  }
  return false;
}

template <typename Context, typename Iter> inline bool _parse_array(Context &ctx, input<Iter> &in) {
  if (!ctx.parse_array_start()) {
    return false;
  }
  size_t idx = 0;
  if (in.expect(']')) {
    return ctx.parse_array_stop(idx);
  }
  do {
    if (!ctx.parse_array_item(in, idx)) {
      return false;
    }
    idx++;
  } while (in.expect(','));
  return in.expect(']') && ctx.parse_array_stop(idx);
}

template <typename Context, typename Iter> inline bool _parse_object(Context &ctx, input<Iter> &in) {
  if (!ctx.parse_object_start()) {
    return false;
  }
  if (in.expect('}')) {
    return true;
  }
  do {
    std::string key;
    if (!in.expect('"') || !_parse_string(key, in) || !in.expect(':')) {
      return false;
    }
    if (!ctx.parse_object_item(in, key)) {
      return false;
    }
  } while (in.expect(','));
  return in.expect('}');
}

template <typename Iter> inline std::string _parse_number(input<Iter> &in) {
  std::string num_str;
  while (1) {
    int ch = in.getc();
    if (('0' <= ch && ch <= '9') || ch == '+' || ch == '-' || ch == 'e' || ch == 'E') {
      num_str.push_back(static_cast<char>(ch));
    } else if (ch == '.') {
#if PICOJSON_USE_LOCALE
      num_str += localeconv()->decimal_point;
#else
      num_str.push_back('.');
#endif
    } else {
      in.ungetc();
      break;
    }
  }
  return num_str;
}

template <typename Context, typename Iter> inline bool _parse(Context &ctx, input<Iter> &in) {
  in.skip_ws();
  int ch = in.getc();
  switch (ch) {
#define IS(ch, text, op)                                                                                                           \
  case ch:                                                                                                                         \
    if (in.match(text) && op) {                                                                                                    \
      return true;                                                                                                                 \
    } else {                                                                                                                       \
      return false;                                                                                                                \
    }
    IS('n', "ull", ctx.set_null());
    IS('f', "alse", ctx.set_bool(false));
    IS('t', "rue", ctx.set_bool(true));
#undef IS
  case '"':
    return ctx.parse_string(in);
  case '[':
    return _parse_array(ctx, in);
  case '{':
    return _parse_object(ctx, in);
  default:
    if (('0' <= ch && ch <= '9') || ch == '-') {
      double f;
      char *endp;
      in.ungetc();
      std::string num_str(_parse_number(in));
      if (num_str.empty()) {
        return false;
      }
#ifdef PICOJSON_USE_INT64
      {
        errno = 0;
        intmax_t ival = strtoimax(num_str.c_str(), &endp, 10);
        if (errno == 0 && std::numeric_limits<int64_t>::min() <= ival && ival <= std::numeric_limits<int64_t>::max() &&
            endp == num_str.c_str() + num_str.size()) {
          ctx.set_int64(ival);
          return true;
        }
      }
#endif
      f = strtod(num_str.c_str(), &endp);
      if (endp == num_str.c_str() + num_str.size()) {
        ctx.set_number(f);
        return true;
      }
      return false;
    }
    break;
  }
  in.ungetc();
  return false;
}

class deny_parse_context {
public:
  bool set_null() {
    return false;
  }
  bool set_bool(bool) {
    return false;
  }
#ifdef PICOJSON_USE_INT64
  bool set_int64(int64_t) {
    return false;
  }
#endif
  bool set_number(double) {
    return false;
  }
  template <typename Iter> bool parse_string(input<Iter> &) {
    return false;
  }
  bool parse_array_start() {
    return false;
  }
  template <typename Iter> bool parse_array_item(input<Iter> &, size_t) {
    return false;
  }
  bool parse_array_stop(size_t) {
    return false;
  }
  bool parse_object_start() {
    return false;
  }
  template <typename Iter> bool parse_object_item(input<Iter> &, const std::string &) {
    return false;
  }
};

class default_parse_context {
protected:
  value *out_;

public:
  default_parse_context(value *out) : out_(out) {
  }
  bool set_null() {
    *out_ = value();
    return true;
  }
  bool set_bool(bool b) {
    *out_ = value(b);
    return true;
  }
#ifdef PICOJSON_USE_INT64
  bool set_int64(int64_t i) {
    *out_ = value(i);
    return true;
  }
#endif
  bool set_number(double f) {
    *out_ = value(f);
    return true;
  }
  template <typename Iter> bool parse_string(input<Iter> &in) {
    *out_ = value(string_type, false);
    return _parse_string(out_->get<std::string>(), in);
  }
  bool parse_array_start() {
    *out_ = value(array_type, false);
    return true;
  }
  template <typename Iter> bool parse_array_item(input<Iter> &in, size_t) {
    array &a = out_->get<array>();
    a.push_back(value());
    default_parse_context ctx(&a.back());
    return _parse(ctx, in);
  }
  bool parse_array_stop(size_t) {
    return true;
  }
  bool parse_object_start() {
    *out_ = value(object_type, false);
    return true;
  }
  template <typename Iter> bool parse_object_item(input<Iter> &in, const std::string &key) {
    object &o = out_->get<object>();
    default_parse_context ctx(&o[key]);
    return _parse(ctx, in);
  }

private:
  default_parse_context(const default_parse_context &);
  default_parse_context &operator=(const default_parse_context &);
};

class null_parse_context {
public:
  struct dummy_str {
    void push_back(int) {
    }
  };

public:
  null_parse_context() {
  }
  bool set_null() {
    return true;
  }
  bool set_bool(bool) {
    return true;
  }
#ifdef PICOJSON_USE_INT64
  bool set_int64(int64_t) {
    return true;
  }
#endif
  bool set_number(double) {
    return true;
  }
  template <typename Iter> bool parse_string(input<Iter> &in) {
    dummy_str s;
    return _parse_string(s, in);
  }
  bool parse_array_start() {
    return true;
  }
  template <typename Iter> bool parse_array_item(input<Iter> &in, size_t) {
    return _parse(*this, in);
  }
  bool parse_array_stop(size_t) {
    return true;
  }
  bool parse_object_start() {
    return true;
  }
  template <typename Iter> bool parse_object_item(input<Iter> &in, const std::string &) {
    return _parse(*this, in);
  }

private:
  null_parse_context(const null_parse_context &);
  null_parse_context &operator=(const null_parse_context &);
};

// obsolete, use the version below
template <typename Iter> inline std::string parse(value &out, Iter &pos, const Iter &last) {
  std::string err;
  pos = parse(out, pos, last, &err);
  return err;
}

template <typename Context, typename Iter> inline Iter _parse(Context &ctx, const Iter &first, const Iter &last, std::string *err) {
  input<Iter> in(first, last);
  if (!_parse(ctx, in) && err != NULL) {
    char buf[64];
    SNPRINTF(buf, sizeof(buf), "syntax error at line %d near: ", in.line());
    *err = buf;
    while (1) {
      int ch = in.getc();
      if (ch == -1 || ch == '\n') {
        break;
      } else if (ch >= ' ') {
        err->push_back(static_cast<char>(ch));
      }
    }
  }
  return in.cur();
}

template <typename Iter> inline Iter parse(value &out, const Iter &first, const Iter &last, std::string *err) {
  default_parse_context ctx(&out);
  return _parse(ctx, first, last, err);
}

inline std::string parse(value &out, const std::string &s) {
  std::string err;
  parse(out, s.begin(), s.end(), &err);
  return err;
}

inline std::string parse(value &out, std::istream &is) {
  std::string err;
  parse(out, std::istreambuf_iterator<char>(is.rdbuf()), std::istreambuf_iterator<char>(), &err);
  return err;
}

template <typename T> struct last_error_t { static std::string s; };
template <typename T> std::string last_error_t<T>::s;

inline void set_last_error(const std::string &s) {
  last_error_t<bool>::s = s;
}

inline const std::string &get_last_error() {
  return last_error_t<bool>::s;
}

inline bool operator==(const value &x, const value &y) {
  if (x.is<null>())
    return y.is<null>();
#define PICOJSON_CMP(type)                                                                                                         \
  if (x.is<type>())                                                                                                                \
  return y.is<type>() && x.get<type>() == y.get<type>()
  PICOJSON_CMP(bool);
  PICOJSON_CMP(double);
  PICOJSON_CMP(std::string);
  PICOJSON_CMP(array);
  PICOJSON_CMP(object);
#undef PICOJSON_CMP
  PICOJSON_ASSERT(0);
#ifdef _MSC_VER
  __assume(0);
#endif
  return false;
}

inline bool operator!=(const value &x, const value &y) {
  return !(x == y);
}
}

#if !PICOJSON_USE_RVALUE_REFERENCE
namespace std {
template <> inline void swap(picojson::value &x, picojson::value &y) {
  x.swap(y);
}
}
#endif

inline std::istream &operator>>(std::istream &is, picojson::value &x) {
  picojson::set_last_error(std::string());
  const std::string err(picojson::parse(x, is));
  if (!err.empty()) {
    picojson::set_last_error(err);
    is.setstate(std::ios::failbit);
  }
  return is;
}

inline std::ostream &operator<<(std::ostream &os, const picojson::value &x) {
  x.serialize(std::ostream_iterator<char>(os));
  return os;
}
#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif

// -------------------------------------------------------------------------------------------------------------

/********************************************************************************
 * Copyright (c) 15.10.2020 ecFeed AS
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 1.0 which is available at
 * https://www.eclipse.org/org/documents/epl-v10.html.
 *
 * SPDX-License-Identifier: EPL-1.0
 ********************************************************************************/

#ifndef ECFEED_HPP
#define ECFEED_HPP

#include <curl/curl.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <openssl/pkcs12.h>
#include <iostream>
#include <list>
#include <set>
#include <filesystem>
#include <typeindex>
#include <regex>
#include <any>
#include <future>
#include <unistd.h>
#include <unordered_map>
#include <optional>
#include <algorithm>

namespace ecfeed {

enum class template_type {
    csv = 1,
    xml = 2,
    gherkin = 3,
    json = 4,
    raw = 5
};

static std::string template_type_url_param(const template_type& t) {

    switch (t) {
    case template_type::csv:
        return "CSV";
    case template_type::xml:
        return "XML";
    case template_type::gherkin:
        return "Gherkin";
    case template_type::json:
        return "JSON";
    case template_type::raw:
        return "RAW";
    }

    return "UNKNOWN";
}

enum class data_source {
    static_data = 1,
    nwise = 2,
    cartesian = 3,
    random = 4,
};

static std::string data_source_url_param(const data_source& s) {

    switch(s) {
    case data_source::static_data:
        return "static";
    case data_source::nwise:
        return "genNWise";
    case data_source::cartesian:
        return "genCartesian";
    case data_source::random:
        return "genRandom";
    }

    return "UNKNOWN";
}

struct session_data_feedback {
  bool process = false;
  int test_cases_total = 0;
  int test_cases_parsed = 0;
  bool transmission_finished = false;
  std::map<std::string, std::any> test_results;
};

struct session_data_connection {
  std::string generator_address;
  std::string request_type;
  std::string cert_path;
  std::string pkey_path;
  std::string ca_path;
  CURL* curl_handle;
};

struct session_data_internal {
  std::string framework = "C++";
  std::string method_name_qualified;
  std::string test_session_id;
  std::string timestamp;
  std::vector<std::string> arg_names;
  std::vector<std::string> arg_types;
  bool method_info_ready = false;
};

struct session_data {
  std::string model;
  std::string method_name;
  ecfeed::data_source data_source;
  
  std::map<std::string, std::any> main;
  std::map<std::string, std::string> properties;
  
  ecfeed::session_data_feedback feedback;
  ecfeed::session_data_internal internal;
  ecfeed::session_data_connection connection;

  friend std::ostream& operator<<(std::ostream& os, const session_data& data);

  std::optional<picojson::value> process_gen_options() const;
  std::optional<picojson::value> process_test_results() const;
  std::optional<picojson::value> process_test_session_label() const;
  std::optional<picojson::value> process_constraints() const;
  std::optional<picojson::value> process_test_suites() const;
  std::optional<picojson::value> process_custom() const;
  std::optional<picojson::value> process_choices() const;
  std::optional<picojson::value> process_template() const;
  std::optional<picojson::value> process_property(const std::string parameter) const;

  void set_connection_details(const std::string& cert_path, const std::string& pkey_path, const std::string& ca_path, CURL* curl_handle);
};

class request {
  static std::string escape_url(const std::string& request);
  static std::string generate_request_url_stream(const session_data& session_data);
  static std::string generate_request_url_stream_parameter(const session_data& session_data);
  static std::string generate_request_url_feedback(const session_data& session_data);
  static std::string generate_request_url_feedback_body(const session_data& session_data);
  static size_t curl_data_callback(void *data, size_t size, size_t nmemb, void *userp);
public:
  static void process_feedback(session_data& session_data);
  static size_t request_feedback_cb(void *contents, size_t size, size_t nmemb, void *userp);
  static void perform_request_feedback(const ecfeed::session_data& session_data);
  static void perform_request_stream(const ecfeed::session_data& session_data, const std::function<size_t(void *data, size_t size, size_t nmemb)>* data_callback);
};

class parser {
public:
  template<typename T> static std::optional<picojson::value> process_number(const T value);
  static std::optional<picojson::value> process_string(const std::string& value);
  static std::optional<picojson::value> process_set(const std::set<std::string>& value);
  static std::optional<picojson::value> process_vector(const std::vector<std::string>& value);
  static std::optional<picojson::value> process_map(const std::map<std::string, std::string>& value);
  static std::string append_string_debug(std::string appendix, std::optional<picojson::value> box);
  static std::string append_string_request(std::string key, std::optional<picojson::value> box);
  static picojson::object& append_json(picojson::object& json, std::string field, std::optional<picojson::value> box);
};

class test_handle {
  session_data& _session_data;
  std::string _data;
  std::string _id;
  bool _pending = true;
  std::string _status;
  int _duration = -1;
  std::string _comment;
  std::map<std::string, std::string> _custom;

  friend std::ostream& operator<<(std::ostream& os, const test_handle& test_handle);
  friend std::optional<picojson::value> session_data::process_test_results() const;
  
  void _process() { 
    this->_session_data.feedback.test_cases_parsed++;
    this->_session_data.feedback.test_results[this->_id] = this;
    this->_pending = false;

    request::process_feedback(this->_session_data);
  }

public:

  test_handle(session_data& session_data, std::string data, std::string id) : _session_data(session_data), _id(id) {
    this->_data = "{\"testCase\":" + data + "}";
  }

  std::string add_feedback(bool status, int duration = -1, std::string comment = "", std::map<std::string, std::string> custom = std::map<std::string, std::string>()) {
    
    if (this->_session_data.feedback.process && this->_pending) {

      this->_status = status ? "P" : "F";
      this->_duration = duration;
      this->_comment = comment;
      this->_custom = custom;

      _process();
    }
    
    return comment != "" ? comment : "feedback";
  }

  std::string add_feedback(bool status, int duration, std::map<std::string, std::string> custom) {

    return add_feedback(status, duration, "", custom);
  }

  std::string add_feedback(bool status, std::string comment, std::map<std::string, std::string> custom) {

    return add_feedback(status, -1, comment, custom);
  }
};

struct argument {
    std::string name;
    std::string type;
    std::string value;
};

class test_arguments {
    std::vector<argument> core;
    ecfeed::test_handle* handle;

    friend std::ostream& operator<<(std::ostream& os, const test_arguments& test_arguments);

public:

    test_arguments(ecfeed::test_handle* test_handle) : handle(test_handle) {
    }

    void add(std::string name, std::string type, std::string value) {
        argument element;

        element.name = name;
        element.type = type;
        element.value = value;

        core.push_back(element);
    }

    std::vector<argument> get_vector() const {

        return core;
    }

    ecfeed::test_handle* get_handle() const {

        return handle;
    }

    int get_size() const {

        return core.size();
    }

    template<typename T>
    T get(int index) const {
        argument element = core.at(index);

        return _parse<T>(element.type, element.value);
    }

    template<typename T>
    T get(std::string name) const {

        for (int i = 0 ; i < core.size() ; i++) {
            if (core.at(i).name == name) {
                return get<T>(i);
            }
        }

        throw std::invalid_argument("Invalid argument name");
    }

    std::string get_string(int index) const { 

        return get<std::string>(index); 
    }

    bool get_bool(int index) const { 

        return get<bool>(index); 
    }

    double get_double(int index) const { 

        return get<double>(index); 
    }

    double get_float(int index) const { 

        return get<float>(index); 
    }

    int get_long(int index) const  { 

        return get<long>(index); 
    }

    int get_int(int index) const { 

        return get<int>(index); 
    }

    std::string get_string(std::string name) const { 

        return get<std::string>(name); 
    }

    bool get_bool(std::string name) const { 

        return get<bool>(name); 
    }

    double get_double(std::string name) const { 

        return get<double>(name); 
    }

    double get_float(std::string name) const { 

        return get<float>(name); 
    }

    int get_long(std::string name) const { 

        return get<long>(name); 
    }

    int get_int(std::string name) const { 

        return get<int>(name); 
    }

private:

    template<typename T>
    T _parse(std::string type, std::string value) const {

      if (type == "String") return std::any_cast<T>(value);
      else if (type == "char") return std::any_cast<T>(static_cast<char>(value.at(0)));
      else if (type == "short") return std::any_cast<T>(static_cast<short>(std::stoi(value)));
      else if (type == "byte") return std::any_cast<T>(std::stoi(value));
      else if (type == "int") return std::any_cast<T>(std::stoi(value));
      else if (type == "long") return std::any_cast<T>(std::stol(value));
      else if (type == "float") return std::any_cast<T>(std::stof(value));
      else if (type == "double") return std::any_cast<T>(std::stod(value));
      else if (type == "boolean") return std::any_cast<T>(value == "true");
      else {
        std::cerr << "Unknown parameter type: " << value << ". Converting the parameter to String\n";
        return std::any_cast<T>(value);
      }
    }
};

template<typename T>
class test_queue {
    class const_iterator : public std::iterator<std::forward_iterator_tag, test_queue> {
        const bool _END_ITERATOR;
        test_queue<T>& _queue;
            
    public:
            
        const_iterator(test_queue<T>& queue, bool end = false) :_queue(queue), _END_ITERATOR(end) {
        }

        const_iterator(const const_iterator& other) : _queue(other._queue), _END_ITERATOR(other._END_ITERATOR) {
        }

        bool operator==(const const_iterator& other) const {

            if (_END_ITERATOR && other._END_ITERATOR) {
                return true;
            }

            if (_END_ITERATOR || other._END_ITERATOR) {
                return _queue.done();
            }

            return false;
        }

        bool operator!=(const const_iterator& other) const {

            return (*this == other) == false;
        }

        const_iterator& operator++() {
            _queue.next();

            return *this;
        }

        const T& operator*() {

            return _queue.current_element();
        }
    };

    bool _done;
    std::vector<T> _data;
    const_iterator _begin;
    const_iterator _end;
    std::mutex _mutex;
    std::mutex _cv_mutex;
    std::condition_variable _cv;

    session_data _session_data;

public:

    test_queue(const session_data& session_data) : _done(false), _begin(*this), _end(*this, true) {
      _session_data = session_data;
    }

    test_queue() : _done(false), _begin(*this), _end(*this, true) {
    }

    const_iterator begin() const {

        return _begin;
    }
        
    const_iterator end() const {

        return _end;
    }

    friend class const_iterator;
    friend class test_provider;

    bool done() {
        std::unique_lock<std::mutex> cv_lock(_mutex);

        if (_data.size() != 0) {
            return false;
        } else if (_done) {
            return true;
        }

        _cv.wait(cv_lock);
        return _done;
    }

    void next() {
        std::lock_guard<std::mutex> lock(_mutex);

        _data.erase(_data.begin());
    }

    void insert(const T& element) {
        std::lock_guard<std::mutex> lock(_mutex);

        _data.push_back(element);
        _cv.notify_one();

        _session_data.feedback.test_cases_total++;
    }

    T& current_element() {
        std::unique_lock<std::mutex> cv_lock(_mutex);

        if (_data.size() > 0) {
            return _data[0];
        }

        _cv.wait(cv_lock);

        return _data[0];
    }

    void finish() {
        std::lock_guard<std::mutex> lock(_mutex);

        _done = true;
        _cv.notify_one();

        _session_data.feedback.transmission_finished = true;
        request::process_feedback(_session_data);
    }

    bool empty() {
        std::lock_guard<std::mutex> lock(_mutex);

        return _data.size() == 0;
    }

    std::vector<T> to_list() {
        std::unique_lock<std::mutex> cv_lock(_mutex);

        while (!_done) {
            _cv.wait(cv_lock);
        }

        return _data;
    }

    std::vector<std::string> get_argument_types() {
        std::unique_lock<std::mutex> cv_lock(_mutex);

        while (!_get_method_info_ready()) {
            _cv.wait(cv_lock);
        }

        return _session_data.internal.arg_types;
    }
        
    std::vector<std::string> get_argument_names() {
        std::unique_lock<std::mutex> cv_lock(_mutex);

        while (!_get_method_info_ready()) {
            _cv.wait(cv_lock);
        }

        return _session_data.internal.arg_names;
    }

private:

    void _set_method_info_ready() {

        _session_data.internal.method_info_ready = true;
    }

    bool& _get_method_info_ready() {

        return _session_data.internal.method_info_ready;
    }

    session_data& _get_session_data() {

        return _session_data;
    }

};

class params_common_getter {
protected:

    ecfeed::template_type _template_type;
    std::string _label;
    std::map<std::string, std::string> _custom;
    std::any _constraints;
    std::any _choices;
    bool _feedback;

public:

    params_common_getter() : 
        _template_type(ecfeed::template_type::csv),
        _label(std::string("")), 
        _constraints(std::string("ALL")), 
        _choices(std::string("ALL")),
        _feedback(false)
    {}

    ecfeed::template_type& get_template_type()  {
        return _template_type;
    }

    std::string& get_label() {
        return _label;
    }

    std::map<std::string, std::string> get_custom() {
        return _custom;
    }

    std::any& get_constraints() {
        return _constraints;
    }

    std::any& get_choices() {
        return _choices;
    }

    bool& get_feedback() {
      return _feedback;
    }

    virtual session_data get_session_data(const std::string& model, const std::string& method, const std::string& generator) {
        session_data data;

        data.model = model;
        data.method_name = method;
        data.connection.generator_address = generator;
        data.feedback.process = _feedback;
        
        data.main["template"] = _template_type;
        data.main["label"] = _label;
        data.main["custom"] = _custom;
        data.main["constraints"] = _constraints;
        data.main["choices"] = _choices;

        return data;
    }

};

template<typename T>
class params_common_setter : public params_common_getter {
public:

    T& template_type(ecfeed::template_type template_type) {
        _template_type = template_type;
        return self();
    }

    T& label(std::string label) {
        _label = label;
        return self();
    }

    T& custom(std::map<std::string, std::string> custom) {
        _custom = custom;
        return self();
    }

    T& constraints(std::set<std::string> constraints) {
        _constraints = constraints;
        return self();
    }

    T& constraints(std::string constraints) {
        _constraints = constraints;
        return self();
    }

    T& choices(std::map<std::string, std::set<std::string>> choices) {
        _choices = choices;
        return self();
    }

    T& choices(std::string choices) {
        _choices = choices;
        return self();
    }

    T& feedback(bool feedback) {
      _feedback = feedback;
      return self();
    }

    virtual T& self() = 0;
};

class params_nwise : public params_common_setter<params_nwise> {

    unsigned int _n;
    unsigned int _coverage;

public:

    params_nwise() : _n(2), _coverage(100)
    {}

    unsigned int get_n() {
        return _n;
    }

    unsigned int get_coverage() {
        return _coverage;
    }

    params_nwise& n(unsigned int n) {
        _n = n;
        return *this;
    }

    params_nwise& coverage(unsigned int coverage) {
        _coverage = coverage;
        return *this;
    }

    params_nwise& self() {
      return *this;
    }

    virtual session_data get_session_data(const std::string& model, const std::string& method, const std::string& generator) {
        session_data data = params_common_getter::get_session_data(model, method, generator);
      
        data.data_source = data_source::nwise;

        data.properties["n"] = std::to_string(_n);
        data.properties["coverage"] = std::to_string(_coverage);

        return data;
    }
    
};

class params_pairwise : public params_common_setter<params_pairwise> {

    unsigned int _n;
    unsigned int _coverage;

public:

    params_pairwise() : _n(2), _coverage(100)
    {}

    unsigned int get_n() {
        return _n;
    }

    unsigned int get_coverage() {
        return _coverage;
    }

    params_pairwise& coverage(unsigned int coverage) {
        _coverage = coverage;
        return *this;
    }

    params_pairwise& self() {
      return *this;
    }

    virtual session_data get_session_data(const std::string& model, const std::string& method, const std::string& generator) {
        session_data data = params_common_getter::get_session_data(model, method, generator);
      
        data.data_source = data_source::nwise;

        data.properties["n"] = std::to_string(_n);
        data.properties["coverage"] = std::to_string(_coverage);

        return data;
    }
    
};

class params_cartesian : public params_common_setter<params_cartesian> {
public:

    params_cartesian& self() {
      return *this;
    }

    virtual session_data get_session_data(const std::string& model, const std::string& method, const std::string& generator) {
        session_data data = params_common_getter::get_session_data(model, method, generator);
      
        data.data_source = data_source::cartesian;

        return data;
    }
};

class params_random : public params_common_setter<params_random> {

    unsigned int _length;
    bool _duplicates;
    bool _adaptive;

public:

    params_random() : _length(10), _duplicates(true), _adaptive(false)
    {}

    unsigned int get_length() {
        return _length;
    }

    bool get_duplicates() {
        return _duplicates;
    }

    bool get_adaptive() {
        return _adaptive;
    }

    params_random& length(unsigned int length) {
        _length = length;
        return *this;
    }

    params_random& duplicates(bool duplicates) {
        _duplicates = duplicates;
        return *this;
    }

    params_random& adaptive(bool adaptive) {
        _adaptive = adaptive;
        return *this;
    }

    params_random& self() {
        return *this;
    }

    virtual session_data get_session_data(const std::string& model, const std::string& method, const std::string& generator) {
        session_data data = params_common_getter::get_session_data(model, method, generator);
      
        data.data_source = data_source::random;

        data.properties["length"] = std::to_string(_length);
        data.properties["duplicates"] = _duplicates ? "true" : "false";
        data.properties["adaptive"] = _adaptive ? "true" : "false";

        return data;
    }
    
};

class params_static : public params_common_setter<params_static> {

    std::any _test_suites;

public:

    params_static() : _test_suites(std::string("ALL"))
    {}

    std::any& get_test_suites()  {
        return _test_suites;
    }

    params_static& test_suites(std::set<std::string> test_suites) {
        _test_suites = test_suites;
        return *this;
    }

    params_static& test_suites(std::string test_suites) {
        _test_suites = test_suites;
        return *this;
    }

    params_static& self() {
        return *this;
    }

    virtual session_data get_session_data(const std::string& model, const std::string& method, const std::string& generator) {
        session_data data = params_common_getter::get_session_data(model, method, generator);
      
        data.data_source = data_source::static_data;

        data.main["test_suites"] = _test_suites;

        return data;
    }

};

class test_provider {
    std::string _keystore_path;
    const std::string _genserver;
    const std::string _keystore_password;

    std::filesystem::path _temp_dir;
    std::filesystem::path _pkey_path;
    std::filesystem::path _cert_path;
    std::filesystem::path _ca_path;

    CURL* _curl_handle;
    std::list<std::future<void>> _running_requests;

    std::mutex _mutex;
    
public:

    std::string model;

    test_provider(const std::string& model,
                const std::filesystem::path& keystore_path = "",
                const std::string& genserver = "gen.ecfeed.com",
                const std::string& keystore_password = "changeit") :
        model(model), _genserver(genserver),
        _keystore_password(keystore_password),

        _temp_dir(std::filesystem::temp_directory_path()),
        _pkey_path(_temp_dir / _random_filename()),
        _cert_path(_temp_dir / _random_filename()),
        _ca_path(_temp_dir   / _random_filename()) {

        _keystore_path = _get_key_store(keystore_path);

        OpenSSL_add_all_algorithms();
        ERR_load_CRYPTO_strings();

        curl_global_init(CURL_GLOBAL_ALL);

        _curl_handle = curl_easy_init();

        _dump_key_store();
    }

    ~test_provider() {
        std::filesystem::remove(_pkey_path);
        std::filesystem::remove(_cert_path);
        std::filesystem::remove(_ca_path);

        curl_easy_cleanup(_curl_handle);
    }

    std::vector<std::string> get_argument_names(const std::string& method, const std::string& model = "") {

        return generate_random(method, params_random().length(0).adaptive(false).duplicates(true))->get_argument_names();
    }

    std::vector<std::string> get_argument_types(const std::string& method, const std::string& model = "") {

        return generate_random(method, params_random().length(0).adaptive(false).duplicates(true))->get_argument_types();
    }

    std::shared_ptr<test_queue<std::string>> export_nwise(const std::string& method, ecfeed::params_nwise options = ecfeed::params_nwise()) {
        std::lock_guard<std::mutex> lock(_mutex);

        session_data data = options.get_session_data(model, method, _genserver);
        data.set_connection_details(_cert_path.string(), _pkey_path.string(), _ca_path.string(), _curl_handle);
        data.connection.request_type = "requestExport";

        return _export(data);
    }

    std::shared_ptr<test_queue<test_arguments>> generate_nwise(const std::string& method, ecfeed::params_nwise options = ecfeed::params_nwise()) {
        std::lock_guard<std::mutex> lock(_mutex);

        session_data data = options.get_session_data(model, method, _genserver);
        data.set_connection_details(_cert_path.string(), _pkey_path.string(), _ca_path.string(), _curl_handle);
        data.connection.request_type = "requestData";
        
        return _generate(data);
    }

    std::shared_ptr<test_queue<std::string>> export_pairwise(const std::string& method, ecfeed::params_pairwise options = ecfeed::params_pairwise()) {
        std::lock_guard<std::mutex> lock(_mutex);

        session_data data = options.get_session_data(model, method, _genserver);
        data.set_connection_details(_cert_path.string(), _pkey_path.string(), _ca_path.string(), _curl_handle);
        data.connection.request_type = "requestExport";

        return _export(data);
    }

    std::shared_ptr<test_queue<test_arguments>> generate_pairwise(const std::string& method, ecfeed::params_pairwise options = ecfeed::params_pairwise()) {
        std::lock_guard<std::mutex> lock(_mutex);

        session_data data = options.get_session_data(model, method, _genserver);
        data.set_connection_details(_cert_path.string(), _pkey_path.string(), _ca_path.string(), _curl_handle);
        data.connection.request_type = "requestData";
        
        return _generate(data);
    }

    std::shared_ptr<test_queue<std::string>> export_random(const std::string& method, ecfeed::params_random options = ecfeed::params_random()) {
        std::lock_guard<std::mutex> lock(_mutex);

        session_data data = options.get_session_data(model, method, _genserver);
        data.set_connection_details(_cert_path.string(), _pkey_path.string(), _ca_path.string(), _curl_handle);
        data.connection.request_type = "requestExport";

        return _export(data);
    }

    std::shared_ptr<test_queue<test_arguments>> generate_random(const std::string& method, ecfeed::params_random options = ecfeed::params_random()) {
        std::lock_guard<std::mutex> lock(_mutex);

        session_data data = options.get_session_data(model, method, _genserver);
        data.set_connection_details(_cert_path.string(), _pkey_path.string(), _ca_path.string(), _curl_handle);
        data.connection.request_type = "requestData";
        
        return _generate(data);
    }

    std::shared_ptr<test_queue<std::string>> export_cartesian(const std::string& method, ecfeed::params_cartesian options = ecfeed::params_cartesian()) {
        std::lock_guard<std::mutex> lock(_mutex);

        session_data data = options.get_session_data(model, method, _genserver);
        data.set_connection_details(_cert_path.string(), _pkey_path.string(), _ca_path.string(), _curl_handle);
        data.connection.request_type = "requestExport";

        return _export(data);
    }

    std::shared_ptr<test_queue<test_arguments>> generate_cartesian(const std::string& method, ecfeed::params_cartesian options = ecfeed::params_cartesian()) {
        std::lock_guard<std::mutex> lock(_mutex);

        session_data data = options.get_session_data(model, method, _genserver);
        data.set_connection_details(_cert_path.string(), _pkey_path.string(), _ca_path.string(), _curl_handle);
        data.connection.request_type = "requestData";
        
        return _generate(data);
    }

    std::shared_ptr<test_queue<std::string>> export_static(const std::string& method, ecfeed::params_static options = ecfeed::params_static()) {
        std::lock_guard<std::mutex> lock(_mutex);

        session_data data = options.get_session_data(model, method, _genserver);
        data.set_connection_details(_cert_path.string(), _pkey_path.string(), _ca_path.string(), _curl_handle);
        data.connection.request_type = "requestExport";

        return _export(data);
    }

    std::shared_ptr<test_queue<test_arguments>> generate_static(const std::string& method, ecfeed::params_static options = ecfeed::params_static()) {
        std::lock_guard<std::mutex> lock(_mutex);

        session_data data = options.get_session_data(model, method, _genserver);
        data.set_connection_details(_cert_path.string(), _pkey_path.string(), _ca_path.string(), _curl_handle);
        data.connection.request_type = "requestData";
        
        return _generate(data);
    }
        
private:

    std::string _get_key_store(std::string keystore_path = "") {

        if (keystore_path == "") {
            std::string homepath = getenv("HOME");
            if ( access( (homepath + "/.ecfeed/security.p12").c_str(), F_OK ) != -1 ) {
                return homepath + "/.ecfeed/security.p12";
            } else if (access( (homepath + "/ecfeed/security.p12").c_str(), F_OK ) != -1) {
                return homepath + "/ecfeed/security.p12";
            } else {
                throw std::invalid_argument("Invalid key store path");
            }
        } else {
            return keystore_path;
        }
    }

    std::shared_ptr<test_queue<std::string>> _export(session_data& data) {

      std::shared_ptr<test_queue<std::string>> result(new test_queue<std::string>(data));

      std::function<size_t(void *data, size_t size, size_t nmemb)> data_cb = [result](void *data, size_t size, size_t nmemb) -> size_t {

        if (nmemb > 0) {
			
          std::string line((char*) data, (char*) data + nmemb);
          line.erase(std::remove(line.begin(), line.end(), '\n'), line.end()); // trim
          
          // Data line is in plain text. Error line is in JSON.
          
          picojson::value v;          
          std::string err = picojson::parse(v, line);
          
          if (!err.empty()) {
			  result->insert(line); // data line
			  return nmemb;
          }
          
		if (! v.is<picojson::object>()) {
			  result->insert(line);  // data line
			  return nmemb;
		}          
          
		const picojson::value::object& obj = v.get<picojson::object>();
          
		if (obj.size() <= 0) {
			  result->insert(line); // data line
			  return nmemb;
		}
	    
	    auto value = *obj.begin();
	    
	    std::string code = value.first;
	    std::string message = value.second.to_str();
	    
	    if (code == "error") { //  error line
			  std::cerr << std::endl  << "EXPORT ERROR: " << std::endl;
			  std::cerr << message << std::endl <<  std::endl;
			  return 0;
	    }	    
    	    
		result->insert(line);
        }

        return nmemb;
      };

      _running_requests.push_back(std::async(std::launch::async, [data, result, data_cb, this]() {
        request::perform_request_stream(data, &data_cb);
        result->finish();
      }));

      return result;
    }

    std::shared_ptr<test_queue<test_arguments>> _generate(session_data& session_data) {

        std::vector<std::string> types;
        std::shared_ptr<test_queue<test_arguments>> result(new test_queue<test_arguments>(session_data));

        std::function<size_t(void *data, size_t size, size_t nmemb)> data_cb = [this, result](void *data, size_t size, size_t nmemb) -> size_t {

            if (nmemb > 0) {
                std::string test_line((char*) data, (char*) data + nmemb);

                auto [name, value] = _parse_test_line(test_line);
                
                if (name == "error") {
                    std::cerr << std::endl << "ERROR: " << std::endl << value.to_str() << std::endl << std::endl;
                    return 0;
                }
                
                if (name == "info" && value.to_str() != "alive" && !result->_get_method_info_ready()) {
                    _parse_method_header(value.to_str(), result->_get_session_data());
                } else if (name == "testCase" && result->_get_method_info_ready()) {
                    result->insert(_parse_test_case(value, result->_get_session_data()));
                }
            }

            return nmemb;
        };

        _running_requests.push_back(std::async(std::launch::async, [session_data, result, data_cb, this]() {

            request::perform_request_stream(session_data, &data_cb);
            result->finish();
        }));

        return result;
    }

    void _dump_key_store() {
        std::FILE* pkey_file = fopen(_pkey_path.string().c_str(), "w");
        std::FILE* cert_file = fopen(_cert_path.string().c_str(), "w");
        std::FILE* ca_file = fopen(_ca_path.string().c_str(), "w");

        FILE* fp;
        EVP_PKEY* pkey;
        X509* cert;
        STACK_OF(X509)* ca = nullptr;
        PKCS12* p12;

        if ((fp = fopen(_keystore_path.c_str(), "rb")) == nullptr) {
            std::cerr << "Can't open the keystore file: " << _keystore_path << "\n";
            exit(1);
        }

        p12 = d2i_PKCS12_fp(fp, nullptr);
        fclose(fp);

        if (!p12) {
            std::cerr << "Error reading keystore file\n";
            ERR_print_errors_fp(stderr);
            exit (1);
        }

        if (!PKCS12_parse(p12, _keystore_password.c_str(), &pkey, &cert, &ca)) {
            fprintf(stderr, "Error parsing keystore file\n");
            ERR_print_errors_fp(stderr);
            exit (1);
        }

        PKCS12_free(p12);

        if (pkey) {
            PEM_write_PrivateKey(pkey_file, pkey, NULL, NULL, 0, NULL, NULL);
            fclose(pkey_file);
        }

        if (cert) {
            PEM_write_X509(cert_file, cert);
            fclose(cert_file);
        }

        if (ca && sk_X509_num(ca)) {

            for (unsigned i = 0; i < sk_X509_num(ca); i++) {
                PEM_write_X509(ca_file, sk_X509_value(ca, i));
            }

            fclose(ca_file);
        }

        sk_X509_pop_free(ca, X509_free);

        X509_free(cert);

        EVP_PKEY_free(pkey);
    }

    std::string _random_filename() {

        std::string result = "tmp";

        for (unsigned i = 0; i < 10; ++i) {
            result += static_cast<char>((std::rand() % ('z' - 'a')) + 'a');
        }

        return result;
    } 

    void _parse_method_header(std::string line, session_data& session_data) {
      std::replace(line.begin(), line.end(), '\'', '\"');

      picojson::value v;
      std::string err = picojson::parse(v, line);
      
      if (false == err.empty()) {
        std::cerr << "Cannot parse method header line '" << line << "': " << err << std::endl;
        return;
      }

      if (v.is<picojson::object>()) {

        try {
          session_data.internal.method_name_qualified = v.get<picojson::object>()["method"].to_str();
          session_data.internal.test_session_id = v.get<picojson::object>()["testSessionId"].to_str();
          session_data.internal.timestamp = v.get<picojson::object>()["timestamp"].to_str();

          _parse_method_info(session_data.internal.method_name_qualified, session_data);

        } catch(const std::exception& e) {
          std::cerr << "Exception caught: " << e.what() << ". The method header does not contain critical information: " << line << std::endl;
        }

      } else {
        std::cerr << "Error: method header should be a JSON object" << std::endl;
      }

    }

    void _parse_method_info(const std::string& line, session_data& session_data) {
        auto begin = line.find_first_of("(");
        auto end = line.find_last_of(")");
        auto args = line.substr(begin + 1, end - begin - 1);

        std::stringstream ss(args);
        std::string token;

        while (std::getline(ss, token, ',')) {
            token = token.substr(token.find_first_not_of(" "));
            std::string arg_type = token.substr(0, token.find(" "));
            std::string arg_name = token.substr(token.find(" ") + 1);
            session_data.internal.arg_names.push_back(arg_name);
            session_data.internal.arg_types.push_back(arg_type);
        }

        session_data.internal.method_info_ready = true;
    }

    test_arguments _parse_test_case(picojson::value test, session_data& session_data) {
        test_handle* _test_handle = new test_handle(session_data, test.serialize(), "0:" + std::to_string(session_data.feedback.test_cases_total)); 
        test_arguments _test_arguments(_test_handle);
        
        if (test.is<picojson::array>()) {
            auto test_array = test.get<picojson::array>();
            unsigned arg_index = 0;

            for (auto element : test_array) {
                    
                try {                    
                    std::string value = element.get<picojson::object>()["value"].to_str();
                    _test_arguments.add(session_data.internal.arg_names[arg_index], session_data.internal.arg_types[arg_index], value);
                } catch(const std::exception& e) {
                    std::cerr << "Exception caught: " << e.what() << ". Too many parameters in the test: " << test.to_str() << std::endl;
                }

                arg_index++;
            }

        }

        if (! test.is<picojson::array>()) {
            std::cerr << "Error: test case should be a JSON array" << std::endl;
        }

        return _test_arguments;
    }

    std::tuple<std::string, picojson::value> _parse_test_line(std::string line) {
        picojson::value v;
        std::string err = picojson::parse(v, line);
        picojson::value nothing;

        if (false == err.empty()) {
            std::cerr << "Cannot parse test line '" << line << "': " << err << std::endl;
            return std::tie("", nothing);
        }

        if (! v.is<picojson::object>()) {
            std::cerr << "Error: received line is not a JSON object" << std::endl;
            return std::tie("", nothing);
        }

        const picojson::value::object& obj = v.get<picojson::object>();
            
        if (obj.size() > 0) {
            auto value = *obj.begin();
            return std::tuple<std::string, picojson::value>(value.first, value.second);
        }

        return std::tie("", nothing);
    }
};

inline std::string _show_properties(const session_data& session_data) {
  
  return "Properties: \n" +
    parser::append_string_debug("  - n: ", session_data.process_property("n")) +
    parser::append_string_debug("  - coverage: ", session_data.process_property("coverage")) +
    parser::append_string_debug("  - length: ", session_data.process_property("length")) +
    parser::append_string_debug("  - duplicates: ", session_data.process_property("duplicates")) +
    parser::append_string_debug("  - adaptive: ", session_data.process_property("adaptive"));
}

inline std::string _show_feedback(const session_data& session_data) {
  
  auto results =  std::string("Feedback: \n") +
    parser::append_string_debug("  - test cases total: ", parser::process_number(session_data.feedback.test_cases_total)) +
    parser::append_string_debug("  - test cases parsed: ", parser::process_number(session_data.feedback.test_cases_parsed)) +
    parser::append_string_debug("  - transmission: ", parser::process_string(session_data.feedback.transmission_finished ? "finished" : "pending")) +
    parser::append_string_debug("  - handlers: ", parser::process_number(session_data.feedback.test_results.size()));

    for (auto const& [key, val] : session_data.feedback.test_results) { 
      results += key + " - "; 

      std::ostringstream ss;
      ss << *(std::any_cast<ecfeed::test_handle*>(val));

      results += ss.str();
    }

    return results;
}

inline std::string _show_internal(const session_data& session_data) {

  auto results = std::string("Internal: \n") +
    parser::append_string_debug("  - framework: ", parser::process_string(session_data.internal.framework)) +
    parser::append_string_debug("  - qualified method name: ", parser::process_string(session_data.internal.method_name_qualified)) +
    parser::append_string_debug("  - test session id: ", parser::process_string(session_data.internal.test_session_id)) +
    parser::append_string_debug("  - timestamp: ", parser::process_string(session_data.internal.timestamp)) +
    parser::append_string_debug("  - method header parsed: ", parser::process_string(session_data.internal.method_info_ready  ? "yes" : "no")) +
    parser::append_string_debug("  - method argument names: ", parser::process_vector(session_data.internal.arg_names)) +
    parser::append_string_debug("  - method argument types: ", parser::process_vector(session_data.internal.arg_types));

  return results;
}

inline std::string _show_main(const session_data& session_data) {

  auto results = std::string("SESSION DATA: \n") +
    parser::append_string_debug("  - model: ", parser::process_string(session_data.model)) +
    parser::append_string_debug("  - method: ", parser::process_string(session_data.method_name)) +
    parser::append_string_debug("  - algorithm: ", parser::process_string(data_source_url_param(session_data.data_source))) +
    parser::append_string_debug("  - template: ", session_data.process_template()) +
    parser::append_string_debug("  - label: ", session_data.process_test_session_label()) +
    parser::append_string_debug("  - custom: ", session_data.process_custom()) +
    parser::append_string_debug("  - constraints: ", session_data.process_constraints()) +
    parser::append_string_debug("  - test suites: ", session_data.process_test_suites()) +
    parser::append_string_debug("  - choices: ", session_data.process_choices());

  return results;
}

inline std::string _show_connection(const session_data& session_data) {
  
  return std::string("Connection: \n") +
    parser::append_string_debug("  - generator address: ", parser::process_string(session_data.connection.generator_address)) +
    parser::append_string_debug("  - request type: ", parser::process_string(session_data.connection.request_type)) +
    parser::append_string_debug("  - path - cert: ", parser::process_string(session_data.connection.cert_path)) +
    parser::append_string_debug("  - path - key: ", parser::process_string(session_data.connection.pkey_path)) +
    parser::append_string_debug("  - path - ca: ", parser::process_string(session_data.connection.ca_path));
}

inline std::ostream& operator<<(std::ostream& os, const session_data& session_data) {
    os << _show_main(session_data);
    os << _show_properties(session_data);
    os << _show_internal(session_data);
    os << _show_connection(session_data);
    os << _show_feedback(session_data);

    return os;
}

inline std::ostream& operator<<(std::ostream& os, const argument& argument) {

  os << argument.type << " " << argument.name << " = " << argument.value << "; ";
    
  return os;
}

inline std::ostream& operator<<(std::ostream& os, const test_arguments& test_arguments) {
        
    os << "size: " << test_arguments.get_size() << " | ";

    for (auto& x : test_arguments.get_vector()) {
        os << x;
    }

    return os;
}

inline std::ostream& operator<<(std::ostream& os, const test_handle& test_handle) {
    os << "Handler:" << std::endl;
    os << parser::append_string_debug("  - processed: ", parser::process_string(test_handle._pending ? "false" : "true"));
    os << parser::append_string_debug("  - data: ", parser::process_string(test_handle._data));
    os << parser::append_string_debug("  - id: ", parser::process_string(test_handle._id));
    os << parser::append_string_debug("  - status: ", parser::process_string(test_handle._status));
    os << parser::append_string_debug("  - duration: ", parser::process_number(test_handle._duration));
    os << parser::append_string_debug("  - comment: ", parser::process_string(test_handle._comment));
    os << parser::append_string_debug("  - custom: ", parser::process_map(test_handle._custom));

    return os;
}

//----------------------------------------------------------------------------------------------

size_t request::curl_data_callback(void *data, size_t size, size_t nmemb, void *userp) {
	
  auto callback = static_cast<std::function<size_t(void *data, size_t size, size_t nmemb)>*>(userp);

  return callback->operator()(data, size, nmemb);
}

std::string request::generate_request_url_stream_parameter(const session_data& session_data) {
   picojson::object request;

    std::optional<picojson::value> schema = session_data.process_template();
    if (schema) {
      std::string element = std::string("RAW");
      if (element.compare(schema) != 0) {
        parser::append_json(request, "template", schema);
      }
    } else if (session_data.connection.request_type == "requestExport") {
      parser::append_json(request, "template", parser::process_string("CSV"));
    }
        
    parser::append_json(request, "method", parser::process_string(session_data.method_name));
    parser::append_json(request, "model", parser::process_string(session_data.model));

    picojson::object user_data;
    parser::append_json(user_data, "dataSource", parser::process_string(data_source_url_param(session_data.data_source)));;
    parser::append_json(user_data, "choices", session_data.process_choices());
    parser::append_json(user_data, "constraints", session_data.process_constraints());
    parser::append_json(user_data, "testSuites", session_data.process_test_suites());

    picojson::object properties;
    parser::append_json(properties, "n", session_data.process_property("n"));
    parser::append_json(properties, "coverage", session_data.process_property("coverage"));
    parser::append_json(properties, "length", session_data.process_property("length"));
    parser::append_json(properties, "duplicates", session_data.process_property("duplicates"));
    parser::append_json(properties, "adaptive", session_data.process_property("adaptive"));

    user_data["properties"] = picojson::value(properties);

    std::string user_data_string = picojson::value(user_data).serialize();
    std::replace(user_data_string.begin(), user_data_string.end(), '"', '\'');

    request["userData"] = picojson::value(user_data_string);

std::cerr << picojson::value(request).serialize() << std::endl;
    return picojson::value(request).serialize();
}

std::string request::escape_url(const std::string& request) {
  std::string url = request;
  
  try {
    url = std::regex_replace(url, std::regex("\\\""), "%22");
    url = std::regex_replace(url, std::regex("'"), "%27");
    url = std::regex_replace(url, std::regex("\\{"), "%7B");
    url = std::regex_replace(url, std::regex("\\}"), "%7D");
    url = std::regex_replace(url, std::regex("\\["), "%5B");
    url = std::regex_replace(url, std::regex("\\]"), "%5D");
  } catch (const std::regex_error& e) {
    std::cerr << e.what() << std::endl;
  }

  return url;
}

std::string request::generate_request_url_stream(const session_data& session_data) {
  std::string url;

  url += "https://" + session_data.connection.generator_address + "/testCaseService";
  url += "?requestType=" + session_data.connection.request_type;
  url += "&client=cpp";
  url += "&request=" + request::generate_request_url_stream_parameter(session_data);

  // std::cerr << "url:" << url << std::endl;

  return request::escape_url(url);
}

std::string request::generate_request_url_feedback(const session_data& session_data) {
  std::string url;

  url += "https://" + session_data.connection.generator_address + "/streamFeedback";
  url += "?client=cpp";
  url += "&generationId=" + session_data.internal.test_session_id;

  // std::cerr << "url:" << url << std::endl;
        
  return request::escape_url(url);
}

std::string request::generate_request_url_feedback_body(const session_data& session_data) {
  picojson::object body;

  parser::append_json(body, "testSessionId", parser::process_string(session_data.internal.test_session_id));
  parser::append_json(body, "modelId", parser::process_string(session_data.model));
  parser::append_json(body, "methodInfo", parser::process_string(session_data.internal.method_name_qualified));
  parser::append_json(body, "framework", parser::process_string(session_data.internal.framework));
  parser::append_json(body, "timestamp", parser::process_number(std::stod(session_data.internal.timestamp)));
  parser::append_json(body, "generatorType", parser::process_string(data_source_url_param(session_data.data_source)));
  
  parser::append_json(body, "generatorOptions", session_data.process_gen_options());
  parser::append_json(body, "testSessionLabel", session_data.process_test_session_label());
  parser::append_json(body, "constraints", session_data.process_constraints());
  parser::append_json(body, "testSuites", session_data.process_test_suites());
  parser::append_json(body, "custom", session_data.process_custom());
  parser::append_json(body, "choices", session_data.process_choices());
  parser::append_json(body, "testResults", session_data.process_test_results());

  return picojson::value(body).serialize();
}

void request::process_feedback(session_data& session_data) {

  if (session_data.feedback.test_cases_parsed == session_data.feedback.test_cases_total && session_data.feedback.transmission_finished) {

    request::perform_request_feedback(session_data);

    for (auto const& [key, val] : session_data.feedback.test_results) { 
      auto index = std::any_cast<ecfeed::test_handle*>(val);
      delete index;
      index = nullptr;
    }
  }
}

size_t request::request_feedback_cb(void *contents, size_t size, size_t nmemb, void *userp)
{
	std::string line((char*) contents, (char*) contents + nmemb);
	
	picojson::value v;
	std::string err = picojson::parse(v, line);
	
	if (!err.empty()) {
		std::cerr << line << std::endl;
		return 0;
	}
	
	if (! v.is<picojson::object>()) {
		std::cerr << line << std::endl;
		return 0;
	}
	
	const picojson::value::object& obj = v.get<picojson::object>();
	 
	if (obj.size() <= 0) {
	  std::cerr << line << std::endl;
	  return 0;
	}

    auto value = *obj.begin();
    
    std::string code = value.first;
    std::string message = value.second.to_str();
    
    if (code == "status" && message == "END_DATA") {
		return nmemb;
    }
    
    if (code  != "error") {
		std::cerr << line << std::endl;
		return 0;
	}
	
    std::cerr << std::endl  << "FEEDBACK ERROR: " << std::endl;
    std::cerr << message << std::endl <<  std::endl; 
    return 0;
}
 
void request::perform_request_feedback(const ecfeed::session_data& session_data) {
  char error_buf[CURL_ERROR_SIZE];

  curl_easy_reset(session_data.connection.curl_handle);
  
  std::string url = request::generate_request_url_feedback(session_data);
  std::string body = request::generate_request_url_feedback_body(session_data);

  struct curl_slist *headers = NULL;
  headers = curl_slist_append(headers, "Accept: application/json");
  headers = curl_slist_append(headers, "Content-Type: application/json");
  headers = curl_slist_append(headers, "Content-Type: application/json; charset=utf-8");
  curl_easy_setopt(session_data.connection.curl_handle, CURLOPT_HTTPHEADER, headers); 

  curl_easy_setopt(session_data.connection.curl_handle, CURLOPT_SSLCERT, session_data.connection.cert_path.c_str());
  curl_easy_setopt(session_data.connection.curl_handle, CURLOPT_SSLCERTTYPE, "pem");
  curl_easy_setopt(session_data.connection.curl_handle, CURLOPT_SSLKEY, session_data.connection.pkey_path.c_str());
  curl_easy_setopt(session_data.connection.curl_handle, CURLOPT_CAINFO, session_data.connection.ca_path.c_str());
  curl_easy_setopt(session_data.connection.curl_handle, CURLOPT_BUFFERSIZE, 8);

  curl_easy_setopt(session_data.connection.curl_handle, CURLOPT_WRITEFUNCTION, request::request_feedback_cb);
  curl_easy_setopt(session_data.connection.curl_handle, CURLOPT_ERRORBUFFER, error_buf);

  curl_easy_setopt(session_data.connection.curl_handle, CURLOPT_URL, url.c_str());
  curl_easy_setopt(session_data.connection.curl_handle, CURLOPT_CUSTOMREQUEST, "POST");
  curl_easy_setopt(session_data.connection.curl_handle, CURLOPT_POSTFIELDS, body.c_str());
  
  auto result = curl_easy_perform(session_data.connection.curl_handle);
  
  if (result != CURLE_OK) {
    std::cerr << error_buf << std::endl;
  }

}

void request::perform_request_stream(const ecfeed::session_data& session_data, const std::function<size_t(void *data, size_t size, size_t nmemb)>* data_callback) {
  char error_buf[CURL_ERROR_SIZE];

  curl_easy_reset(session_data.connection.curl_handle);
  std::string url = request::generate_request_url_stream(session_data);

  curl_easy_setopt(session_data.connection.curl_handle, CURLOPT_SSLCERT, session_data.connection.cert_path.c_str());
  curl_easy_setopt(session_data.connection.curl_handle, CURLOPT_SSLCERTTYPE, "pem");
  curl_easy_setopt(session_data.connection.curl_handle, CURLOPT_SSLKEY, session_data.connection.pkey_path.c_str());
  curl_easy_setopt(session_data.connection.curl_handle, CURLOPT_CAINFO, session_data.connection.ca_path.c_str());
  curl_easy_setopt(session_data.connection.curl_handle, CURLOPT_BUFFERSIZE, 8);

  curl_easy_setopt(session_data.connection.curl_handle, CURLOPT_WRITEFUNCTION, request::curl_data_callback);
  curl_easy_setopt(session_data.connection.curl_handle, CURLOPT_WRITEDATA, (void *)data_callback);

  curl_easy_setopt(session_data.connection.curl_handle, CURLOPT_ERRORBUFFER, error_buf);

  curl_easy_setopt(session_data.connection.curl_handle, CURLOPT_URL, url.c_str());

  auto result = curl_easy_perform(session_data.connection.curl_handle);

  if (result != CURLE_OK) {
    std::cerr << error_buf << std::endl;
  }

}

//----------------------------------------------------------------------------------------------

std::optional<picojson::value> session_data::process_property(const std::string parameter) const {
  std::optional<picojson::value> box;

  auto field = properties.find(parameter);
  if (field != properties.end()) {
    box = picojson::value(std::any_cast<std::string>(field->second));
  }

  return box;
}

std::optional<picojson::value> session_data::process_gen_options() const {
  std::optional<picojson::value> box;
  std::string body = "";

  std::string delimeter = "";
  for (auto const& [key, val] : properties) { 
    body += delimeter + key + "=" + val;
    delimeter = ", ";
  }

  return picojson::value(body);
}

std::optional<picojson::value> session_data::process_test_results() const {
  std::optional<picojson::value> box;

  picojson::object body;
  for (auto const& [key, val] : feedback.test_results) { 
    ecfeed::test_handle* test_handle = std::any_cast<ecfeed::test_handle*>(val);

    picojson::object data;

    parser::append_json(data, "data", parser::process_string(test_handle->_data));
    parser::append_json(data, "status", parser::process_string(test_handle->_status));
    parser::append_json(data, "comment", parser::process_string(test_handle->_comment));
    parser::append_json(data, "duration", parser::process_number(test_handle->_duration));
    parser::append_json(data, "custom", parser::process_map(test_handle->_custom));

    body[key] = picojson::value(data);
  }

  return picojson::value(body);
}

std::optional<picojson::value> session_data::process_test_session_label() const {
  std::optional<picojson::value> box;

  auto field = main.find("label");
  if (field != main.end()) {
    box = picojson::value(std::any_cast<std::string>(field->second));
  }

  return box;
}

std::optional<picojson::value> session_data::process_constraints() const {
  std::optional<picojson::value> box;

  auto field = main.find("constraints");
  if (field != main.end()) {
    
    try {
      std::string value = std::any_cast<std::string>(field->second);

      if (value != "" && value != "ALL") {
        box = picojson::value(value);
      }
    } catch(std::bad_any_cast) {
      box = parser::process_set(std::any_cast<std::set<std::string>>(field->second));
    }
  }

  return box;
}

std::optional<picojson::value> session_data::process_test_suites() const {
  std::optional<picojson::value> box;

  auto field = main.find("test_suites");
  if (field != main.end()) {
    
    try {
      std::string value = std::any_cast<std::string>(field->second);

      if (value != "" && value != "ALL") {
        box = picojson::value(value);
      }
    } catch(std::bad_any_cast) {
      box = parser::process_set(std::any_cast<std::set<std::string>>(field->second));
    }
  }

  return box;
}

std::optional<picojson::value> session_data::process_custom() const {
  std::optional<picojson::value> box;

  auto field = main.find("custom");
  if (field != main.end()) {
    box = parser::process_map(std::any_cast<std::map<std::string, std::string>>(field->second));
  }

  return box;
}

std::optional<picojson::value> session_data::process_choices() const {
  std::optional<picojson::value> box;
  
  auto field = main.find("choices");
  if (field != main.end()) {
    
    try {
      std::string value = std::any_cast<std::string>(field->second);

      if (value != "" && value != "ALL") {
        box = picojson::value(value);
      }
    } catch(std::bad_any_cast) {
      picojson::object parser;
        
      for (auto x : std::any_cast<std::map<std::string, std::set<std::string>>>(field->second)) {
        parser::append_json(parser, x.first, parser::process_set(x.second));
      }
        
      box = picojson::value(parser);
    }
  }

  return box;
}

std::optional<picojson::value> session_data::process_template() const {
  std::optional<picojson::value> box;

  auto field = main.find("template");
  if (field != main.end()) {
    auto element = std::any_cast<ecfeed::template_type>(field->second);
    box = picojson::value(template_type_url_param(element));
  }

  return box;
}

void session_data::set_connection_details(const std::string& cert_path, const std::string& pkey_path, const std::string& ca_path, CURL* curl_handle) {
  connection.cert_path = cert_path;
  connection.pkey_path = pkey_path;
  connection.ca_path = ca_path;
  connection.curl_handle = curl_handle;
}

//----------------------------------------------------------------------------------------------

template<typename T>
std::optional<picojson::value> parser::process_number(const T value) {
  std::optional<picojson::value> box;

  if (value > 0) {
    box = picojson::value(static_cast<double>(value));
  }

  return box;
}

std::optional<picojson::value> parser::process_string(const std::string& value) {
  std::optional<picojson::value> box;

  if (value != "") {
    box = picojson::value(value);
  }

  return box;
}

std::optional<picojson::value> parser::process_set(const std::set<std::string>& value) {
  std::optional<picojson::value> box;
  std::vector<picojson::value> parser;
      
  if (value.size() > 0) {
    
    for (auto x : value) {
      parser.push_back(picojson::value(x));
    }
  
    box = picojson::value(parser);
  }
      
  return box;
}

std::optional<picojson::value> parser::process_vector(const std::vector<std::string>& value) {
  std::optional<picojson::value> box;
  std::vector<picojson::value> parser;
      
  if (value.size() > 0) {
    
    for (auto x : value) {
      parser.push_back(picojson::value(x));
    }
  
    box = picojson::value(parser);
  }
      
  return box;
}

std::optional<picojson::value> parser::process_map(const std::map<std::string, std::string>& value) {
  std::optional<picojson::value> box;
  
  if (value.size() > 0) {
    picojson::object parser;

    for (auto const& [key, val] : value) { 
      parser[key] = picojson::value(val);
    }

    box = picojson::value(parser);
  }

  return box;
}

std::string parser::append_string_debug(std::string appendix, std::optional<picojson::value> box) {

  if (box) {
    return appendix + box.value().serialize() + "\n";
  } else {
    return appendix + "-\n";
  }
}

std::string parser::append_string_request(std::string key, std::optional<picojson::value> box) {

  if (box) {
    picojson::object body;
    body[key] = box.value();
    return picojson::value(body).serialize();
  } else {
    return "";
  }
}

picojson::object& parser::append_json(picojson::object& json, std::string field, std::optional<picojson::value> box) {
  
  if (box) {
    json[field] = box.value();
  }

  return json;
}

}

#endif
