//
// Copyright (c) 2018 jackarain (jack dot wgm at gmail dot com)
// Copyright (c) 2018 oneiric (oneiric at i2pmail dot org)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

#include <boost/beast/experimental/core/uri/scheme.hpp>

namespace boost {
namespace beast {
namespace uri {

url_parts parse_url(string_view url) {
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
  auto part_start = b;
  bool is_ipv6 = false;
  url_parts parts;

  while (b != e) {
    auto c = *b++;
    switch (state) {
    case scheme_start:
      if (detail::is_alpha(c)) {
        state = scheme;
        continue;
      }
      return {};
    case scheme:
      if (detail::is_alpha(c) || detail::is_digit(c) || c == '+' ||
          c == '-' || c == '.')
        continue;
      if (c == ':') {
        std::size_t size = b - part_start - 1;
        parts.scheme = to_string(string_to_scheme({part_start, size}));
        state = slash_start;
        continue;
      }
      return {};
    case slash_start:
      if (c == '/') {
        if (parts.scheme == "file") {
          if (b != e && *b == '/')
            b++;
          else
            return {};
        }
        state = slash;
        continue;
      }
      return {};
    case slash:
      if (c == '/') {
        auto search = b;
        bool find_at = false;
        while (search != e) {
          if (*search == '/')
            break;
          if (*search == '@') {
            find_at = true;
            break;
          }
          search++;
        }
        part_start = b;
        if (find_at) {
          state = username_start;
          continue;
        }
        state = host_start;
        continue;
      }
      return {};
    case username_start:
      if (detail::is_uchar(c) || detail::is_sub_delims(c)) {
        state = username;
        continue;
      }
      return {};
    case username:
      if (detail::is_uchar(c) || detail::is_sub_delims(c))
        continue;
      if (c == ':') {
        parts.username = string_view(part_start, b - part_start - 1);
        part_start = b;
        state = password_start;
        continue;
      }
      if (c == '@') {
        parts.username = string_view(part_start, b - part_start - 1);
        part_start = b;
        state = host_start;
        continue;
      }
      return {};
    case password_start:
      if (detail::is_uchar(c) || detail::is_sub_delims(c)) {
        state = password;
        continue;
      }
      return {};
    case password:
      if (detail::is_uchar(c) || detail::is_sub_delims(c) || c == '#')
        continue;
      if (c == '@') {
        parts.password = string_view(part_start, b - part_start - 1);
        part_start = b;
        state = host_start;
        continue;
      }
      return {};
    case host_start:
      if (detail::is_unreserved(c) || detail::is_sub_delims(c) ||
          c == '%' || c == '[') {
        if (c == '[') {
          is_ipv6 = true;
          part_start = b;
        }

        state = host;
        continue;
      }
      return {};
    case host:
      if (is_ipv6) {
        if (c == ']') {
          parts.host = string_view(part_start, b - part_start - 1);
          if (b == e)
            return parts;
          if (*b == ':') {
            b++;
            part_start = b;
            state = port_start;
            continue;
          }
          if (*b == '/') {
            b++;
            part_start = b;
            state = path;
            continue;
          }
          return {};
        } else if (c == '/') {
          return {};
        } else if (c == ':' || detail::is_digit(c) || c == 'a' ||
                   c == 'b' || c == 'c' || c == 'd' || c == 'e' || c == 'f')
          continue;
      } else {
        if (b == e) // end
        {
          parts.host = string_view(part_start, b - part_start);
          return parts;
        }
        if (c == ':') {
          parts.host = string_view(part_start, b - part_start - 1);
          part_start = b;
          state = port_start;
          continue;
        }
      }
      if (c == '/') {
        parts.host = string_view(part_start, b - part_start - 1);
        part_start = b - 1;
        state = path;
        continue;
      }
      if (detail::is_unreserved(c) || detail::is_sub_delims(c) ||
          c == '%' || c == ':')
        continue;
      return {};
    case port_start:
      if (detail::is_digit(c)) {
        state = port;
        continue;
      }
      return {};
    case port:
      if (b == e) // no path
      {
        if (c == '/') {
          parts.port = string_view(part_start, b - part_start - 1);
          parts.path = string_view(b - 1, 1);
        } else if (detail::is_digit(c))
          parts.port = string_view(part_start, b - part_start);
        return parts;
      }
      if (c == '/') {
        parts.port = string_view(part_start, b - part_start - 1);
        part_start = b - 1;
        state = path;
        continue;
      }
      if (detail::is_digit(c))
        continue;
      return {};
    case path:
      if (b == e) {
        if (c == '?' || c == '#')
          parts.path = string_view(part_start, b - part_start - 1);
        else
          parts.path = string_view(part_start, b - part_start);
        return parts;
      }
      if (c == '?') {
        parts.path = string_view(part_start, b - part_start - 1);
        part_start = b;
        state = query;
        continue;
      }
      if (c == '#') {
        parts.path = string_view(part_start, b - part_start - 1);
        part_start = b;
        state = fragment;
        continue;
      }
      if (detail::is_hsegment(c) || c == '/')
        continue;
      return {};
    case query:
      if (b == e) {
        if (c == '#')
          parts.query = string_view(part_start, b - part_start - 1);
        else
          parts.query = string_view(part_start, b - part_start);
        return parts;
      }
      if (c == '#') {
        parts.query = string_view(part_start, b - part_start - 1);
        part_start = b;
        state = fragment;
        continue;
      }
      if (detail::is_hsegment(c) || detail::is_sub_delims(c) ||
          c == '/' || c == '?')
        continue;
      return {};
    case fragment:
      if (b == e) {
        parts.fragment = string_view(part_start, b - part_start);
        return parts;
      }
      if (detail::is_hsegment(c) || detail::is_sub_delims(c) ||
          c == '/' || c == '?')
        continue;
      return {};
    }
  }

  return {};
}

} // namespace uri
} // namespace beast
} // namespace boost
