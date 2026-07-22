#pragma once

#include <cstdint>

namespace lob {
enum class Side{
    Buy = 0,
    Sell = 1
};

using OrderId = std::uint64_t;
using Price = std::uint32_t;
using Quantity = std::uint32_t;

class Order{
    

    // Members
    OrderId id_;
    Price price_;
    Quantity quantity_;
    Side side_;

    public:

        Order(OrderId id_in, Price price_in, Quantity quantity_in, Side side_in);
        ~Order() = default;

        // Copy        
        Order(const Order& order) = delete;
        Order& operator=(const Order&) = delete;

        // Move
        Order(Order&& order) noexcept = default;
        Order& operator=(Order&&) noexcept = default;
        

        // Accessors
        [[nodiscard]] OrderId id() const noexcept {return id_;};
        [[nodiscard]] Price price() const noexcept {return price_;};
        [[nodiscard]] Quantity quantity() const noexcept {return quantity_;};
        [[nodiscard]] Side side() const noexcept {return side_;};
        [[nodiscard]] bool is_filled() const noexcept {return quantity_ == 0;};
        
        // Mutator
        [[nodiscard]] Quantity fill(const Quantity amount) noexcept;
        
};
}