#include "Order.hpp"
#include <algorithm>
#include <cassert>

namespace lob {
    Order::Order (OrderId id_in, Price price_in, Quantity quantity_in, Side side_in):
    id_(id_in), price_(price_in), quantity_(quantity_in), side_(side_in){}

    
    Quantity Order::fill(const Quantity amount) noexcept {
        const Quantity filled = std::min(amount, quantity_);
        quantity_ -= filled;
        return filled; // Returns actual qty executed
    }
};
