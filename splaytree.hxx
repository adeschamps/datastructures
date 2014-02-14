#ifndef AD_SPLAYTREE_HXX
#define AD_SPLAYTREE_HXX

#include <cstddef>
#include <iterator>
#include <utility>

namespace ad
{
  template <typename T>
  class splaytree
  {
  private:
    struct node;

  public:
    class iterator;

    ~ splaytree ();

  private:
    node * root = nullptr;
    size_t num_nodes = 0;

    void destroy_tree (node * n);
    void splay (node * n);
    void rotate_left  (node * n);
    void rotate_right (node * n);

    void delete_node (node * n);

  public:
    iterator begin ();
    iterator end ();

    iterator insert (T const & value);
    iterator insert (T && value);
  private:
    iterator insert (node * n);

  public:
    iterator find (T const & value);

    // NOTE: Not yet implemented
    void   erase (iterator iter);
    void   erase (iterator begin, iterator end);
    size_t erase (T const & value);
  };

  template <typename T>
  struct splaytree<T>::node
  {
    T value;
    node * parent = nullptr;
    node * left   = nullptr;
    node * right  = nullptr;
  };

  template <typename T>
  splaytree<T>::~splaytree ()
  { destroy_tree (root); }

  template <typename T>
  void splaytree<T>::destroy_tree (node * n)
  {
    if (n == nullptr) return;
    destroy_tree (n -> left);
    destroy_tree (n -> right);

    delete n;
  }

  template <typename T>
  void splaytree<T>::splay (node * n)
  {
    // Trivial case
    if (n == root) return;

    // Zig step; base case
    node * p = n -> parent;
    if (p == root)
      {
	if (n == n -> parent -> left)
	  rotate_right (n -> parent);
	else
	  rotate_left  (n -> parent);
	return;
      }

    // Recursive cases
    node * g = p -> parent;
    if (n == p -> left)
      {
	if (p == g -> left) // zig-zig
	  {
	    rotate_right (g);
	    rotate_right (p);
	  }
	else // zig-zag
	  {
	    rotate_right (p);
	    rotate_left  (g);
	  }
      }
    else
      {
	if (p == g -> right) // zig-zig
	  {
	    rotate_left (g);
	    rotate_left (p);
	  }
	else // zig-zag
	  {
	    rotate_left  (p);
	    rotate_right (g);
	  }
      }

    splay (n);
  }

  template <typename T>
  void splaytree<T>::rotate_left (node * n)
  {
    node * c = n -> right;

    n -> right = c -> left;
    c -> left = n;
    c -> parent = n -> parent;
    n -> parent = c;
    if (n -> right) n -> right -> parent = n;
    if (c -> parent)
      {
	if (c -> parent -> left == n)
	  c -> parent -> left = c;
	else
	  c -> parent -> right = c;
      }
    else
      root = c;
  }

  template <typename T>
  void splaytree<T>::rotate_right (node * n)
  {
    node * c = n -> left;

    n -> left = c -> right;
    c -> right = n;
    c -> parent = n -> parent;
    n -> parent = c;
    if (n -> left) n -> left -> parent = n;
    if (c -> parent)
      {
	if (c -> parent -> left == n)
	  c -> parent -> left = c;
	else
	  c -> parent -> right = c;
      }
    else
      root = c;
  }

  template <typename T>
  void splaytree<T>::delete_node (node * n)
  {
    // n is a leaf
    if (n -> left == nullptr && n -> right == nullptr)
      if (n -> parent)
	{
	  if (n == n -> parent -> left)
	    n -> parent -> left = nullptr;
	  else
	    n -> parent -> right = nullptr;
	  delete n;
	  -- num_nodes;
	  return;
	}

    // n has two children
    if (n -> left && n -> right)
      {
	node * loc = n -> left;
	while (loc -> right != nullptr) loc = loc -> right;
	std::swap (n -> value, loc -> value);
	n = loc;
      }

    // n has one child
    // ... on the left
    if (n -> left)
      {
	if (n -> parent)
	  {
	    if (n == n -> parent -> left)
	      n -> parent -> left = n -> left;
	    else
	      n -> parent -> right = n -> left;
	    n -> left -> parent = n -> parent;
	  }
	else
	  {
	    n -> left -> parent = nullptr;
	    root = n -> left;
	  }
      }
    // ... on the right
    else
      {
	if (n -> parent)
	  {
	    if (n == n -> parent -> left)
	      n -> parent -> left = n -> right;
	    else
	      n -> parent -> right = n -> right;
	    n -> right -> parent = n -> parent;
	  }
	else
	  {
	    n -> right -> parent = nullptr;
	    root = n -> right;
	  }
      }

    delete n;
    -- num_nodes;
  }

