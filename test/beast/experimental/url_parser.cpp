//
// Copyright (c) 2016-2018 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

// Test that header file is self-contained.
#include <boost/beast/experimental/core/uri/url_parser.hpp>

#include <boost/beast/unit_test/suite.hpp>

namespace boost {
namespace beast {
namespace uri   {

class url_parser_test : public unit_test::suite {
public:
  void doParse(string_view url,
      string_view scheme = "",
      string_view username = "",
      string_view password = "",
      string_view host = "",
      string_view port = "",
      string_view path = "",
      string_view query = "",
      string_view fragment = "")
  {
    auto parts = parse_url(url);

    BEAST_EXPECT(parts.scheme == scheme);
    BEAST_EXPECT(parts.username == username);
    BEAST_EXPECT(parts.password == password);
    BEAST_EXPECT(parts.host == host);
    BEAST_EXPECT(parts.port == port);
    BEAST_EXPECT(parts.path == path);
    BEAST_EXPECT(parts.query == query);
    BEAST_EXPECT(parts.fragment == fragment);
  }

  void
  testParse()
  {
    // IPv4
    doParse("ws://1.1.1.1",              "ws", "", "", "1.1.1.1");
    doParse("wss://1.1.1.1",             "wss", "", "", "1.1.1.1");
    doParse("ftp://1.1.1.1",             "ftp", "", "", "1.1.1.1");
    doParse("file:///1.1.1.1",           "file", "", "", "1.1.1.1");
    doParse("http://1.1.1.1",            "http", "", "", "1.1.1.1");
    doParse("https://1.1.1.1",           "https", "", "", "1.1.1.1");
    doParse("gopher://1.1.1.1",          "gopher", "", "", "1.1.1.1");
    doParse("a://1.1.1.1",               "<unknown>", "", "", "1.1.1.1");
    doParse("http://a:b@1.1.1.1",        "http", "a", "b", "1.1.1.1");
    doParse("http://1.1.1.1:80",         "http", "", "", "1.1.1.1", "80");
    doParse("http://1.1.1.1:80",         "http", "", "", "1.1.1.1", "80");
    doParse("http://1.1.1.1:80/",        "http", "", "", "1.1.1.1", "80", "/");
    doParse("http://1.1.1.1:80/?",       "http", "", "", "1.1.1.1", "80", "/");
    doParse("http://1.1.1.1:80//",       "http", "", "", "1.1.1.1", "80", "//");
    doParse("http://1.1.1.1:80//a",      "http", "", "", "1.1.1.1", "80", "//a");
    doParse("http://1.1.1.1:80/a",       "http", "", "", "1.1.1.1", "80", "/a");
    doParse("http://1.1.1.1:80/a/b",     "http", "", "", "1.1.1.1", "80", "/a/b");
    doParse("http://1.1.1.1:80/a?b",     "http", "", "", "1.1.1.1", "80", "/a", "b");
    doParse("http://1.1.1.1:80/a?b=1",   "http", "", "", "1.1.1.1", "80", "/a", "b=1");
    doParse("http://1.1.1.1:80/a#",      "http", "", "", "1.1.1.1", "80", "/a");
    doParse("http://1.1.1.1:80/#a",      "http", "", "", "1.1.1.1", "80", "/", "", "a");
    doParse("http://1.1.1.1:80/a#a",     "http", "", "", "1.1.1.1", "80", "/a", "", "a");
    doParse("http://1.1.1.1:80/a?b=1#",  "http", "", "", "1.1.1.1", "80", "/a", "b=1");
    doParse("http://1.1.1.1:80/a?b=1#a", "http", "", "", "1.1.1.1", "80", "/a", "b=1", "a");
    // IPv6
    doParse("http://[::1]",              "http", "", "", "::1");
    doParse("http://[::1]:80",           "http", "", "", "::1", "80");
    doParse("http://[fe80:1010::1010]",  "http", "", "", "fe80:1010::1010");
    // Registered name
    doParse("http://boost.org",          "http", "", "", "boost.org");
  }

  void
  run() override
  {
    testParse();
  }
};

BEAST_DEFINE_TESTSUITE(beast,core,url_parser);

}  // uri
}  // beast
}  // boost
