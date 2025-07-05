# UUtils

C++20 library with random stuff I wrote for fun

## Features

* Zero allocation C# LINQ-like data processing

## TODO Features

* Static vector
* Small vector

## Usage examples

Data processing:
```cpp
#include <uutils/data_processing.h>

using uutils::data_processing;

auto result = range(0, 100)
    > filter([](int x) { return x % 2 == 0; })
    > map([](int x) { return x * x; })
    > to_vector();

// Data from somewhere
std::vector<int> data = { 1, 2, 3, 4, 5 };
int sum = Range::from(data)
    > skip(2)
    > take(5) 
    > sum();

std::array<float, 10> ten_dimensional_vector = // ...
bool non_negative = Range::from(ten_dimensional_vector)
    > all([](float x) { return x >= 0; }); // or none([](float x) { return x < 0; })
```
