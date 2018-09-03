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

class parser_test : public unit_test::suite {
public:
  void doParseAbsolute(string_view url,
      string_view scheme = "",
      string_view username = "",
      string_view password = "",
      string_view host = "",
      string_view port = "",
      string_view path = "",
      string_view query = "",
      string_view fragment = "")
  {
    buffer out;
    error_code ec;
    parse_absolute_form(out, url, ec);

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
  testParseAbsolute()
  {
    // IPv4
    doParseAbsolute("WS://1.1.1.1",              "ws", "", "", "1.1.1.1");
    doParseAbsolute("ws://1.1.1.1",              "ws", "", "", "1.1.1.1");
    doParseAbsolute("wss://1.1.1.1",             "wss", "", "", "1.1.1.1");
    doParseAbsolute("ftp://1.1.1.1",             "ftp", "", "", "1.1.1.1");
    doParseAbsolute("http://1.1.1.1",            "http", "", "", "1.1.1.1");
    doParseAbsolute("https://1.1.1.1",           "https", "", "", "1.1.1.1");
    doParseAbsolute("gopher://1.1.1.1",          "gopher", "", "", "1.1.1.1");
    doParseAbsolute("a://1.1.1.1",               "a", "", "", "1.1.1.1");
    doParseAbsolute("http://a@1.1.1.1",          "http", "a", "", "1.1.1.1");
    doParseAbsolute("http://a:b@1.1.1.1",        "http", "a", "b", "1.1.1.1");
    doParseAbsolute("http://1.1.1.1:80",         "http", "", "", "1.1.1.1", "80");
    doParseAbsolute("http://1.1.1.1:80",         "http", "", "", "1.1.1.1", "80");
    // Empty path
    doParseAbsolute("http://1.1.1.1?a=b",        "http", "", "", "1.1.1.1", "", "", "a=b");
    doParseAbsolute("http://1.1.1.1#a",          "http", "", "", "1.1.1.1", "", "", "", "a");
    doParseAbsolute("http://1.1.1.1:80?a=b",     "http", "", "", "1.1.1.1", "80", "", "a=b");
    doParseAbsolute("http://1.1.1.1:80#a",       "http", "", "", "1.1.1.1", "80", "", "", "a");
    // Non-empty Path
    doParseAbsolute("http://1.1.1.1:80/",        "http", "", "", "1.1.1.1", "80", "/");
    doParseAbsolute("http://1.1.1.1:80/?",       "http", "", "", "1.1.1.1", "80", "/");
    doParseAbsolute("http://1.1.1.1:80/a",       "http", "", "", "1.1.1.1", "80", "/a");
    doParseAbsolute("http://1.1.1.1:80/a/",      "http", "", "", "1.1.1.1", "80", "/a/");
    doParseAbsolute("http://1.1.1.1:80/a/b",     "http", "", "", "1.1.1.1", "80", "/a/b");
    doParseAbsolute("http://1.1.1.1:80/a?b",     "http", "", "", "1.1.1.1", "80", "/a", "b");
    doParseAbsolute("http://1.1.1.1:80/a?b=1",   "http", "", "", "1.1.1.1", "80", "/a", "b=1");
    doParseAbsolute("http://1.1.1.1:80/a#",      "http", "", "", "1.1.1.1", "80", "/a");
    doParseAbsolute("http://1.1.1.1:80/#a",      "http", "", "", "1.1.1.1", "80", "/", "", "a");
    doParseAbsolute("http://1.1.1.1:80/a#a",     "http", "", "", "1.1.1.1", "80", "/a", "", "a");
    doParseAbsolute("http://1.1.1.1:80/a?b=1#",  "http", "", "", "1.1.1.1", "80", "/a", "b=1");
    doParseAbsolute("http://1.1.1.1:80/a?b=1#a", "http", "", "", "1.1.1.1", "80", "/a", "b=1", "a");
    // IPv6
    doParseAbsolute("http://[::1]",              "http", "", "", "::1");
    doParseAbsolute("http://[::1]/a",            "http", "", "", "::1", "", "/a");
    doParseAbsolute("http://[::1]?a",            "http", "", "", "::1", "", "", "a");
    doParseAbsolute("http://[::1]#a",            "http", "", "", "::1", "", "", "", "a");
    doParseAbsolute("http://[::1]:80",           "http", "", "", "::1", "80");
    doParseAbsolute("http://[fe80:1010::1010]",  "http", "", "", "fe80:1010::1010");
    // Registered name
    doParseAbsolute("https://boost.org",          "https", "", "", "boost.org");
  }

  void
  run() override
  {
    testParseAbsolute();
  }
};

BEAST_DEFINE_TESTSUITE(beast,core,uri_parser);

// Test the private implementation
class parser_detail_test : public unit_test::suite {
  detail::parser p;

