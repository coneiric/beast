//
// Copyright (c) 2016-2018 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

#ifndef BOOST_BEAST_CORE_URI_IMPL_PARSE_IPP
#define BOOST_BEAST_CORE_URI_IMPL_PARSE_IPP

#include <boost/beast/core/uri/error.hpp>
#include <boost/beast/core/uri/input.hpp>
#include <stdexcept>

namespace boost {
namespace beast {
namespace uri {

/*
    absolute-URI    = scheme ":" hier-part [ "?" query ]

    https://tools.ietf.org/html/rfc3986#section-4.3

    To allow for transition to absoluteURIs in all requests in future
    versions of HTTP, all HTTP/1.1 servers MUST accept the absoluteURI
    form in requests, even though HTTP/1.1 clients will only generate
    them in requests to proxies.
*/
inline
void
parse_absolute_form(
    buffer& out,
    string_view s,
    error_code& ec)
{
    input in{s};
    out.clear();
    ec.assign(0, ec.category());
    try
    {
        detail::parser p;
        p.parse_absolute_form(out, in, ec);
    }
    catch(std::length_error const&)
    {
        //ec = error::overflow
    }
}

} // uri
} // beast
} // boost

#endif
