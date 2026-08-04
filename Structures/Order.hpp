#pragma once

#include "Units.hpp"

namespace lob {
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
        [[nodiscard]] OrderId id() const noexcept {return id_;}
        [[nodiscard]] Price price() const noexcept {return price_;}
        [[nodiscard]] Quantity quantity() const noexcept {return quantity_;}
        [[nodiscard]] Side side() const noexcept {return side_;}

        /**
         * @brief Check if quantity() == 0 
         * 
         * @return true 
         * @return false 
         */
        [[nodiscard]] bool is_filled() const noexcept {return quantity_ == 0;}
        
        /**
         * @brief Execute a specified quantity of orders
         * 
         * @param amount 
         * @return Quantity amount of the requested orders fulfilled
         */
        [[nodiscard]] Quantity fill(const Quantity amount) noexcept;
        [[nodiscard("Alert! Order was Cancelled.")]] Quantity cancel() noexcept;
};
}