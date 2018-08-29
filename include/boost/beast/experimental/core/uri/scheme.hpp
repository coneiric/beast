//
// Copyright (c) 2016-2018 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

#ifndef BOOST_BEAST_URI_SCHEME_HPP
#define BOOST_BEAST_URI_SCHEME_HPP

#include <boost/beast/core/detail/config.hpp>
#include <boost/beast/core/string.hpp>

namespace boost {
namespace beast {
namespace uri {

/** Identifies a known URI scheme
*/
enum class known_scheme : unsigned char
{
    unknown = 0,

    ftp,
    file,
    gopher,
    http,
    https,
    ws,
    wss
};

/** Return the scheme for a non-normalized string, if known
*/
known_scheme
string_to_scheme(string_view s);

/** Return the normalized string for a known scheme
*/
string_view
to_string(known_scheme s);

/** Return `true` if the known scheme is a special scheme
    The list of special schemes is as follows:
    ftp, file, gopher, http, https, ws, wss.
    @param s The known-scheme constant to check
    @return `true` if the scheme is special
*/
bool
is_special(known_scheme s);

} // uri
} // beast
} // boost

#include <boost/beast/experimental/core/uri/impl/scheme.ipp>

#endif