  void
  doParseScheme(string_view in, string_view scheme) {
    buffer out;
    error_code ec;

    auto ret = p.parse_scheme(in.begin(), in.end(), out, ec);

    BEAST_EXPECT(!ec);
    BEAST_EXPECT(ret != in.begin());
    BEAST_EXPECT(out.scheme() == scheme);
  }

  void
  testParseScheme() {
    doParseScheme("a:",  "a");
    doParseScheme("A:",  "a");
    doParseScheme("ab:", "ab");
    doParseScheme("aB:", "ab");
    doParseScheme("a-:", "a-");
    doParseScheme("a+:", "a+");
    doParseScheme("a.:", "a.");
  }

  void
  doParseUsername(string_view in, string_view username) {
    buffer out;
    error_code ec;

    p.parse_username(in.begin(), in.end(), out, ec);

    BEAST_EXPECT(ec != error::syntax);
    BEAST_EXPECT(out.username() == username);
  }

  void
  testParseUsername() {
    doParseUsername("2:",  "2");
    doParseUsername("a:",  "a");
    doParseUsername("a@",  "a");
    doParseUsername("ab@", "ab");
    doParseUsername("a-@", "a-");
    doParseUsername("a$@", "a$");
    doParseUsername("a~@", "a~");
  }

  void
  doParsePassword(string_view in, string_view password) {
    buffer out;
    error_code ec;

    auto ret = p.parse_password(in.begin(), in.end(), out, ec);

    BEAST_EXPECT(!ec);
    BEAST_EXPECT(ret != in.begin());
    BEAST_EXPECT(out.password() == password);
  }

  void
  testParsePassword() {
    doParsePassword(":2@",  "2");
    doParsePassword(":a@",  "a");
    doParsePassword(":ab@", "ab");
    doParsePassword(":a-@", "a-");
    doParsePassword(":a$@", "a$");
    doParsePassword(":a~@", "a~");
  }

  void
  doParseHost(string_view in, string_view host) {
    buffer out;
    error_code ec;

    auto ret = p.parse_host(in.begin(), in.end(), out, ec);

    BEAST_EXPECT(!ec);
    BEAST_EXPECT(ret != in.begin());
    BEAST_EXPECT(out.host() == host);
  }

  void
  testParseHost() {
    doParseHost("1.1.1.1",          "1.1.1.1");
    doParseHost("boost.org",        "boost.org");
    doParseHost("b:",               "b");
    doParseHost("b/",               "b");
    doParseHost("b?",               "b");
    doParseHost("b#",               "b");
    doParseHost("[::1]",            "::1");
    doParseHost("[::1]/",           "::1");
    doParseHost("[::1]?",           "::1");
    doParseHost("[::1]#",           "::1");
    doParseHost("[::1]:",           "::1");
    doParseHost("[fe80:dead:beef]", "fe80:dead:beef");
  }

  void
  doParsePort(string_view in, string_view port) {
    buffer out;
    error_code ec;

    auto ret = p.parse_port(in.begin(), in.end(), out, ec);

    BEAST_EXPECT(!ec);
    BEAST_EXPECT(ret != in.begin());
    BEAST_EXPECT(out.port() == port);
  }

