// Copyright (c) 2018 oneiric (oneiric at i2pmail dot org)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

#ifndef BOOST_BEAST_CORE_URI_BUFFER_HPP
#define BOOST_BEAST_CORE_URI_BUFFER_HPP

#include <vector>
#include <boost/beast/core/string.hpp>
#include <boost/beast/experimental/core/uri/error.hpp>
#include <boost/beast/experimental/core/uri/parts.hpp>

namespace boost {
namespace beast {
namespace uri   {

class buffer : public parts {
  using const_iterator = std::vector<char>::const_iterator;
  using value_type = std::vector<char>::value_type;
  using size_type = std::vector<char>::size_type;

  std::vector<char> data_;

public:
  std::vector<char> &data() { return data_; }

  value_type back() const noexcept { return data_.back(); }

  const_iterator begin() const noexcept { return data_.begin(); }

  const_iterator end() const noexcept { return data_.end(); }

  size_type size() const noexcept { return data_.size(); }

  std::string part_from(const_iterator const start, const_iterator const end) {
    BOOST_ASSERT(start >= data_.begin() && start < data_.end());
    BOOST_ASSERT(end > data_.begin() && end <= data_.end());
    BOOST_ASSERT(start < end);

    return {start, end};
  }

  string_view uri() const noexcept { return {data_.data(), data_.size()}; }
};

} // namespace uri
} // namespace beast
} // namespace boost

#endif  // BOOST_BEAST_CORE_URI_BUFFER_HPP
