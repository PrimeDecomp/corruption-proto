#ifndef RSTL_PAIR_HPP
#define RSTL_PAIR_HPP

namespace rstl {

template < typename L, typename R >
struct pair {
  L first;
  R second;

  pair() {}
  pair(const L& left, const R& right) : first(left), second(right) {}
};

} // namespace rstl

#endif
