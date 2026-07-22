#pragma once

#include <deque>
#include <utility>
#include "Order.hpp"

namespace lob {
    class PriceLevel{
        Price price_;
        Quantity quantity_;
        std::deque<Order> orders_{};

        public:
            PriceLevel(const Price price);
            ~PriceLevel() = default;

            PriceLevel(const PriceLevel&) = delete;
            PriceLevel operator=(const PriceLevel&) = delete;

            PriceLevel(PriceLevel&&) noexcept = default;
            PriceLevel& operator=(PriceLevel&&) noexcept = default;

            PriceLevel& operator+=(Order&& order) noexcept;
            
            [[nodiscard]] Order& pop() noexcept;
            [[nodiscard]] Order& peek() noexcept;
            Quantity quantity() const noexcept;

            [[nodiscard]] std::pair<Quantity, std::deque<Order>> get_shares(const Quantity amount) noexcept;
    };
};