# UUtils

C++20 library with random stuff I wrote for fun

## Features

* Zero allocation C# LINQ-like data processing
* Static vector

## TODO Features

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

Static vector:
```cpp
#include <uutils/static_vector.h>

uutils::StaticVector<int, 10> vector;

vector.push_back(1);
vector.push_back(2);
vector.push_back(3);

assert(vector.size() == 3);

for (int v : vector)
{
    std::cout << v << '\n';
}

vector.erase(vector.begin() + 1);
assert(vector.size() == 2 && vector.front() == 1 && vector.back() == 3);
```