#ifndef AD_SKIPLIST_HXX
#define AD_SKIPLIST_HXX

#include <cstddef>
#include <functional>
#include <iterator>

#include <iostream>

namespace ad
{
  template <typename T>
  class skiplist
  {
    struct node;

    node * anchor;
    size_t num_nodes = 0;

  public:

    skiplist ();
    ~ skiplist ();

    class iterator;
    iterator begin ();
    iterator end ();

    bool empty () const;
    size_t size () const;

    iterator find (T const & value);

    iterator insert (T const & value);

  };

  template <typename T>
  struct skiplist<T>::node
  {
    T value;
    size_t links;
    node ** next;
    node ** prev;

    node (T const & value_ = T());
    node (T && value_);
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
    value = std::move(value);
    links = 1;
    while (rand() % 2 == 0)
      ++ links;
    next = new node * [links];
    prev = new node * [links];
  }

  template <typename T>
  skiplist<T>::node::~node ()
  {
    delete next;
    delete prev;
  }

  template <typename T>
  void skiplist<T>::node::resize (size_t new_links)
  {
    node ** new_next = new node * [new_links];
    node ** new_prev = new node * [new_links];
    for (size_t i=0; i<links; ++i)
      {
	new_next[i] = next[i];
	new_prev[i] = prev[i];
      }
    for (size_t i=links; i<new_links; ++i)
      {
	new_next[i] = this;
	new_prev[i] = this;
      }
    delete [] next;
    delete [] prev;
    next = new_next;
    prev = new_prev;
    links = new_links;
  }
  // end struct node;






  template <typename T>
  skiplist<T>::skiplist ()
  {
    anchor = new node;
    anchor -> next[0] = anchor;
    anchor -> prev[0] = anchor;
  }

  template <typename T>
  skiplist<T>::~skiplist ()
  {
    anchor -> prev[0] -> next[0] = nullptr;
    node * n = anchor -> next[0];
    while (n)
      {
	node * np = n;
	n = n -> next[0];
	delete np;
      }
    delete anchor;
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
  typename skiplist<T>::iterator skiplist<T>::find (T const & value)
  {
    size_t h = anchor -> links;
    node * n = anchor;

    while (h != 0)
      {
	-- h;
	while (n -> next[h] != anchor &&
	       n -> next[h] -> value < value)
	  n = n -> next[h];
      }

    if (n -> next[0] == anchor ||
	value < n -> next[0] -> value)
      return iterator (anchor);
    else return iterator (n);
  }

  template <typename T>
  typename skiplist<T>::iterator skiplist<T>::insert (T const & value)
  {
    node * n = new node (value);
    if (n -> links > anchor -> links)
      anchor -> resize(n -> links);

    size_t h = anchor -> links;
    node * loc = anchor;

    while (h != 0)
      {
	-- h;
	while (loc -> next[h] != anchor &&
	       loc -> next[h] -> value < value)
	  loc = loc -> next[h];
      }

    node * r = loc -> next[0];
    for (size_t i=0; i < n -> links; ++i)
      {
	while (i >= loc -> links) loc = loc -> prev[i-1];
	while (i >= r   -> links) r   = r   -> next[i-1];
	loc -> next[i] = n;
	r   -> prev[i] = n;
	n -> prev[i] = loc;
	n -> next[i] = r;
      }

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

    bool operator == (iterator iter) const
    { return n == iter.n; }

    bool operator != (iterator iter) const
    { return n != iter.n; }
  }; // iterator
}

#endif
