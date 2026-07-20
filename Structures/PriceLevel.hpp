#include <deque>
#include "Order.hpp"

class PriceLevel{
    uint32 price_;
    uint32 quantity_;
    std::deque<Order> orders_{};

    public:
        PriceLevel(const uint32 price);
        ~PriceLevel() = default;

        PriceLevel(const PriceLevel&) = delete;
        PriceLevel operator=(const PriceLevel&) = delete;

        PriceLevel(PriceLevel&&) noexcept = default;
        PriceLevel& operator=(PriceLevel&&) noexcept;

        PriceLevel& operator+=(Order order) noexcept;
        
        uint32 pop(uint32 amount) noexcept;
};