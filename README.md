# SortAlgorithmCollection

実装中…

[x]bubble sort
[x]shaker sort

```cpp

#include "sort.hpp"

void sample_sort() {
  int array[] = {8, 1, 3, 2, 4, 6, 5, 7};
  
  //sorted by bubble sort.
  sort_collection::sort<sort_collection::bubble_sort>(array);
  
  for(auto& n : array) {
    std::cout << n << ", ";
  }
  //1, 2, 3, 4, 5, 6, 7, 8, 
}

```
