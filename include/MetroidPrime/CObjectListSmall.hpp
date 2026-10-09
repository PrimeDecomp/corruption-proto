#ifndef _COBJECTLISTSMALL
#define _COBJECTLISTSMALL

#include "types.h"

#include "MetroidPrime/TGameTypes.hpp"

#include "rstl/construct.hpp"
#include "rstl/rmemory_allocator.hpp"

class CEntity;

// Guessed name. A list of fixed-size blocks: each block holds up to N values, and the blocks are
// linked in a ring through a sentinel header kept in the list itself (the end node, whose count
// stays 0). An iterator is a block and an index into it. Its functions are weak instances:
// CObjectListSmall.cpp emits the erase, find and insert paths, CStateManager.cpp (0x80296E24)
// the copy constructor.
//
// Insertion puts a value in the first block with room. A block that empties is unlinked and
// freed. Order is not kept: when inserting into a full block in the middle, its last value moves
// into a new block linked before it.
template < typename T, int N, typename Alloc = rstl::rmemory_allocator >
class TBlockList {
public:
  struct node {
    node* mPrev;
    node* mNext;
    int mCount;
    uchar mData[N * sizeof(T)];

    T* data() { return reinterpret_cast< T* >(mData); }
    const T* data() const { return reinterpret_cast< const T* >(mData); }

    // Weak (0x802623C4): opens a slot at idx.
    void insert(int idx, T val);
    void erase(int idx) {
      for (int i = idx; i < mCount - 1; ++i) {
        data()[i] = data()[i + 1];
      }
      --mCount;
    }
    void destroy() {
      for (int i = 0; i < mCount; ++i) {
        rstl::destroy(&data()[i]);
      }
      mCount = 0;
    }
  };

  class const_iterator {
  public:
    const_iterator(const node* n, int idx) : mNode(const_cast< node* >(n)), mIndex(idx) {}

    const T& operator*() const { return mNode->data()[mIndex]; }
    const_iterator& operator++() {
      if (mIndex == mNode->mCount - 1) {
        mNode = mNode->mNext;
        mIndex = 0;
      } else {
        ++mIndex;
      }
      return *this;
    }
    bool operator==(const const_iterator& other) const {
      return mNode == other.mNode && mIndex == other.mIndex;
    }
    bool operator!=(const const_iterator& other) const {
      return mNode != other.mNode || mIndex != other.mIndex;
    }

    node* mNode;
    int mIndex;
  };
  class iterator : public const_iterator {
  public:
    iterator(node* n, int idx) : const_iterator(n, idx) {}

    T& operator*() const { return const_cast< T& >(const_iterator::operator*()); }
    iterator& operator++() {
      const_iterator::operator++();
      return *this;
    }
  };

  TBlockList()
  : mStart(sentinel())
  , mEnd(sentinel())
  , mSentinelPrev(sentinel())
  , mSentinelNext(sentinel())
  , mSentinelCount(0)
  , mSize(0) {}
  // Weak (0x80296E24). Every value is inserted at the end position, which never moves, so each
  // one gets a block of its own.
  TBlockList(const TBlockList& other)
  : mAllocator(other.mAllocator)
  , mStart(sentinel())
  , mEnd(sentinel())
  , mSentinelPrev(sentinel())
  , mSentinelNext(sentinel())
  , mSentinelCount(0)
  , mSize(0) {
    insert(end(), other.begin(), other.end());
  }
  ~TBlockList() {
    node* n = mStart;
    while (n != mEnd) {
      node* cur = n;
      n = n->mNext;
      cur->destroy();
      mAllocator.deallocate(cur);
    }
  }

  iterator begin() { return iterator(mStart, 0); }
  iterator end() { return iterator(mEnd, 0); }
  const_iterator begin() const { return const_iterator(mStart, 0); }
  const_iterator end() const { return const_iterator(mEnd, 0); }

  // Weak (0x8026235C): into the first block with room, else a new last block. Filling a block
  // that is not full does not count the value in the size.
  void insert(const T& val);
  // Weak (0x80262470).
  void push_back(T val);
  // Weak (0x80296F54).
  iterator insert(const iterator& pos, T val);
  // Weak (0x80296EB4).
  template < typename It >
  void insert(const iterator& pos, It first, It last);
  // Weak (0x8026202C).
  iterator erase(const iterator& it);

private:
  node* sentinel() { return reinterpret_cast< node* >(&mSentinelPrev); }
  node* create_node(node* prev, node* next, T val) {
    node* n;
    mAllocator.allocate(n, 1);
    n->mPrev = prev;
    n->mNext = next;
    n->mCount = 1;
    n->data()[0] = val;
    return n;
  }
  iterator do_insert(node* n, int idx, T val); // Weak (0x802624A8)
  iterator do_erase(node* n, int idx);         // Weak (0x80262064)

