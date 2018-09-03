//
// Copyright (c) 2018 oneiric (oneiric at i2pmail dot org)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

#ifndef BOOST_BEAST_CORE_URI_DETAIL_PARSER_HPP
#define BOOST_BEAST_CORE_URI_DETAIL_PARSER_HPP

#include <boost/beast/core/string.hpp>
#include <boost/beast/experimental/core/uri/buffer.hpp>
#include <boost/beast/experimental/core/uri/rfc3986.hpp>

namespace boost {
namespace beast {
namespace uri {
namespace detail {

class parser_impl {
  template <typename It>
  It append_decoded_or_char(It first, It last, buffer& out, error_code& ec)
  {
    if (*first == '%')
      {
        auto ch = pct_decode(++first, last, ec);
        if (ec)
          return first;
        out.push_back(ch);
        first += 2;
      }
    else
      out.push_back(*(first++));

    return first;
  }

  template <typename It>
  It parse_scheme(It first, It last, buffer &out, error_code &ec) {
    using beast::detail::ascii_tolower;

    /*  scheme         ; = ALPHA / *(ALPHA / DIGIT / "-" / "." / "+") / ":"
     */
    if (!is_alpha(*first)) {
      ec = error::syntax;
      return first;
    }
    while (*first != ':') {
      // Convert the scheme to lowercase
      out.push_back(ascii_tolower(*(first++)));
      if (first == last) {
        ec = error::syntax;
        return first;
      }
      if (!is_alpha(*first) && !is_digit(*first) && *first != '-' &&
          *first != '.' && *first != '+' && *first != ':') {
        ec = error::syntax;
        return first;
      }
    }
    out.scheme(out.part_from(out.begin(), out.end()));
    out.push_back(*(first++));
    return first;
  }

  template <typename It>
  It parse_username(It first, It last, buffer &out, error_code &ec) {
    /*  username       ; = *(unreserved / pct-encoded / sub_delims) / ":" / "@"
     */
    auto const size = out.size();
    while (*first != ':' && *first != '@') {
      if (first == last) {
        ec = error::syntax;
        return first;
      }
      if (!is_pchar(*first)) {
        ec = error::syntax;
        return first;
      }
			first = append_decoded_or_char(first, last, out, ec);
      if (ec)
        return first;
    }
    auto const start = out.begin() + size;
    out.username(out.part_from(start, out.end()));
    out.push_back(*(first++));
    return first;
  }

  template <typename It>
  It parse_password(It first, It last, buffer &out, error_code &ec) {
    /*  password       ; = ":" / *(unreserved / pct-encoded / sub_delims) / "@"
     */
    auto const size = out.size();
    while (*first != '@') {
      if (first == last) {
        ec = error::syntax;
        return first;
      }
      if (!is_pchar(*first)){
        ec = error::syntax;
        return first;
      }
			first = append_decoded_or_char(first, last, out, ec);
      if (ec)
        return first;
    }
    auto const start = out.begin() + size;
    out.password(out.part_from(start, out.end()));
    out.push_back(*(first++));
    return first;
  }

  template <typename It>
  It parse_host(It first, It last, buffer &out, error_code &ec) {
    auto const size = out.size();
    if (*first == '[') {
      /*  IP-literal = "[" ( IPv6address / IPvFuture  ) "]"
       *
       *  IPv6address =                              6( h16 ":" ) ls32
       *                /                       "::" 5( h16 ":" ) ls32
       *                / [               h16 ] "::" 4( h16 ":" ) ls32
       *                / [ *1( h16 ":" ) h16 ] "::" 3( h16 ":" ) ls32
       *                / [ *2( h16 ":" ) h16 ] "::" 2( h16 ":" ) ls32
       *                / [ *3( h16 ":" ) h16 ] "::"    h16 ":"   ls32
       *                / [ *4( h16 ":" ) h16 ] "::"              ls32
       *                / [ *5( h16 ":" ) h16 ] "::"              h16
       *                / [ *6( h16 ":" ) h16 ] "::"
       *
       *  TODO: need better IPv6 parsing.
       *  Current rule is serviceable, but doesn't cover all cases.
       */
      while (++first != last && *first != ']') {
        if (*first != ':' && !is_hex(*first)) {
          ec = error::syntax;
          return first;
        }
        out.push_back(*first);
      }
      if (first != last && *first != ']') {
        ec = error::syntax;
        return first;
      }
      if(++first != last && *first != ':' && *first != '/' && *first != '?' &&
          *first != '#') {
        ec = error::syntax;
        return first;
      }
      auto const start = out.begin() + size;
      out.host(out.part_from(start, out.end()));
      if (first != last)
        out.push_back(*first);
      return first;
    } else { // Parse IPv4 or registered name
      /*  IPv4address = dec-octet "." dec-octet "." dec-octet "." dec-octet
       *
       *  dec-octet   = DIGIT                 ; 0-9
       *              / %x31-39 DIGIT         ; 10-99
       *              / "1" 2DIGIT            ; 100-199
       *              / "2" %x30-34 DIGIT     ; 200-249
       *              / "25" %x30-35          ; 250-255
       *
       *  reg-name    = *( unreserved / pct-encoded / sub-delims )
       *
       *  TODO: need distinct IPv4 parsing.
       *  Current rule captures IPv4, but is too loose, allowing invalid IPv4.
       */
      while (first != last && *first != ':' && *first != '/' && *first != '?' &&
             *first != '#') {
        if (!is_pchar(*first)) {
          ec = error::syntax;
          return first;
        }
        first = append_decoded_or_char(first, last, out, ec);
        if (ec)
          return first;
      }
      auto const start = out.begin() + size;
      out.host(out.part_from(start, out.end()));
      return first;
    }
  }

