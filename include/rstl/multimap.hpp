#ifndef _RSTL_MULTIMAP
#define _RSTL_MULTIMAP

#include "types.h"

#include "rstl/pair.hpp"
#include "rstl/red_black_tree.hpp"
#include "rstl/rmemory_allocator.hpp"

namespace rstl {
template < typename K, typename V, typename Cmp = less< K >, typename Alloc = rmemory_allocator >
class multimap
: public red_black_tree< K, pair< K, V >, 1, select1st< pair< K, V > >, Cmp, Alloc > {
public:
  typedef pair< K, V > value_type;

private:
  typedef red_black_tree< K, value_type, 1, select1st< value_type >, Cmp, Alloc > rep_type;

public:
  typedef typename rep_type::iterator iterator;
  typedef typename rep_type::const_iterator const_iterator;

  explicit multimap(const Cmp& cmp = Cmp(), const Alloc& alloc = Alloc())
  : rep_type(select1st< value_type >(), cmp, alloc) {}
  ~multimap() {}
};
} // namespace rstl

#endif // _RSTL_MULTIMAP
