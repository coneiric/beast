//
// Copyright (c) 2018 jackarain (jack dot wgm at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

#ifndef BOOST_BEAST_CORE_URI_URL_PARSER_HPP
#define BOOST_BEAST_CORE_URI_URL_PARSER_HPP

#include <boost/beast/core/string.hpp>

namespace boost {
namespace beast {
namespace uri   {

struct url_parts {
  string_view scheme;
  string_view username;
  string_view password;
  string_view host;
  string_view port;
  string_view path;
  string_view query;
  string_view fragment;
};

/** Parse URL based on RFC3986

    Hierarchical URL

      [scheme:][//[user[:pass]@]host[:port]][/path][?query][#fragment]

    @param url URL to parse into component parts
    @return Parsed URL parts
*/
url_parts parse_url(string_view url);

} // namespace uri
} // namespace beast
} // namespace boost

#include <boost/beast/experimental/core/uri/impl/url_parser.ipp>

#endif  // BOOST_BEAST_CORE_URI_URL_PARSER_HPP
