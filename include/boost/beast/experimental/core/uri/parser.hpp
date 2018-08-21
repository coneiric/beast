//
// Copyright (c) 2018 jackarain (jack dot wgm at gmail dot com)
// Copyright (c) 2018 oneiric (oneiric at i2pmail dot org)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

#ifndef BOOST_BEAST_CORE_URI_URL_PARSER_HPP
#define BOOST_BEAST_CORE_URI_URL_PARSER_HPP

#include <boost/beast/core/string.hpp>
#include <boost/beast/experimental/core/uri/buffer.hpp>
#include <boost/beast/experimental/core/uri/error.hpp>

namespace boost {
namespace beast {
namespace uri   {

/** Parse URL based on RFC3986

    Hierarchical URL

      [scheme:][//[user[:pass]@]host[:port]][/path][?query][#fragment]

    @param url URL to parse into component parts
    @return Parsed URL parts
*/
void parse_url(string_view url, buffer& out, error_code& ec);

} // namespace uri
} // namespace beast
} // namespace boost

#include <boost/beast/experimental/core/uri/impl/parser.ipp>

#endif  // BOOST_BEAST_CORE_URI_URL_PARSER_HPP
