#include "PriceLevel.hpp"
#include <iterator>
#include <utility>

PriceLevel::PriceLevel(uint32 price):
    price_(price), quantity_(0){};

PriceLevel& PriceLevel::operator+=(Order&& order) noexcept{
    quantity_ += order.quantity();
    orders_.push_back(order);
    return *this;
}

Order& PriceLevel::pop() noexcept{
    Order& order = orders_.front();
    quantity_ -= order.quantity();
    orders_.pop_front();
    return order;
}

Order& PriceLevel::peek() noexcept{
    return  orders_.front();
}

uint32 PriceLevel::quantity() const noexcept{
    return quantity_;
}

/**
 * @brief Returns available shares that full fill the requested amount if not enough is available 
 * the deficit amount is returned with the available shares
 * 
 * @param amount quantity of shares requested at this price
 * @return std::pair<uint32, std::vector<Order>> deficit and shares
 */
std::pair<uint32, std::deque<Order>> PriceLevel::get_shares(const uint32 amount) noexcept{
    std::pair<uint32, std::deque<Order>> results;
    int deficit = amount;
    if(quantity_ < amount){
        deficit -= quantity_;
        quantity_ = 0;
        return std::pair<uint32, std::deque<Order>>(deficit, std::move(orders_));
    }
    
    std::deque<Order> shares{};
    while (deficit != 0) {
        Order& front = orders_.front();
        if (front.quantity() <= deficit){            
            orders_.pop_front();
            shares.push_back(front);
            quantity_ -= front.quantity();
            deficit -= front.quantity();
        }else{
            
        }
    }
}