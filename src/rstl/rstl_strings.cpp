#include "rstl/string.hpp"
#include "Kyoto/Alloc/CMemory.hpp"

#include "Kyoto/Streams/CInputStream.hpp"
#include "Kyoto/Streams/COutputStream.hpp"

namespace rstl {

template <>
char basic_string< char >::mNull;

template <>
wchar_t basic_string< wchar_t >::mNull;

template <>
char basic_string< char, case_insensitive_char_traits >::mNull;

template <>
void basic_string< char, case_insensitive_char_traits >::internal_dereference() {
  if (mCow && --mCow->mRefCount == 0) {
    CMemory::Free(mCow);
  }
}

// Keep the constructor's allocation call in this TU's helper graph.
#pragma dont_inline on
template <>
void basic_string< char, case_insensitive_char_traits >::internal_allocate(int size) {
  mCow = reinterpret_cast< control* >(rmemory_allocator::allocate(sizeof(control) + size));
  mPtr = reinterpret_cast< char* >(mCow + 1);
  mCow->mCapacity = size;
  mCow->mRefCount = 1;
}
#pragma dont_inline reset

static pair< const char*, int > compute_case_insensitive_length(const char* data, int count) {
  int length = 0;
  while (count == -1 || length < count) {
    unsigned char value = static_cast< unsigned char >(data[length]);
    if (value >= 'a' && value <= 'z') {
      value -= 'a' - 'A';
    } else if (value >= 0xe0 && value <= 0xfe) {
      value -= 0x20;
    } else if (value >= 0xa0 && value <= 0xff) {
      value -= 0x60;
    }
    if (value == 0) {
      break;
    }
    ++length;
  }
  return pair< const char*, int >(data + length, length);
}

template <>
basic_string< char, case_insensitive_char_traits >::basic_string(const char* data, int count,
                                                                 const rmemory_allocator& alloc)
: mAllocator(alloc) {
  if (count <= 0 && !*data) {
    mPtr = &mNull;
    mSize = 0;
    mCow = 0;
    return;
  }
  const pair< const char*, int > range = compute_case_insensitive_length(data, count);
  const int length = range.second;
  internal_allocate(length + 1);
  mSize = length;
  char_traits< char >::copy(const_cast< char* >(mPtr), data, length);
  const_cast< char& >(mPtr[length]) = 0;
}

template <>
basic_string< char >::basic_string(CInputStream& in, const rmemory_allocator& alloc)
: mPtr(&mNull), mCow(0), mSize(0), mAllocator(alloc) {
  char buffer[1025];
  int count = 0;
  char ch = in.ReadInt8();
  while (ch != '\0') {
    buffer[count++] = ch;
    if (count == 1024) {
      buffer[1024] = '\0';
      append(buffer, -1);
      count = 0;
    }
    ch = in.ReadInt8();
  }
  if (count != 0) {
    buffer[count] = '\0';
    append(buffer, -1);
  }
}

template <>
basic_string< char >::basic_string(const char* data, int count, const rmemory_allocator& alloc)
: mAllocator(alloc) {
  if (count <= 0 && !*data) {
    mPtr = &mNull;
    mSize = 0;
    mCow = 0;
    return;
  }

  const pair< const char*, int > range = compute_length(data, count);
  const int len = range.second;
  internal_allocate(len + 1);
  mSize = len;
  char_traits< char >::copy(const_cast< char* >(mPtr), data, len);
  char_traits< char >::assign(const_cast< char& >(mPtr[len]), char_traits< char >::eos());
}

template <>
basic_string< char >::basic_string(const basic_string& other)
: mPtr(other.mPtr), mCow(other.mCow), mSize(other.mSize), mAllocator(other.mAllocator) {
  internal_reference();
}

template <>
basic_string< char >::basic_string(char value) {
  internal_allocate(2);
  const_cast< char& >(mPtr[0]) = value;
  const_cast< char& >(mPtr[1]) = 0;
  mSize = 1;
}

template <>
basic_string< char >::basic_string(literal_t, const char* data) {
  mPtr = data;
  mCow = 0;
  const char* end = data;
  while (*end) {
    ++end;
  }
  mSize = end - data;
}

template <>
void basic_string< char >::clear() {
  internal_dereference();
  mPtr = &mNull;
  mCow = 0;
  mSize = 0;
}

template <>
basic_string< char >& basic_string< char >::erase(int pos, int count) {
  if (pos > length()) {
    pos = length();
  }
  if (count == -1 || count > length() - pos) {
    count = length() - pos;
  }
  internal_prepare_to_write(length(), true);
  char* data = const_cast< char* >(mPtr);
  for (int i = pos; i <= length() - count; ++i) {
    data[i] = data[i + count];
  }
  mSize -= count;
  return *this;
}

template <>
basic_string< char >& basic_string< char >::append(const basic_string& other) {
  internal_prepare_to_write(length() + other.length(), true);
  char_traits< char >::copy(const_cast< char* >(mPtr) + length(), other.data(), other.length());
  mSize += other.length();
  char_traits< char >::assign(const_cast< char& >(mPtr[length()]), char_traits< char >::eos());
  return *this;
}

template <>
basic_string< char >& basic_string< char >::append(const char* data, int count) {
  const pair< const char*, int > range = compute_length(data, count);
  const int len = range.second;
  internal_prepare_to_write(mSize + len, true);
  char_traits< char >::copy(const_cast< char* >(mPtr) + length(), data, len);
  mSize += len;
  char_traits< char >::assign(const_cast< char& >(mPtr[length()]), char_traits< char >::eos());
  return *this;
}

template <>
basic_string< char >& basic_string< char >::append(int count, char value) {
  internal_prepare_to_write(mSize + count, true);
  char_traits< char >::assign(const_cast< char* >(mPtr) + length(), count, value);
  mSize += count;
  char_traits< char >::assign(const_cast< char& >(mPtr[length()]), char_traits< char >::eos());
  return *this;
}

template <>
basic_string< char >& basic_string< char >::assign(const basic_string& other) {
  if (mCow && mCow == other.mCow) {
    return *this;
  }

  internal_dereference();
  mCow = other.mCow;
  mPtr = other.mPtr;
  mSize = other.mSize;
  internal_reference();
  return *this;
}

template <>
basic_string< char >& basic_string< char >::assign(const char* data, int count) {
  const pair< const char*, int > range = compute_length(data, count);
  const int len = range.second;
  internal_prepare_to_write(len, false);
  char_traits< char >::copy(const_cast< char* >(mPtr), data, len);
  mSize = len;
  char_traits< char >::assign(const_cast< char& >(mPtr[length()]), char_traits< char >::eos());
  return *this;
}

template <>
void basic_string< char >::PutTo(COutputStream& out) const {
  for (int i = 0; i < length() + 1; ++i) {
    out.WriteChar(mPtr[i]);
  }
}

template <>
pair< basic_string< char >::const_iterator, basic_string< char >::const_iterator >
basic_string< char >::range_iterator(int pos, int count) const {
  const_iterator first = position_iterator(pos);
  const_iterator last = count != -1 && pos + count < length() ? first + count : end();
  return pair< const_iterator, const_iterator >(first, last);
}

template <>
basic_string< char >::const_iterator basic_string< char >::position_iterator(int pos) const {
  if (pos == -1 || pos >= length()) {
    return end();
  }

  return begin() + pos;
}

template <>
void basic_string< char >::internal_allocate(int size) {
  rmemory_allocator::allocate(reinterpret_cast< uchar*& >(mCow),
                              sizeof(control) + sizeof(char) * size);
  mPtr = reinterpret_cast< char* >(mCow + 1);
  mCow->mCapacity = size;
  mCow->mRefCount = 1;
}

template <>
void basic_string< char >::internal_dereference() {
  if (mCow && --mCow->mRefCount == 0) {
    CMemory::Free(mCow);
  }
}

template <>
void basic_string< char >::internal_prepare_to_write(int len, bool preserve) {
  const int required = len + 1;
  if (mCow == 0 || mCow->mRefCount != 1 || mCow->mCapacity < required) {
    int capacity;
    if (mCow) {
      capacity = mCow->mCapacity < 4 ? 4 : mCow->mCapacity;
      while (capacity < required) {
        capacity *= 2;
      }
    } else {
      capacity = required;
    }

    uchar* allocation;
    rmemory_allocator::allocate(allocation, sizeof(control) + sizeof(char) * capacity);
    control* newControl = reinterpret_cast< control* >(allocation);
    newControl->mCapacity = capacity;
    newControl->mRefCount = 1;
    char* const newData = reinterpret_cast< char* >(newControl + 1);
    if (preserve) {
      char_traits< char >::copy(newData, mPtr, length());
      char_traits< char >::assign(newData[length()], char_traits< char >::eos());
    }
    internal_dereference();
    mCow = newControl;
    mPtr = newData;
  }
}

template <>
basic_string< wchar_t >::basic_string(const wchar_t* data, int count,
                                      const rmemory_allocator& alloc)
: mAllocator(alloc) {
  if (count <= 0 && !*data) {
    mPtr = &mNull;
    mSize = 0;
    mCow = 0;
    return;
  }

  const pair< const wchar_t*, int > range = compute_length(data, count);
  const int len = range.second;
  internal_allocate(len + 1);
  mSize = len;
  char_traits< wchar_t >::copy(const_cast< wchar_t* >(mPtr), data, len);
  char_traits< wchar_t >::assign(const_cast< wchar_t& >(mPtr[len]), char_traits< wchar_t >::eos());
}

template <>
basic_string< wchar_t >::basic_string(const basic_string& other)
: mPtr(other.mPtr), mCow(other.mCow), mSize(other.mSize), mAllocator(other.mAllocator) {
  internal_reference();
}

template <>
basic_string< wchar_t >& basic_string< wchar_t >::append(const basic_string& other) {
  internal_prepare_to_write(length() + other.length(), true);
  char_traits< wchar_t >::copy(const_cast< wchar_t* >(mPtr) + length(), other.data(),
                               other.length());
  mSize += other.length();
  char_traits< wchar_t >::assign(const_cast< wchar_t& >(mPtr[length()]),
                                 char_traits< wchar_t >::eos());
  return *this;
}

template <>
basic_string< wchar_t >& basic_string< wchar_t >::append(const wchar_t* data, int count) {
  const pair< const wchar_t*, int > range = compute_length(data, count);
  const int len = range.second;
  internal_prepare_to_write(mSize + len, true);
  char_traits< wchar_t >::copy(const_cast< wchar_t* >(mPtr) + length(), data, len);
  mSize += len;
  char_traits< wchar_t >::assign(const_cast< wchar_t& >(mPtr[length()]),
                                 char_traits< wchar_t >::eos());
  return *this;
}

template <>
basic_string< wchar_t >& basic_string< wchar_t >::append(int count, wchar_t value) {
  internal_prepare_to_write(mSize + count, true);
  char_traits< wchar_t >::assign(const_cast< wchar_t* >(mPtr) + length(), count, value);
  mSize += count;
  char_traits< wchar_t >::assign(const_cast< wchar_t& >(mPtr[length()]),
                                 char_traits< wchar_t >::eos());
  return *this;
}

template <>
basic_string< wchar_t >& basic_string< wchar_t >::assign(const basic_string& other) {
  if (mCow && mCow == other.mCow) {
    return *this;
  }

  internal_dereference();
  mCow = other.mCow;
  mPtr = other.mPtr;
  mSize = other.mSize;
  internal_reference();
  return *this;
}

template <>
basic_string< wchar_t >& basic_string< wchar_t >::assign(const wchar_t* data, int count) {
  const pair< const wchar_t*, int > range = compute_length(data, count);
  const int len = range.second;
  internal_prepare_to_write(len, false);
  char_traits< wchar_t >::copy(const_cast< wchar_t* >(mPtr), data, len);
  mSize = len;
  char_traits< wchar_t >::assign(const_cast< wchar_t& >(mPtr[length()]),
                                 char_traits< wchar_t >::eos());
  return *this;
}

template <>
void basic_string< wchar_t >::internal_allocate(int size) {
  rmemory_allocator::allocate(reinterpret_cast< uchar*& >(mCow),
                              sizeof(control) + sizeof(wchar_t) * size);
  mPtr = reinterpret_cast< wchar_t* >(mCow + 1);
  mCow->mCapacity = size;
  mCow->mRefCount = 1;
}

template <>
void basic_string< wchar_t >::internal_dereference() {
  if (mCow && --mCow->mRefCount == 0) {
    CMemory::Free(mCow);
  }
}

template <>
void basic_string< wchar_t >::internal_prepare_to_write(int len, bool preserve) {
  const int required = len + 1;
  if (mCow == 0 || mCow->mRefCount != 1 || mCow->mCapacity < required) {
    int capacity;
    if (mCow) {
      capacity = mCow->mCapacity < 4 ? 4 : mCow->mCapacity;
      while (capacity < required) {
        capacity *= 2;
      }
    } else {
      capacity = required;
    }

    uchar* allocation;
    rmemory_allocator::allocate(allocation, sizeof(control) + sizeof(wchar_t) * capacity);
    control* newControl = reinterpret_cast< control* >(allocation);
    newControl->mCapacity = capacity;
    newControl->mRefCount = 1;
    wchar_t* const newData = reinterpret_cast< wchar_t* >(newControl + 1);
    if (preserve) {
      char_traits< wchar_t >::copy(newData, mPtr, length());
      char_traits< wchar_t >::assign(newData[length()], char_traits< wchar_t >::eos());
    }
    internal_dereference();
    mCow = newControl;
    mPtr = newData;
  }
}

wstring wstring_l(const wchar_t* data) { return wstring(wstring::literal_t(), data); }

string string_l(const char* data) { return string(string::literal_t(), data); }

} // namespace rstl
