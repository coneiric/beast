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

#include <vector>
#include <boost/beast/core/string.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/beast/experimental/core/uri/buffer.hpp>
#include <boost/beast/experimental/core/uri/detail/rfc3986.hpp>

namespace boost {
namespace beast {
namespace uri {
namespace detail {

template <typename It, typename Res = reserved<It>>
class parser_impl {
  rules<It> r;

public:
  It parse_scheme(It first, It last, buffer &out, error_code &ec) {
    using beast::detail::ascii_tolower;
    using boost::spirit::ascii::alpha;
    using boost::spirit::ascii::char_;
    using boost::spirit::ascii::digit;
    using boost::spirit::qi::no_skip;
    using boost::spirit::qi::parse;

    /*  scheme         ; = ALPHA / *(ALPHA / DIGIT / "-" / "." / "+") / ":"
     */
    auto const scheme =
        no_skip[alpha >> *(alpha | digit | char_("-.+")) >> char_(':')];
    if (!parse(first, last, scheme, out.data())) {
      ec = error::syntax;
      return first;
    }
    // Convert the scheme to lowercase
    for (auto &ch : out.data())
      ch = ascii_tolower(ch);
    auto const end = out.end() - 1; // skip the ending delimiter
    out.scheme(out.part_from(out.begin(), end));
    return first;
  }

  It parse_username(It first, It last, buffer &out, error_code &ec) {
    using boost::spirit::ascii::char_;
    using boost::spirit::qi::no_skip;
    using boost::spirit::qi::parse;

    /*  username       ; = *(uchar / sub_delims) / ":" / "@"
     */
    auto const username = no_skip[*(r.uchar | r.sub_delims) >> char_(":@")];
    auto size = out.size();
    if (!parse(first, last, username, out.data())) {
      ec = error::syntax;
      return first;
    }
    auto start = out.begin();
    if (size > 0)
      start += size;
    auto const end = out.end() - 1; // skip ending delimiter
    out.username(out.part_from(start, end));
    return first;
  }

  It parse_password(It first, It last, buffer &out, error_code &ec) {
    using boost::spirit::ascii::char_;
    using boost::spirit::qi::no_skip;
    using boost::spirit::qi::parse;

    /*  password       ; = ":" / *(unreserved / sub_delims) / "@"
     */
    auto const password =
        no_skip[':' >> *(r.unreserved | r.sub_delims) >> char_('@')];
    auto size = out.size();
    if (!parse(first, last, password, out.data())) {
      ec = error::syntax;
      return first;
    }
    auto start = out.begin();
    if (size > 0)
      start += size;
    auto const end = out.end() - 1; // skip ending delimiter
    out.password(out.part_from(start, end));
    return first;
  }

  It parse_host(It first, It last, buffer &out, error_code &ec) {
    using boost::spirit::ascii::char_;
    using boost::spirit::ascii::digit;
    using boost::spirit::ascii::xdigit;
    using boost::spirit::qi::no_skip;
    using boost::spirit::qi::parse;
    using boost::spirit::qi::repeat;

    auto const hex_group =
        no_skip[-repeat(1, 2)[char_(':')] >> repeat(1, 2)[xdigit]];
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
    auto const ipv6 =
        no_skip['[' >> repeat(1, 6)[hex_group] >> ']' >> -char_(":/?#")];
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
    auto const ipv4_reg =
        no_skip[*(r.unreserved | r.sub_delims | char_('%')) >> -char_(":/?#")];
    auto size = out.size();
    if (*first == '[') {
      // Parse IPv6
      if (!parse(first, last, ipv6, out.data())) {
        ec = error::syntax;
        return first;
      }
    } else {
      // Parse IPv4 or registered name
      if (!parse(first, last, ipv4_reg, out.data())) {
        ec = error::syntax;
        return first;
      }
    }
    auto start = out.begin();
    if (size > 0)
      start += size;
    auto end = out.end();
    if (out.back() == ':' || out.back() == '/' || out.back() == '?' ||
        out.back() == '#')
      --end; // skip the ending delimiter
    out.host(out.part_from(start, end));
    return first;
  }

  It parse_port(It first, It last, buffer &out, error_code &ec) {
    using boost::spirit::ascii::char_;
    using boost::spirit::ascii::digit;
    using boost::spirit::qi::no_skip;
    using boost::spirit::qi::parse;

    /*  port          ; = ":" / *DIGIT
     */
    auto const port = no_skip[':' >> +digit >> -char_("/?#")];
    auto size = out.size();
    if (!parse(first, last, port, out.data())) {
      ec = error::syntax;
      return first;
    }
    auto start = out.begin();
    if (size > 0)
      start += size;
    auto end = out.end();
    if (out.back() == '/' || out.back() == '?' || out.back() == '#')
      --end; // skip ending delimiter
    out.port(out.part_from(start, end));
    return first;
  }

