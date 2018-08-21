//
// Copyright (c) 2018 oneiric (oneiric at i2pmail dot org)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

// Test that header file is self-contained.
#include <boost/beast/experimental/core/uri/parser.hpp>

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
    static_buffer<1024> out;
    error_code ec;
    parse_url(url, out, ec);

    BEAST_EXPECT(!ec);

    BEAST_EXPECT(out.scheme() == scheme);
    BEAST_EXPECT(out.username() == username);
    BEAST_EXPECT(out.password() == password);
    BEAST_EXPECT(out.host() == host);
    BEAST_EXPECT(out.port() == port);
    BEAST_EXPECT(out.path() == path);
    BEAST_EXPECT(out.query() == query);
    BEAST_EXPECT(out.fragment() == fragment);
  }

  void
  testParse()
  {
    // IPv4
    doParse("a://1.1.1.1",               "a", "", "", "1.1.1.1");
    doParse("A://1.1.1.1",               "a", "", "", "1.1.1.1");
    doParse("ws://1.1.1.1",              "ws", "", "", "1.1.1.1");
    doParse("wss://1.1.1.1",             "wss", "", "", "1.1.1.1");
    doParse("ftp://1.1.1.1",             "ftp", "", "", "1.1.1.1");
    doParse("file:///1.1.1.1",           "file", "", "", "1.1.1.1");
    doParse("http://1.1.1.1",            "http", "", "", "1.1.1.1");
    doParse("https://1.1.1.1",           "https", "", "", "1.1.1.1");
    doParse("gopher://1.1.1.1",          "gopher", "", "", "1.1.1.1");
    doParse("http://a:b@1.1.1.1",        "http", "a", "b", "1.1.1.1");
    doParse("http://1.1.1.1:80",         "http", "", "", "1.1.1.1", "80");
    doParse("http://1.1.1.1:80/",        "http", "", "", "1.1.1.1", "80", "/");
    doParse("http://1.1.1.1:80?",        "http", "", "", "1.1.1.1", "80");
    doParse("http://1.1.1.1:80#",        "http", "", "", "1.1.1.1", "80");
    doParse("http://1.1.1.1/",           "http", "", "", "1.1.1.1", "", "/");
    doParse("http://1.1.1.1?",           "http", "", "", "1.1.1.1");
    doParse("http://1.1.1.1#",           "http", "", "", "1.1.1.1");
    doParse("http://1.1.1.1/?",          "http", "", "", "1.1.1.1", "", "/");
    doParse("http://1.1.1.1//",          "http", "", "", "1.1.1.1", "", "//");
    doParse("http://1.1.1.1//a",         "http", "", "", "1.1.1.1", "", "//a");
    doParse("http://1.1.1.1/a",          "http", "", "", "1.1.1.1", "", "/a");
    doParse("http://1.1.1.1/a/b",        "http", "", "", "1.1.1.1", "", "/a/b");
    doParse("http://1.1.1.1/a?b",        "http", "", "", "1.1.1.1", "", "/a", "b");
    doParse("http://1.1.1.1/a?b=1",      "http", "", "", "1.1.1.1", "", "/a", "b=1");
    doParse("http://1.1.1.1/a#",         "http", "", "", "1.1.1.1", "", "/a");
    doParse("http://1.1.1.1/#a",         "http", "", "", "1.1.1.1", "", "/", "", "a");
    doParse("http://1.1.1.1/a#a",        "http", "", "", "1.1.1.1", "", "/a", "", "a");
    doParse("http://1.1.1.1/a?b=1#",     "http", "", "", "1.1.1.1", "", "/a", "b=1");
    doParse("http://1.1.1.1/a?b=1#a",    "http", "", "", "1.1.1.1", "", "/a", "b=1", "a");
    // IPv6
    doParse("http://[::1]",              "http", "", "", "::1");
    doParse("http://[::1]:80",           "http", "", "", "::1", "80");
    doParse("http://[::1]/",             "http", "", "", "::1", "", "/");
    doParse("http://[::1]?",             "http", "", "", "::1");
    doParse("http://[::1]#",             "http", "", "", "::1");
    doParse("http://[fe80:1010::1010]",  "http", "", "", "fe80:1010::1010");
    // Registered name
    doParse("http://boost.org",          "http", "", "", "boost.org");
    // Path
    doParse("h://1/abcdefghijklmnopqrstuvwxyz0123456789", "h", "", "", "1", "", "/abcdefghijklmnopqrstuvwxyz0123456789");
    doParse("h://1/-._~;&=:@?",                           "h", "", "", "1", "", "/-._~;&=:@");
    // Query
    doParse("h://1?abcdefghijklmnopqrstuvwxyz0123456789", "h", "", "", "1", "", "", "abcdefghijklmnopqrstuvwxyz0123456789");
    doParse("h://1?-._~;&=:@?!$&\'()*+,/",                "h", "", "", "1", "", "", "-._~;&=:@?!$&\'()*+,/");
    // Fragment
    doParse("h://1#abcdefghijklmnopqrstuvwxyz0123456789", "h", "", "", "1", "", "", "", "abcdefghijklmnopqrstuvwxyz0123456789");
    doParse("h://1#-._~;&=:@?!$&\'()*+,/",                "h", "", "", "1", "", "", "", "-._~;&=:@?!$&\'()*+,/");
    // Potentially malicious request smuggling
    doParse("http://boost.org#@evil.com/",                "http", "", "", "boost.org", "", "", "", "@evil.com/");
  }

  void
  badParse(string_view url)
  {
    static_buffer<1024> out;
    error_code ec;
    parse_url(url, out, ec);

    BEAST_EXPECT(ec);
  }

  void
  testBadParse()
  {
    // Attack test-cases courtesy of Orange Tsai: A New Era Of SSRF Exploiting URL Parser In Trending Programming Languages
    badParse("http://1.1.1.1 &@2.2.2.2# @3.3.3.3/");
    badParse("http://127.0.0.1:25/%0D%0AHELO boost.org%0D%0AMAIL FROM: admin@boost.org:25");
    badParse("https://127.0.0.1 %0D%0AHELO boost.org%0D%0AMAIL FROM: admin@boost.org:25");
    badParse("http://127.0.0.1:11211:80");
    badParse("http://foo@evil.com:80@boost.org/");
    badParse("http://foo@127.0.0.1 @boost.org/");
    badParse("http://boost.org/\xFF\x2E\xFF\x2E");
    badParse("http://boost.org/%0D%0ASLAVEOF%20boost.org%206379%0D%0A");
    badParse("http://0\r\n SLAVEOF boost.org 6379\r\n :80");
    badParse("http://foo@127.0.0.1:11211@boost.org:80");
    badParse("http://foo@127.0.0.1 @boost.org:11211");
  }

  void
  run() override
  {
    testParse();
    testBadParse();
  }
};

BEAST_DEFINE_TESTSUITE(beast,core,url_parser);

}  // uri
}  // beast
}  // boost
