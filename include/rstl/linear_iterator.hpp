#ifndef RSTL_LINEAR_ITERATOR_HPP
#define RSTL_LINEAR_ITERATOR_HPP

namespace rstl {

template < typename Iterator >
inline int distance(Iterator first, Iterator last) {
  return last - first;
}

template < typename T, typename Container, typename Alloc >
class const_linear_iterator {
public:
  const_linear_iterator(const Container* owner, int index) : mOwner(owner), mIndex(index) {}

  const T& operator*() const { return (*mOwner)[mIndex]; }
  const_linear_iterator& operator++() { ++mIndex; return *this; }
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
