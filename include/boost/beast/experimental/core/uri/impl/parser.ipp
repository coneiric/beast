//
// Copyright (c) 2018 jackarain (jack dot wgm at gmail dot com)
// Copyright (c) 2018 oneiric (oneiric at i2pmail dot org)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

#ifndef BOOST_BEAST_CORE_URI_IMPL_PARSER_IPP
#define BOOST_BEAST_CORE_URI_IMPL_PARSER_IPP

namespace boost {
namespace beast {
namespace uri {

void parse_url(string_view url, buffer& out, error_code& ec) {
  enum {
    scheme_start,
    scheme,
    slash_start,
    slash,
    username_start,
    username,
    password_start,
    password,
    host_start,
    host,
    port_start,
    port,
    path,
    query,
    fragment
  } state = scheme_start;

  auto b = url.begin();
  auto e = url.end();
  auto start = out.begin();
  bool is_ipv6 = false;

  while (b != e) {
    auto c = *b++;
    switch (state) {
    case scheme_start:
      if (detail::is_alpha(c)) {
        out.append(beast::detail::ascii_tolower(c));
        state = scheme;
        continue;
      }
      ec = error::syntax;
      return;
    case scheme:
      if (detail::is_alpha(c) || detail::is_digit(c) || c == '+' || c == '-' ||
          c == '.') {
        out.append(beast::detail::ascii_tolower(c));
        continue;
      }
      if (c == ':') {
        out().scheme = out.piece_from(start);
        start = out.end();
        state = slash_start;
        continue;
      }
      ec = error::syntax;
      return;
    case slash_start:
      if (c == '/') {
        if (out.scheme() == "file") {
          if (b != e && *b == '/')
            b++;
          else {
            ec = error::mismatch;
            return;
          }
        }
        state = slash;
        continue;
      }
      ec = error::syntax;
      return;
    case slash:
      if (c == '/') {
        auto search = b;
        bool find_at = false;
        while (search != e) {
          if (*search == '/')
            break;
          if (*search == '?')
            break;
          if (*search == '#')
            break;
          if (*search == '@') {
            find_at = true;
            break;
          }
          search++;
        }
        if (find_at) {
          state = username_start;
          continue;
        }
        state = host_start;
        continue;
      }
      ec = error::syntax;
      return;
    case username_start:
      if (detail::is_uchar(c) || detail::is_sub_delims(c)) {
        out.append(c);
        state = username;
        continue;
      }
      ec = error::syntax;
      return;
    case username:
      if (detail::is_uchar(c) || detail::is_sub_delims(c)) {
        out.append(c);
        continue;
      }
      if (c == ':') {
        out().username = out.piece_from(start);
        start = out.end();
        state = password_start;
        continue;
      }
      if (c == '@') {
        out().username = out.piece_from(start);
        start = out.end();
        state = host_start;
        continue;
      }
      ec = error::syntax;
      return;
    case password_start:
      if (detail::is_uchar(c) || detail::is_sub_delims(c)) {
        out.append(c);
        state = password;
        continue;
      }
      ec = error::syntax;
      return;
    case password:
      if (detail::is_uchar(c) || detail::is_sub_delims(c) || c == '#') {
        out.append(c);
        continue;
      }
      if (c == '@') {
        out().password = out.piece_from(start);
        start = out.end();
        state = host_start;
        continue;
      }
      ec = error::syntax;
      return;
    case host_start:
      if (detail::is_unreserved(c) || detail::is_sub_delims(c) || c == '%' ||
          c == '[') {
        if (c == '[')
          is_ipv6 = true;
        else
          out.append(c);
        state = host;
        continue;
      }
      ec = error::syntax;
      return;
    case host:
      if (is_ipv6) {
        if (c == ']') {
          out().host = out.piece_from(start);
          start = out.end();
          if (b == e)
            return;
          c = *b++; // end of host, check delimiter
        } else if (c == ':' || detail::is_digit(c) || c == 'a' || c == 'b' ||
                   c == 'c' || c == 'd' || c == 'e' || c == 'f') {
          out.append(c);
          continue;
        } else {
          ec = error::syntax;
          return;
        }
      } else {
        if (detail::is_unreserved(c) || detail::is_sub_delims(c) || c == '%') {
          out.append(c);
          if (b == e) {
            out().host = out.piece_from(start);
            return;
          }
          continue;
        }
      }
      if (c == ':') {
        if (!is_ipv6)
          out().host = out.piece_from(start);
        start = out.end();
        state = port_start;
        continue;
      }
      if (c == '/') {
        if (!is_ipv6)
          out().host = out.piece_from(start);
        start = out.end();
        out.append(c);
        if (b == e) {
          out().path = out.piece_from(start);
          return;
        }
        state = path;
        continue;
      }
      if (c == '?') {
        if (!is_ipv6)
          out().host = out.piece_from(start);
        start = out.end();
        state = query;
        continue;
      }
      if (c == '#') {
        if (!is_ipv6)
          out().host = out.piece_from(start);
        start = out.end();
        state = fragment;
        continue;
      }
      if (b == e) {
        out().host = out.piece_from(start);
        return;
      }
      ec = error::syntax;
      return;
    case port_start:
      if (detail::is_digit(c)) {
        out.append(c);
        state = port;
        continue;
      }
      ec = error::syntax;
      return;
    case port:
      if (detail::is_digit(c)) {
        out.append(c);
        if (b == e) {
          out().port = out.piece_from(start);
          return;
        }
        continue;
      }
      if (c == '/') {
        out().port = out.piece_from(start);
        start = out.end();
        out.append(c);
        if (b == e) {
          out().path = out.piece_from(start);
          return;
        }
        state = path;
        continue;
      }
      if (c == '?') {
        out().port = out.piece_from(start);
        start = out.end();
        state = query;
        continue;
      }
      if (c == '#') {
        out().port = out.piece_from(start);
        start = out.end();
        state = fragment;
        continue;
      }
      ec = error::syntax;
      return;
    case path:
      if (c == '?') {
        out().path = out.piece_from(start);
        start = out.end();
        state = query;
        continue;
      }
      if (c == '#') {
        out().path = out.piece_from(start);
        start = out.end();
        state = fragment;
        continue;
      }
      if (detail::is_hsegment(c) || c == '/') {
        out.append(c);
        if (b == e) {
          out().path = out.piece_from(start);
          return;
        }
        continue;
      }
      ec = error::syntax;
      return;
    case query:
      if (detail::is_hsegment(c) || detail::is_sub_delims(c) ||
          c == '/' || c == '?') {
        out.append(c);
        if (b == e) {
          out().query = out.piece_from(start);
          return;
        }
        continue;
      }
      if (c == '#') {
        out().query = out.piece_from(start);
        start = out.end();
        state = fragment;
        continue;
      }
      ec = error::syntax;
      return;
    case fragment:
      if (detail::is_hsegment(c) || detail::is_sub_delims(c) ||
          c == '/' || c == '?') {
        out.append(c);
        if (b == e) {
          out().fragment = out.piece_from(start);
          return;
        }
        continue;
      }
      ec = error::syntax;
      return;
    }
  }
}

} // namespace uri
} // namespace beast
} // namespace boost

#endif 
