//
// Copyright (c) 2016-2018 Vinnie Falco (vinnie dot falco at gmail dot com)
// Copyright (c) 2018 oneiric (oneiric at i2pmail dot org)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

#ifndef BOOST_BEAST_CORE_URI_DETAIL_RFC3986_HPP
#define BOOST_BEAST_CORE_URI_DETAIL_RFC3986_HPP

namespace boost {
namespace beast {
namespace uri {
namespace detail {

bool is_alpha(const char c) {
  unsigned constexpr a = 'a';
  return ((static_cast<unsigned>(c) | 32) - a) < 26U;
}

bool is_digit(const char c) {
  unsigned constexpr zero = '0';
  return (static_cast<unsigned>(c) - zero) < 10;
}


/*  gen-delims      = ":" / "/" / "?" / "#" / "[" / "]" / "@"
*/
bool is_gen_delims(const char c) {
  return c == ':' || c == '/' || c == '?' || c == '#' || c == '[' || c == ']' ||
         c == '@';
}

/*  sub-delims      = "!" / "$" / "&" / "'" / "(" / ")"
                          / "*" / "+" / "," / ";" / "="
*/
bool is_sub_delims(const char c) {
  return c == '!' || c == '$' || c == '&' || c == '\'' || c == '(' ||
         c == ')' || c == '*' || c == '+' || c == ',' || c == '=';
}

/*  reserved        = gen-delims / sub-delims
*/
bool is_reserved(const char c) {
  return is_gen_delims(c) || is_sub_delims(c);
}

/*  unreserved      = ALPHA / DIGIT / "-" / "." / "_" / "~"
*/
bool is_unreserved(const char c) {
  return is_alpha(c) || is_digit(c) || c == '-' || c == '.' || c == '_' ||
         c == '~';
}

/*  pchar           = unreserved / sub-delims / ":" / "@"
*/
bool is_pchar(const char c) {
  return is_unreserved(c) || is_sub_delims(c) || c == ':' || c == '@';
}

/*  qchar           = pchar / "/" / "?"
*/
bool is_qchar(const char c) {
  return is_pchar(c) || c == '/' || c == '?';
}

/*  uchar           = unreserved / ";" / "?" / "&" / "="
*/
bool is_uchar(const char c) {
  return is_unreserved(c) || c == ';' || c == '?' || c == '&' || c == '=';
}

/*  hsegment        = uchar / ":" / "@"
*/
bool is_hsegment(const char c) {
  return is_uchar(c) || c == ':' || c == '@';
}

} // detail
} // uri
} // beast
} // boost

#endif
