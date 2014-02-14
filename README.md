# Data Structures #
A collection of data structures in the spirit of the C++ Standard Library

## splaytree (work in progress) ##
See http://en.wikipedia.org/wiki/Splay_Tree

Splay trees move a node to the root every time it is accessed.
This makes recently accessed nodes much faster to find.
For many practical applications, they can be faster than other trees.

## skiplist (work in progress) ##
See http://en.wikipedia.org/wiki/Skip_list

A skip list is a linked list with multiple layers.
The bottom layer links each node to its neighbor, while the higher layers link nodes over larger distances.
Operations have the same average time complexity as a balanced tree.