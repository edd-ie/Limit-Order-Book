#include <print>
#include "Book.hpp"

int main() {
    std::println("hello");
    lob::Book bk{"Stock"};
    auto add = bk.add(1, 1001, 300, lob::Side::Sell);

    for(auto report : add)
        std::println("HUH {}", report.to_string());
}