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

        using container=std::deque<Order>;
        using iterator = typename container::iterator;
        using const_iterator = typename container::const_iterator;

        public:
            PriceLevel(const Price price);
            ~PriceLevel() = default;

            PriceLevel(const PriceLevel&) = delete;
            PriceLevel& operator=(const PriceLevel&) = delete;

            PriceLevel(PriceLevel&&) noexcept = default;
            PriceLevel& operator=(PriceLevel&&) noexcept = default;

            [[nodiscard]] Price price() const noexcept {return price_;}
            [[nodiscard]] Quantity quantity() const noexcept {return quantity_;}

            iterator begin(){return orders_.begin();}
            iterator end(){return orders_.end();}
            const_iterator begin() const {return orders_.begin();}
            const_iterator end() const {return orders_.end();}

            /**
             * @brief Add an order to the PriceLevel
             * 
             * @param order 
             * @return Order& 
             */
            Order& push(Order&& order);

            /**
             * @brief Check if the PriceLevel queue is empty;
             * quantity() == 0 ⟺ empty_queue()
             * 
             * @return true 
             * @return false 
             */
            [[nodiscard]] bool empty_queue() const{return orders_.empty();}

            /**
             * @brief check if the total quantity of shares is zero;
             * quantity() == 0 ⟺ empty_queue()
             * 
             * @return true 
             * @return false 
             */
            [[nodiscard]] bool is_exhausted() const{return quantity_==0;}
        
            Quantity cancel(Order& order) noexcept;

            /**
             * @brief Execute a quantity of live shares from the PriceLevel.
             * 
             * @param amount quantity requested.
             * @return ExecutionResult quantity of the requested amount actually fulfilled and which orders were used.
             */
            [[nodiscard]]ExecutionReport execute(Quantity amount);

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