  template <typename T>
  typename splaytree<T>::iterator splaytree<T>::begin ()
  {
    if (root == nullptr) return iterator (nullptr);
    node * n = root;
    while (n -> left) n = n -> left;
    return iterator (n);
  }

  template <typename T>
  typename splaytree<T>::iterator splaytree<T>::end ()
  {
    return iterator (nullptr);
  }

  template <typename T>
  typename splaytree<T>::iterator splaytree<T>::insert (T const & value)
  {
    node * n = new node;
    ++ num_nodes;
    n -> value = value;
    return insert (n);
  }

  template <typename T>
  typename splaytree<T>::iterator splaytree<T>::insert (T && value)
  {
    node * n = new node;
    ++ num_nodes;
    n -> value = std::move(value);
    return insert (n);
  }

  template <typename T>
  typename splaytree<T>::iterator splaytree<T>::insert (node * n)
  {
    if (root == nullptr) { root = n ; return end(); }

    node * loc = root;
    while (true)
      {
	if (n -> value < loc -> value)
	  {
	    if (loc -> left) loc = loc -> left;
	    else
	      {
		loc -> left = n;
		n -> parent = loc;
		splay (n);
		return iterator (n);
	      }
	  }
	else if (loc -> value < n -> value)
	  {
	    if (loc -> right) loc = loc -> right;
	    else
	      {
		loc -> right = n;
		n -> parent = loc;
		splay (n);
		return iterator (n);
	      }
	  }
	else
	  {
	    delete n;
	    return end ();
	  }
      }
  }

  template <typename T>
  typename splaytree<T>::iterator splaytree<T>::find (T const & value)
  {
    node * n = root;
    while (n != nullptr)
      {
	if (value < n -> value) n = n -> left;
	else if (n -> value < value) n = n -> right;
	else
	  {
	    splay (n);
	    return iterator (n);
	  }
      }
    return iterator (nullptr);
  }

  // NOTE: Not yet implemented
  template <typename T>
  void splaytree<T>::erase (iterator iter)
  {}

  // template <typename T>
  // void splaytree<T>::erase (iterator begin, iterator end)
  // {}

  template <typename T>
  size_t splaytree<T>::erase (T const & value)
  {
    iterator iter = find (value);
    if (iter != end())
      {
	erase (iter);
	return 1;
      }
    else
      return 0;
  }

  template <typename T>
  class splaytree<T>::iterator : public std::iterator<std::forward_iterator_tag, T>
  {
    node * n;

  public:

    iterator (node * n_) : n (n_) {}
    iterator (iterator const & iter) : n (iter.n) {}

    T & operator * ()
    { return n -> value; }

    iterator & operator ++ ();

    iterator operator ++ (int)
    { iterator tmp (*this); ++ (*this); return tmp; }

    iterator & operator -- ();

    iterator operator -- (int)
    { iterator tmp (*this); -- (*this); return tmp; }

    bool operator == (iterator const & iter)
    { return n == iter.n; }

    bool operator != (iterator const & iter)
    { return n != iter.n; }
  }; // class iterator

  template <typename T>
  typename splaytree<T>::iterator & splaytree<T>::iterator::operator ++ ()
  {
    int side = 0;
    do
      {
	switch (side)
	  {
	  case -1: // left
	    if (n -> left)
	      n = n -> left;
	    else
	      side = 0;
	    break;

	  case 0: // bottom
	    if (n -> right)
	      {
		n = n -> right;
		side = -1;
	      }
	    else
	      side = 1;
	    break;

	  case 1: // right
	    if (n -> parent == nullptr || n == n -> parent -> left)
	      side = 0;
	    n = n -> parent;
	    break;
	  }
      }
    while (side != 0);
    return *this;
  }

  template <typename T>
  typename splaytree<T>::iterator & splaytree<T>::iterator::operator -- ()
  {
    int side = 0;
    do
      {
	switch (side)
	  {
	  case -1: // left
	    if (n -> parent == nullptr || n == n -> parent -> right)
	      side = 0;
	    n = n -> parent;
	    break;

	  case 0: // bottom
	    if (n -> left)
	      {
		n = n -> left;
		side = 1;
	      }
	    else
	      side = -1;
	    break;

	  case 1: // right
	    if (n -> right)
	      n = n -> right;
	    else
	      side = 0;
	    break;
	  }
      }
    while (side != 0);
    return *this;
  }

} // namespace ad

#endif
