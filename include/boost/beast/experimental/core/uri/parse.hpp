//
// Copyright (c) 2016-2017 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

#ifndef BOOST_BEAST_CORE_URI_PARSE_HPP
#define BOOST_BEAST_CORE_URI_PARSE_HPP

#include <boost/beast/core/error.hpp>
#include <boost/beast/core/string.hpp>
#include <boost/beast/core/uri/buffer.hpp>

namespace boost {
namespace beast {
namespace uri {

/*
    5.3 Request Target
    https://tools.ietf.org/html/rfc7230#section-5.3

    request-target  = origin-form
                    / absolute-form
                    / authority-form
                    / asterisk-form

    origin-form     = absolute-path [ "?" query ]

    absolute-form   = absolute-URI

    authority-form  = authority
 
    asterisk-form   = "*"
*/        
        
/*  Parse a URI in origin form.

    The origin form is used in direct requests to an
    origin server, except for CONNECT or OPTIONS *.

    @par Example

    The request-target is this example HTTP request
    uses origin form:

    @code
        GET /index.html HTTP/1.1\r\n
    @endcode

    @par BNF

    @code
        origin-form     = absolute-path [ "?" query ]
    @endcode
*/
void
parse_origin_form(error_code& ec);

/** Parse a URI string in absolute form.

    Absolute form is used in HTTP requests to a proxy,
    except for CONNECT or OPTIONS *

    absolute-URI    = scheme ":" hier-part [ "?" query ]

    https://tools.ietf.org/html/rfc3986#section-4.3

    To allow for transition to absoluteURIs in all requests in future
    versions of HTTP, all HTTP/1.1 servers MUST accept the absoluteURI
    form in requests, even though HTTP/1.1 clients will only generate
    them in requests to proxies.
*/
void
parse_absolute_form(
    buffer& out,
    string_view s,
    error_code& ec);

/*  Used in CONNECT requests

    The authority-form of request-target is only used for CONNECT requests
    https://tools.ietf.org/html/rfc7230#section-5.3.3

    Although CONNECT must exclude userinfo and '@' we parse it anyway and
    let the caller decide what to do with it.

    authority-form  = authority
*/
void
parse_authority_form(error_code& ec);

/*  Used for server-wide OPTIONS requests
*/
void
parse_asterisk_form(error_code& ec);

} // uri
} // beast
} // boost

#include <boost/beast/core/uri/impl/parse.ipp>

#endif
