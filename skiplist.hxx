#ifndef AD_SKIPLIST_HXX
#define AD_SKIPLIST_HXX

#include <cstddef>
#include <iterator>
#include <utility>

#include <iostream>

namespace ad
{
  template <typename T>
  class skiplist
  {
  private:
    struct node;

  public:
    class iterator;

    skiplist ();
    ~ skiplist ();

  private:
    node * anchor;
    size_t num_nodes = 0;

  public:
    iterator begin ();
    iterator end ();

    bool   empty () const;
    size_t size  () const;

    iterator insert (T const & value);
    iterator insert (T && value);
  private:
    iterator insert (node * n);

  public:
    iterator find (T const & value);

  private:
    void delete_node (node * n);

    void   erase (iterator iter);
    void   erase (iterator begin, iterator end);
    size_t erase (T const & value);
  };

  template <typename T>
  struct skiplist<T>::node
  {
    T value;
    size_t links;
    node ** next;
    node ** prev;

    node (T const & value = T());
    node (T && value);
    ~ node ();

    void resize (size_t new_links);
  };

  template <typename T>
  skiplist<T>::node::node (T const & value_)
  {
    value = value_;
    links = 1;
    while (rand() % 2 == 0)
      ++ links;
    next = new node * [links];
    prev = new node * [links];
  }

  template <typename T>
  skiplist<T>::node::node (T && value_)
  {
    value = std::move(value_);
    links = 1;
    while (rand() % 2 == 0)
      ++ links;
    next = new node * [links];
    prev = new node * [links];
  }

  template <typename T>
  skiplist<T>::node::~node ()
  {
    // for (uint i=0; i<links; ++i)
    //   {
    // 	prev[i] -> next[i] = next[i];
    // 	next[i] -> prev[i] = prev[i];
    //   }

    delete [] next;
    delete [] prev;
  }

  template <typename T>
  void skiplist<T>::node::resize (size_t new_links)
  {
    node ** new_next = new node * [new_links];
    node ** new_prev = new node * [new_links];

    for (size_t i = 0; i < links; ++i)
      {
	new_next[i] = next[i];
	new_prev[i] = prev[i];
      }
    for (size_t i = links; i < new_links; ++i)
      {
	new_next[i] = this;
	new_prev[i] = this;
      }

    delete [] next;
    delete [] prev;

    links = new_links;
    next = new_next;
    prev = new_prev;
  }

  // End struct node



  template <typename T>
  skiplist<T>::skiplist ()
  {
    anchor = new node;
    for (uint i=0; i < anchor->links; ++i)
      {
	anchor -> prev[i] = anchor;
	anchor -> next[i] = anchor;
      }
  }

  template <typename T>
  skiplist<T>::~skiplist ()
  {
    node * n = anchor -> next[0];
    anchor -> next[0] = nullptr;
    node * np;
    while (n)
      {
	np = n;
	n = n -> next[0];
	delete np;
      }
  }

  template <typename T>
  typename skiplist<T>::iterator skiplist<T>::begin ()
  { return iterator (anchor -> next[0]); }

  template <typename T>
  typename skiplist<T>::iterator skiplist<T>::end ()
  { return iterator (anchor); }

  template <typename T>
  bool skiplist<T>::empty () const
  { return num_nodes == 0; }

  template <typename T>
  size_t skiplist<T>::size () const
  { return num_nodes; }

  template <typename T>
  typename skiplist<T>::iterator skiplist<T>::insert (T const & value)
  {
    node * n = new node (value);
    return insert (n);
  }

  template <typename T>
  typename skiplist<T>::iterator skiplist<T>::insert (T && value)
  {
    node * n = new node (std::move(value));
    return insert (n);
  }

  template <typename T>
  typename skiplist<T>::iterator skiplist<T>::insert (node * n)
  {
    if (n -> links > anchor -> links)
      anchor -> resize(n -> links);

    node * loc = anchor;
    size_t i = anchor -> links;
    do
      {
	-- i;
	while (loc -> next[i] != anchor &&
	       loc -> next[i] -> value < n -> value)
	  loc = loc -> next[i];
      }
    while (i > 0);

    node * r = loc -> next[0];

    for (size_t i = 0; i < n -> links; ++i)
      {
	while (i >= r -> links)
	  r = r -> next[i-1];
	n -> next[i] = r;
	r -> prev[i] = n;

	while (i >= loc -> links)
	  loc = loc -> prev[i-1];
	n -> prev[i] = loc;
	loc -> next[i] = n;
      }

    ++ num_nodes;
    return iterator (n);
  }


  template <typename T>
  class skiplist<T>::iterator : public std::iterator<std::forward_iterator_tag, T>
  {
    node * n;

  public:
    iterator (node * n_) : n (n_) {}
    iterator (iterator const & iter) : n (iter.n) {}

    T & operator * ()
    { return n -> value; }

    iterator & operator ++ ()
    { n = n -> next[0]; return *this; }

    iterator operator ++ (int)
    { iterator tmp (*this); ++ (*this); return tmp; }

    iterator & operator -- ()
    { n = n -> prev[0]; return *this; }

    iterator operator -- (int)
    { iterator tmp (*this); -- (*this); return tmp; }

    bool operator == (iterator const & iter)
    { return n == iter.n; }

    bool operator != (iterator const & iter)
    { return n != iter.n; }
  }; // class iterator
}

#endif