  Alloc mAllocator;
  node* mStart;
  node* mEnd;
  // The sentinel's header: its prev, next and count.
  node* mSentinelPrev;
  node* mSentinelNext;
  int mSentinelCount;
  int mSize;
};

template < typename T, int N, typename Alloc >
void TBlockList< T, N, Alloc >::node::insert(int idx, T val) {
  T* pos = data() + idx;
  for (int i = mCount - idx - 1; i >= 0; --i) {
    pos[i + 1] = pos[i];
  }
  *pos = val;
  ++mCount;
}

template < typename T, int N, typename Alloc >
void TBlockList< T, N, Alloc >::insert(const T& val) {
  node* n = mStart;
  while (n->mCount == N && n != mEnd) {
    n = n->mNext;
  }
  if (n == mEnd) {
    push_back(val);
  } else {
    n->insert(n->mCount, val);
  }
}

template < typename T, int N, typename Alloc >
void TBlockList< T, N, Alloc >::push_back(T val) {
  do_insert(mEnd->mPrev, mEnd->mPrev->mCount, val);
}

template < typename T, int N, typename Alloc >
typename TBlockList< T, N, Alloc >::iterator TBlockList< T, N, Alloc >::do_insert(node* n, int idx,
                                                                                  T val) {
  ++mSize;
  if (n != mEnd && n->mCount < N) {
    n->insert(idx, val);
    return iterator(n, idx);
  }
  if (idx == 0) {
    node* nn = create_node(n->mPrev, n, val);
    if (n == mStart) {
      mStart = nn;
    }
    nn->mPrev->mNext = nn;
    nn->mNext->mPrev = nn;
    return iterator(nn, 0);
  }
  if (idx == N) {
    node* nn = create_node(n, n->mNext, val);
    nn->mNext->mPrev = nn;
    nn->mPrev->mNext = nn;
    return iterator(nn, 0);
  }
  node* nn = create_node(n->mPrev, n, n->data()[n->mCount - 1]);
  n->erase(n->mCount - 1);
  n->insert(idx, val);
  nn->mPrev->mNext = nn;
  nn->mNext->mPrev = nn;
  return iterator(n, idx);
}

template < typename T, int N, typename Alloc >
typename TBlockList< T, N, Alloc >::iterator TBlockList< T, N, Alloc >::insert(const iterator& pos,
                                                                               T val) {
  return do_insert(pos.mNode, pos.mIndex, val);
}

template < typename T, int N, typename Alloc >
template < typename It >
void TBlockList< T, N, Alloc >::insert(const iterator& pos, It first, It last) {
  for (It it = first; it != last; ++it) {
    insert(pos, *it);
  }
}

template < typename T, int N, typename Alloc >
typename TBlockList< T, N, Alloc >::iterator TBlockList< T, N, Alloc >::erase(const iterator& it) {
  return do_erase(it.mNode, it.mIndex);
}

template < typename T, int N, typename Alloc >
typename TBlockList< T, N, Alloc >::iterator TBlockList< T, N, Alloc >::do_erase(node* n, int idx) {
  --mSize;
  n->erase(idx);
  if (n->mCount == 0) {
    node* next = n->mNext;
    if (n == mStart) {
      mStart = next;
    }
    n->mPrev->mNext = n->mNext;
    n->mNext->mPrev = n->mPrev;
    mAllocator.deallocate(n);
    return iterator(next, 0);
  }
  if (idx == n->mCount) {
    return iterator(n->mNext, 0);
  }
  return iterator(n, idx);
}

// Class name from CObjectListSmall.cpp's "CObjectListSmall.cpp(41) : " assert. This replaces
// Echoes' CFilteredObjectList: CStateManagerObject owns five of them (0x24 bytes each). Each
// subclass overrides only IsQualified. The base constructor (0x8026285C) sets up an empty list
// of 0x4C-byte blocks of 16 entity pointers, and the destructor (0x8026277C) frees those blocks.
class CObjectListSmall {
public:
  typedef TBlockList< CEntity*, 16 > TList;

  // The five subclasses pass false; what the flag means is not known.
  explicit CObjectListSmall(bool flag);
  virtual ~CObjectListSmall();
  virtual bool IsQualified(const CEntity& entity) const; // The base one returns true.

  // Echoes' CFilteredObjectList names.
  void RemoveObject(TUniqueId uid);           // 0x80261F60
  void RemoveObject(CEntity& entity);         // 0x8026214C
  void AddObject(CEntity& entity);            // 0x80262294
  bool Contains(const CEntity& entity) const; // 0x8026265C

  // Guessed names. CStateManager's destructor walks a copy of the camera list.
  TList::const_iterator begin() const { return mList.begin(); }
  TList::const_iterator end() const { return mList.end(); }

private:
  TList mList;
  bool x20_;
};
CHECK_SIZEOF(CObjectListSmall, 0x24)

#endif // _COBJECTLISTSMALL
