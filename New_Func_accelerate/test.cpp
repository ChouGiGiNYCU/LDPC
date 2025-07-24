#include <iostream>
#include <chrono>
#include <boost/dynamic_bitset.hpp>
#include "UseFuction.h"
int main() {
    boost::dynamic_bitset<> b(8); // 長度 8，預設為 00000000
    b[0]=1;
    std::cout << b.count() << std::endl;
}