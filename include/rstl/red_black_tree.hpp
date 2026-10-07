#ifndef RSTL_RED_BLACK_TREE_HPP
#define RSTL_RED_BLACK_TREE_HPP

namespace rstl {

enum node_color {
  kNC_Black,
  kNC_Red,
};

void rbtree_rotate_left(void* header, void* node);
void rbtree_rotate_right(void* header, void* node);
void rbtree_rebalance(void* header, void* node);
void* rbtree_rebalance_for_erase(void* header, void* node);
void* rbtree_traverse_forward(const void* header, void* node);

} // namespace rstl

#endif
