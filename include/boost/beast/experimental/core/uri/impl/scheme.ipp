//
// Copyright (c) 2016-2017 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

#ifndef BOOST_BEAST_CORE_IMPL_SCHEME_IPP
#define BOOST_BEAST_CORE_IMPL_SCHEME_IPP

#include <boost/beast/experimental/core/uri/detail/scheme.hpp>

namespace boost {
namespace beast {
namespace uri {

inline
known_scheme
string_to_scheme(string_view s)
{
    return detail::string_to_scheme(s);
}

inline
string_view
to_string(known_scheme s)
{
    return detail::to_string(s);
}

inline
bool
is_special(known_scheme s)
{
    return detail::is_special(s);
}

} // uri
} // beast
} // boost

#endif
