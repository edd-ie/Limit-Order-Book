#include <vector>
#include <print>
#include "Order.hpp"

int main() {
    std::vector<int> v;
    std::print("Running");
    
    Order sale{1005, 50, Side::Sell};
}