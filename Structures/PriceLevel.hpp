#pragma once

#include <deque>
#include "Order.hpp"
#include "Report.hpp"
#include <cassert>

namespace lob {
    class PriceLevel{
        Price price_;
        Quantity quantity_{0};
        std::deque<Order> orders_{};

        public:
            PriceLevel(const Price price);
            ~PriceLevel() = default;

            PriceLevel(const PriceLevel&) = delete;
            PriceLevel& operator=(const PriceLevel&) = delete;

            PriceLevel(PriceLevel&&) noexcept = default;
            PriceLevel& operator=(PriceLevel&&) noexcept = default;

            [[nodiscard]] Price price() const noexcept {return price_;}
            [[nodiscard]] Quantity quantity() const noexcept {return quantity_;}

            
            Order& push(Order&& order);
            [[nodiscard]] bool empty_queue() const;
            [[nodiscard]] bool is_exhausted() const;
        
            Quantity cancel(Order& order) noexcept;
            ExecutionResult execute(Quantity amount);

            [[nodiscard]] Order& peek() noexcept {
                assert(!orders_.empty());
                return orders_.front();
            }

            [[nodiscard]] const Order& peek() const noexcept {
                assert(!orders_.empty());
                return orders_.front();
            }

        };
}