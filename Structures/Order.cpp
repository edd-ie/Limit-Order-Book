#include "Order.hpp"
#include <algorithm>

Order::Order (uint32 price_in, uint32 quantity_in, Side side_in):
    id_(next_id++), price_(price_in), quantity_(quantity_in), side_(side_in), arrival_time_(std::chrono::high_resolution_clock::now()){}

bool Order::operator== (const Order &other) const noexcept {
    return this->id_==other.id_;
}


bool Order::operator< (const Order &other) const noexcept {
    if(price_ != other.price_){
        // Buy orders: Higher price = higher priority (comes first)
        // Sell orders: Lower price = higher priority (comes first)
        return side_ == Side::Buy ? price_ > other.price_ : price_ < other.price_;
    }
    if(arrival_time_ != other.arrival_time_)
        return arrival_time_ < other.arrival_time_;       
    return quantity_ > other.quantity_;
}

bool Order::operator> (const Order &other) const noexcept {
    return other < *this;
}

Order& Order::operator-=(const uint32 amount) noexcept{
    quantity_ -= std::min(quantity_, amount);
    return *this;
}

uint64 Order::id() const noexcept{
    return id_;
}
    
uint32 Order::price() const noexcept{
    return price_;
}
    
uint32 Order::quantity() const noexcept{
    return quantity_;
}
    
TimePoint Order::arrival_time() const noexcept{
    return arrival_time_;
}
    
Side Order::side() const noexcept{
    return side_;
}

    
void Order::update_quantity(const uint32 amount) noexcept{
    quantity_ = amount;
}

bool Order::is_filled() const noexcept { 
    return quantity_ == 0; 
}