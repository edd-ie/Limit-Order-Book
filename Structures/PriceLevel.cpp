#include "PriceLevel.hpp"
#include <utility>

PriceLevel::PriceLevel(uint32 price):
    price_(price), quantity_(0){};

// PriceLevel& PriceLevel::operator=(PriceLevel&& other) noexcept{
//     this->price_ = other.price_;
//     this->quantity_ = other.quantity_;
//     orders_ = std::move(other.orders_);
//     return *this;
// }

// PriceLevel& PriceLevel::operator+=(Order order)  noexcept{
//     quantity_ += order.quantity();
//     orders_.push_back(order);
//     return *this;
// }
