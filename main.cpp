#include <vector>
#include <print>
#include "Order.hpp"
#include "PriceLevel.hpp"

int main() {
    std::vector<int> v;
    std::print("Running");
    
    lob::Order order{1, 1001, 50, lob::Side::Buy};
    lob::Order order2{2, 1001, 300, lob::Side::Buy};
    lob::PriceLevel pLvl{1001};

    pLvl.push(std::move(order));
    pLvl.push(std::move(order2));

    std::println("{}", pLvl.execute(350).to_string());
}