  void
  testParsePort() {
    doParsePort(":2",    "2");
    doParsePort(":2/",   "2");
    doParsePort(":2?",   "2");
    doParsePort(":2#",   "2");
    doParsePort(":9003", "9003");
  }

  void doParseAuthority(string_view in,
      string_view username = "",
      string_view password = "",
      string_view host = "",
      string_view port = "") {
    buffer out;
    error_code ec;

    auto ret = p.parse_authority(in.begin(), in.end(), out, ec);

    BEAST_EXPECT(!ec);
    BEAST_EXPECT(ret != in.begin());
    BEAST_EXPECT(out.username() == username);
    BEAST_EXPECT(out.password() == password);
    BEAST_EXPECT(out.host() == host);
    BEAST_EXPECT(out.port() == port);
  }

  void
  testParseAuthority() {
    doParseAuthority("//1.1.1.1",        "", "", "1.1.1.1");
    doParseAuthority("//a@1",            "a", "", "1");
    doParseAuthority("//a:b@1",          "a", "b", "1");
    doParseAuthority("//a@1.1.1.1",      "a", "", "1.1.1.1");
    doParseAuthority("//aa@1.1.1.1",     "aa", "", "1.1.1.1");
    doParseAuthority("//a:b@1.1.1.1",    "a", "b", "1.1.1.1");
    doParseAuthority("//aa:bb@1.1.1.1",  "aa", "bb", "1.1.1.1");
    doParseAuthority("//a:b@1.1.1.1:80", "a", "b", "1.1.1.1", "80");
    doParseAuthority("//a:b@[::1]:80",   "a", "b", "::1", "80");
    doParseAuthority("//a:b@ab.\%a2:80", "a", "b", "ab.\%a2", "80");
  }

  void
  doParsePath(string_view in, string_view path) {
    buffer out;
    error_code ec;

    auto ret = p.parse_path(in.begin(), in.end(), out, ec);

    BEAST_EXPECT(!ec);
    BEAST_EXPECT(ret != in.begin());
    BEAST_EXPECT(out.path() == path);
  }

  void
  testParsePath() {
    doParsePath("/",   "/");
    doParsePath("/a",  "/a");
    doParsePath("/9",  "/9");
    doParsePath("/~",  "/~");
    doParsePath("/a=", "/a=");
    doParsePath("/?",  "/");
    doParsePath("/#",  "/");
  }

  void
  doParseQuery(string_view in, string_view query) {
    buffer out;
    error_code ec;

    auto ret = p.parse_query(in.begin(), in.end(), out, ec);

    BEAST_EXPECT(!ec);
    BEAST_EXPECT(ret != in.begin());
    BEAST_EXPECT(out.query() == query);
  }

  void
  testParseQuery() {
    doParseQuery("?",    "");
    doParseQuery("?a",   "a");
    doParseQuery("?9",   "9");
    doParseQuery("?~",   "~");
    doParseQuery("?a=b", "a=b");
    doParseQuery("?/",   "/");
    doParseQuery("??",   "?");
    doParseQuery("?#",   "");
  }

  void
  doParseFragment(string_view in, string_view fragment) {
    buffer out;
    error_code ec;

    auto ret = p.parse_fragment(in.begin(), in.end(), out, ec);

    BEAST_EXPECT(!ec);
    BEAST_EXPECT(ret != in.begin());
    BEAST_EXPECT(out.fragment() == fragment);
  }

  void
  testParseFragment() {
    doParseFragment("#",    "");
    doParseFragment("#a",   "a");
    doParseFragment("#9",   "9");
    doParseFragment("#~",   "~");
    doParseFragment("#a=b", "a=b");
    doParseFragment("#/",   "/");
    doParseFragment("#?",   "?");
  }

  void
  run() override
  {
    testParseScheme();
    testParseUsername();
    testParsePassword();
    testParseHost();
    testParsePort();
    testParseAuthority();
    testParsePath();
    testParseQuery();
    testParseFragment();
  }
};

BEAST_DEFINE_TESTSUITE(beast,core,uri_parser_detail);

}  // uri
}  // beast
}  // boost
