#ifndef _RSTL_BIT_VECTOR
#define _RSTL_BIT_VECTOR

#include "rstl/vector.hpp"

namespace rstl {
// Ported from Echoes. G2MEAB's at() reads the word storage at +0x10 (mSize, then the vector) and
// builds a word/mask reference; operator[] forwards to it out of line.
template < typename Alloc = rmemory_allocator >
class bit_vector {
  typedef vector< uint, Alloc > storage_type;

public:
  typedef Alloc allocator_type;

  class reference {
  public:
    reference(typename storage_type::iterator word, int bit) : mWord(word), mMask(1 << bit) {}
    operator bool() const { return (mMask & *mWord.get_pointer()) != 0; }
    reference& operator=(bool value) {
      if (value) {
        *mWord |= mMask;
      } else {
        *mWord &= ~mMask;
      }
      return *this;
    }

  private:
    typename storage_type::iterator mWord;
    uint mMask;
  };

  bit_vector() : mSize(0) {}
  bit_vector(int count, bool value);

  int size() const { return mSize; }
  reference at(int bit);
  reference operator[](int bit);

private:
  int get_real_index(int bit) { return bit / 32; }

  int mSize;
  storage_type mData;
};

template < typename Alloc >
typename bit_vector< Alloc >::reference bit_vector< Alloc >::at(int bit) {
  return reference(mData.begin() + get_real_index(bit), bit % 32);
}

template < typename Alloc >
typename bit_vector< Alloc >::reference bit_vector< Alloc >::operator[](int bit) {
  return at(bit);
}
} // namespace rstl

#endif // _RSTL_BIT_VECTOR
