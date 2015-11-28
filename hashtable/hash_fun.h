/*
 * Copyright (c) 1994
 * Javier Barreiro and David R. Musser
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  The authors make no representation 
 * about the suitability of this software for any purpose.  It is 
 * provided "as is" without express or implied warranty.
 *
 */

// A few hash functions that can be used with unordered associative 
// containers hash_set, hash_multiset, hash_map, or hash_multimap.
// They all map from a string to a long, where a string
// is as defined in bstring.h (basic_string<char>).  Each is defined
// in a function object type suitable for passing to a constructor
// of an unordered associative container.

#ifndef hash_fun_h_included
#define hash_fun_h_included

#define STL_USING_STRING
#include "stl.h"

inline long hash_fun1(const std::string& c, const long prime)
{
      int n = c.length();
      const char* d = c.c_str();
      long h = 0; 
      
      for (int i = 0; i < n; ++i, ++d)
          h = (h << 2) + *d;

      return ((h >= 0) ? (h % prime) : (-h % prime)); 
}

inline long hash_fun2(const std::string& c, const long prime)
{
      int n = c.length();
      const char* d = c.c_str();
      long h = n; 
      
      for (int i = 0; i < n; ++i, ++d)
          h = 613*h + *d;

      return ((h >= 0) ? (h % prime) : (-h % prime)); 
}
      
#endif /* hash_fun_h_included */
