#ifndef RSTL_LINEAR_ITERATOR_HPP
#define RSTL_LINEAR_ITERATOR_HPP

#include "rstl/iterator.hpp"

namespace rstl {

template < typename T, typename Container, typename Alloc >
class const_linear_iterator {
public:
  typedef T value_type;
  typedef long difference_type;
  typedef random_access_iterator_tag iterator_category;

  const_linear_iterator(const Container* owner, int index) : mOwner(owner), mIndex(index) {}

  const T& operator*() const { return (*mOwner)[mIndex]; }
  const_linear_iterator& operator++() {
    ++mIndex;
    return *this;
  }
  const_linear_iterator& operator+=(int count) {
    mIndex += count;
    return *this;
  }
  const_linear_iterator& operator-=(int count) {
    mIndex -= count;
    return *this;
  }
  const_linear_iterator operator-(int count) const {
    return const_linear_iterator(mOwner, mIndex - count);
  }
  const_linear_iterator operator+(int count) const {
    return const_linear_iterator(mOwner, mIndex + count);
  }
  long operator-(const const_linear_iterator& other) const { return mIndex - other.mIndex; }
  bool operator==(const const_linear_iterator& other) const {
    return mOwner == other.mOwner && mIndex == other.mIndex;
  }
  bool operator!=(const const_linear_iterator& other) const { return !(*this == other); }

private:
  const Container* mOwner;
  int mIndex;
};

} // namespace rstl

#endif