  It parse_authority(It first, It last, buffer &out, error_code &ec) {
    using boost::spirit::ascii::char_;
    using boost::spirit::qi::matches;
    using boost::spirit::qi::no_skip;
    using boost::spirit::qi::parse;

    /*  userinfo       ; = *( unreserved / pct-encoded / sub-delims / ":" ) / @
     *
     *  Useful as a separate rule for searching without capturing
     */
    auto const userinfo =
        no_skip[*(r.uchar | r.sub_delims | char_(":#")) >> char_('@')];
    if (first[0] != '/' || first[1] != '/') {
      ec = error::syntax;
      return first;
    }
    out.data().push_back(*first++);
    out.data().push_back(*first++);
    auto pos = first;
    auto search = first;
    // Check for a username[:password] section
    bool found{false};
    parse(search, last, matches[userinfo], found);
    if (found) {
      pos = parse_username(pos, last, out, ec);
      if (!ec && pos != last && *(pos - 1) == ':')
        pos = parse_password(--pos, last, out, ec); // start at ':' delimiter
    }
    // Valid authority needs a host
    if (ec || pos == last) {
      ec = error::syntax;
      return pos;
    }
    pos = parse_host(pos, last, out, ec);
    if (ec)
      return pos;
    if (*(pos - 1) == ':')
      pos = parse_port(--pos, last, out, ec);  // start at ':' delimiter
    return pos;
  }

  It parse_path(It first, It last, buffer &out, error_code &ec) {
    using boost::spirit::ascii::char_;
    using boost::spirit::qi::no_skip;
    using boost::spirit::qi::parse;

    /*  path          ; = path-absolute = "/" [ segment-nz *( "/" segment ) ]
     *  segment       ; = *pchar
     *  segment-nz    ; = 1*pchar
     */
    auto const path =
        no_skip[char_('/') >> -(r.pchar >> *(r.pchar | char_('/'))) >>
                -char_("?#")];
    auto size = out.size();
    if (!parse(first, last, path, out.data())) {
      ec = error::syntax;
      return first;
    }
    auto start = out.begin();
    if (size > 0)
      start += size;
    auto end = out.end();
    if (out.back() == '?' || out.back() == '#')
      --end;  // Skip the ending delimiter
    out.path(out.part_from(start, end));
    return first;
  }

  It parse_query(It first, It last, buffer &out, error_code &ec) {
    using boost::spirit::ascii::char_;
    using boost::spirit::qi::no_skip;
    using boost::spirit::qi::parse;

    /*  query         ; = "?" / *( pchar / "/" / "?" )
     */
    auto const query = no_skip['?' >> *r.qchar >> -char_("#")];
    auto size = out.size();
    if (!parse(first, last, query, out.data())) {
      ec = error::syntax;
      return first;
    }
    if (out.size() == size)
      return first;
    auto start = out.begin();
    if (size > 0)
      start += size;
    auto end = out.end();
    if (out.back() == '#')
      --end; // skip ending delimiter
    if (start == end) // empty query
      return first;
    out.query(out.part_from(start, end));
    return first;
  }

  It parse_fragment(It first, It last, buffer &out, error_code &ec) {
    using boost::spirit::ascii::char_;
    using boost::spirit::qi::no_skip;
    using boost::spirit::qi::parse;


    /*  fragment      ; = = "#" / *( pchar / "/" / "?" )
     */
    auto const fragment = no_skip['#' >> *r.qchar];
    auto size = out.size();
    if (!parse(first, last, fragment, out.data())) {
      ec = error::syntax;
      return first;
    }
    if (out.size() == size)
      return first;
    auto start = out.begin();
    if (size > 0)
      start += size;
    out.fragment(out.part_from(start, out.end()));
    return first;
  }

  template <typename Input = string_view>
  inline void parse_absolute_form(buffer &out, Input in, error_code &ec) {
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
    if (*(pos - 1) == '/')
      pos = parse_path(--pos, in.end(), out, ec); // start at '/'
    if (ec)
      return;
    if (*(pos - 1) == '?')
      pos = parse_query(--pos, in.end(), out, ec); // start at '?'
    if (ec)
      return;
    if (*(pos - 1) == '#')
      pos = parse_fragment(--pos, in.end(), out, ec);  // start at '#'
  }
};

using parser = parser_impl<string_view::iterator>;

} // detail

} // uri
} // beast
} // boost

#endif  // BOOST_BEAST_CORE_URI_DETAIL_PARSER_HPP
