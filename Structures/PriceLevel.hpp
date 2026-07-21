#pragma once

#include <deque>
#include <utility>
#include <vector>
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
        PriceLevel& operator=(PriceLevel&&) noexcept = default;

        PriceLevel& operator+=(Order&& order) noexcept;
        
        [[nodiscard]] Order& pop() noexcept;
        [[nodiscard]] Order& peek() noexcept;
        [[nodiscard]] uint32 quantity() const noexcept;

        [[nodiscard]] std::pair<uint32, std::deque<Order>> get_shares(const uint32 amount) noexcept;
};