  template <typename It>
  It parse_port(It first, It last, buffer &out, error_code &ec) {
    /*  port          ; = ":" / *DIGIT
     */
    if (*first != ':') {
      ec = error::syntax;
      return first;
    }
    out.push_back(*(first++));
    auto const size = out.size();
    while (first != last && *first != '/' && *first != '?' && *first != '#')
    {
      if (!is_digit(*first)){
        ec = error::syntax;
        return first;
      }
      out.push_back(*(first++));
    }
    auto const start = out.begin() + size;
    out.port(out.part_from(start, out.end()));
    if (first != last)
      out.push_back(*first);
    return first;
  }

  template <typename It>
  It parse_authority(It first, It last, buffer &out, error_code &ec) {
    if (first[0] != '/' || first[1] != '/') {
      ec = error::syntax;
      return first;
    }
    out.push_back(*first++);
    out.push_back(*first++);
    auto pos = first;
    auto search = first;
    /*  userinfo    ; = *( unreserved / pct-encoded / sub-delims / ":" ) / "@"
     *
     *  Check for a username[:password] section
     */
    while (++search != last && *search != '@') {
      if (!is_pchar(*search)) {
        ec = error::mismatch;
        break;
      }
    }
    if (!ec && search != last && *search == '@') {
      pos = parse_username(pos, last, out, ec);
      if (!ec && pos != last && *(pos - 1) == ':')
        pos = parse_password(pos, last, out, ec);
    }
    // Valid authority needs a host
    if ((ec && ec != error::mismatch) || pos == last) {
      ec = error::syntax;
      return pos;
    }
    ec.assign(0, ec.category());
    pos = parse_host(pos, last, out, ec);
    if (ec)
      return pos;
    if (*pos == ':')
      pos = parse_port(pos, last, out, ec);
    return pos;
  }

  template <typename It>
  It parse_path(It first, It last, buffer &out, error_code &ec) {
    /*  path          ; = path-absolute = "/" [ segment-nz *( "/" segment ) ]
     *  segment       ; = *pchar
     *  segment-nz    ; = 1*pchar
     */
    auto const size = out.size();
    if (*first != '/') {
      ec = error::syntax;
      return first;
    }
    out.push_back(*(first++));
    if (first != last && *first != '?' && *first != '#') {
      if (!is_pchar(*first)) {
        ec = error::syntax;
        return first;
      }
      while (first != last && *first != '?' && *first != '#') {
        if (*first != '/' && !is_pchar(*first)) {
          ec = error::syntax;
          return first;
        }
        first = append_decoded_or_char(first, last, out, ec);
        if (ec)
          return first;
      }
    }
    auto const start = out.begin() + size;
    out.path(out.part_from(start, out.end()));
    if (first != last)
      out.push_back(*first);
    return first;
  }

  template <typename It>
  It parse_query(It first, It last, buffer &out, error_code &ec) {
    /*  query         ; = "?" / *( pchar / "/" / "?" ) / "#"
     */
    if (*first != '?') {
      ec = error::syntax;
      return first;
    }
    out.push_back(*(first++));
    auto const size = out.size();
    while (first != last && *first != '#') {
      if (!is_pchar(*first) && *first != '/' && *first != '?') {
        ec = error::syntax;
        return first;
      }
      first = append_decoded_or_char(first, last, out, ec);
      if (ec)
        return first;
    }
    if (out.size() == size)
      return first;
    auto const start = out.begin() + size;
    if (start == out.end()) // empty query
      return first;
    out.query(out.part_from(start, out.end()));
    return first;
  }

  template <typename It>
  It parse_fragment(It first, It last, buffer &out, error_code &ec) {
    /*  fragment      ; = = "#" / *( pchar / "/" / "?" )
     */
    if (*first != '#') {
      ec = error::syntax;
      return first;
    }
    out.push_back(*(first++));
    auto const size = out.size();
    while (first != last){
      if (!is_pchar(*first) && *first != '/' && *first != '?') {
        ec = error::syntax;
        return first;
      }
      first = append_decoded_or_char(first, last, out, ec);
      if (ec)
        return first;
    }
    if (out.size() == size)  // empty fragment
      return first;
    auto const start = out.begin() + size;
    out.fragment(out.part_from(start, out.end()));
    return first;
  }

public:
  template <typename Input = string_view>
  inline void parse_absolute_form(Input in, buffer &out, error_code &ec) {
    if (in.empty()) {
      ec = error::syntax;
      return;
    }
    auto pos = parse_scheme(in.begin(), in.end(), out, ec);
    if (ec || pos == in.end()) {
      ec = error::syntax;
      return;
    }
    pos = parse_authority(pos, in.end(), out, ec);
    if (ec)
      return;
    if (*pos == '/')
      pos = parse_path(pos, in.end(), out, ec);
    if (ec)
      return;
    if (*pos == '?')
      pos = parse_query(pos, in.end(), out, ec);
    if (ec)
      return;
    if (*pos == '#')
      pos = parse_fragment(pos, in.end(), out, ec);
  }
};

} // detail

} // uri
} // beast
} // boost

#endif  // BOOST_BEAST_CORE_URI_DETAIL_PARSER_HPP
