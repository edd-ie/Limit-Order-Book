#pragma once

#include <deque>
#include <utility>
#include <vector>
#include "Order.hpp"
#include "Report.hpp"

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

            [[nodiscard]] Price price() const noexcept {return price_;}
            [[nodiscard]] Quantity quantity() const noexcept {return quantity_;}

            void pop() noexcept;
            Order& push(Order&& order) noexcept;
            [[nodiscard]] Order& peek() noexcept;
        
            std::pair<Quantity, std::vector<Report>> execute(Quantity amount) noexcept;

        };
};