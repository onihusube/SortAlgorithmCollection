# Sort Algorithm Collection

[![Build status](https://ci.appveyor.com/api/projects/status/vscdijos680uomlv/branch/master?svg=true)](https://ci.appveyor.com/project/onihusube/sortalgorithmcollection/branch/master)

実装中…

Implemented algorithms.
- [x] bubble sort
- [x] shaker sort

Need for C++17 compiler. For exmaple:
- [x] Visual Studio 2017 15.9.4 or later
- [x] clang 7.0.0 or later
- [x] gcc 8.2.0 or later

sample code.
```cpp

#include "sort.hpp"

void sample_sort() {
  int array[] = {8, 1, 3, 2, 4, 6, 5, 7};
  
  //sorted by bubble sort.
  sort_collection::sort<sort_collection::bubble_sort>(array);
  
  for (auto& n : array) {
    std::cout << n << ", ";
  }
  //1, 2, 3, 4, 5, 6, 7, 8, 
}

void sample_sort2() {
  int array[] = {8, 1, 3, 2, 4, 6, 5, 7};
  
  //create functor for sort.
  constexpr sort_collection::shaker_sort shaker_sort{};
  
  //sorted by shaker sort.
  shaker_sort(std::begin(array), std::end(array), std::less<int>{});
  
  for (auto& n : array) {
    std::cout << n << ", ";
  }
  //1, 2, 3, 4, 5, 6, 7, 8, 
}

```
[Run the sample code in Wandbox.](https://wandbox.org/permlink/vdYXQeAk3jvrTsVg)
