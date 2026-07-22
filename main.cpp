#include <vector>
#include <print>
#include "Order.hpp"

int main() {
    std::vector<int> v;
    std::print("Running");
    
    lob::Order sale{123, 1001, 50, lob::Side::Sell};
}