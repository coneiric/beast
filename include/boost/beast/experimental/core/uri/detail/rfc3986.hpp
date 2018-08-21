//
// Copyright (c) 2016-2017 Vinnie Falco (vinnie dot falco at gmail dot com)
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

bool is_unreserved(const char c) {
  if (is_alpha(c) || is_digit(c) || c == '-' ||
      c == '.' || c == '_' || c == '~')
    return true;
  return false;
}

bool is_uchar(const char c) {
  if (is_unreserved(c) || c == ';' || c == '?' || c == '&' ||
      c == '=')
    return true;
  return false;
}

bool is_hsegment(const char c) {
  if (is_uchar(c) || c == ';' || c == ':' || c == '@' || c == '&' ||
      c == '=')
    return true;
  return false;
}

bool is_sub_delims(const char c) {
  if (c == '!' || c == '$' || c == '\'' || c == '(' || c == ')' || c == '*' ||
      c == '+' || c == ',' || c == '=')
    return true;
  return false;
}

} // detail
} // uri
} // beast
} // boost

#endif
