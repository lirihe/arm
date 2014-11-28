Hash tables, Linked lists and Array Utility
===========================================

The gomspace library includes the data storage utility called [uhash](http://uthash.sourceforge.net/userguide.html).

Strictly speaking this is not a library, it's a set of header files containing some rather clever macros. These macros include `uthash.h` for hash tables, `utlist.h` for linked lists and `utarray.h` for arrays.

The list macros support the basic linked list operations: adding and deleting elements, sorting them and iterating over them. It does so for both single linked list double linked lists and circular lists.

The dynamic array macros supports basic operations such as push, pop, and erase on the array elements. These array elements can be any simple datatype or structure. The array operations are based loosely on the C++ STL vector methods. Internally the dynamic array contains a contiguous memory region into which the elements are copied. This buffer is grown as needed using `realloc` to accomodate all the data that is pushed into it.

The hash tables provides a good alternative to linked lists for larger tables where scanning through the entire list is going to be slow. The overhead added is larger memory usage and the additional hash processing time, so for short sets of data linked lists are preferred.

