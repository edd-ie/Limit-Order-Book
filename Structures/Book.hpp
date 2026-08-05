#pragma once

#include "Order.hpp"
#include "PriceLevel.hpp"
#include <functional>
#include <map>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace lob{
    
    class Book{
        // Out-of-memory errors are fatal
        struct OrderPtr{
            Order* order_; 
            PriceLevel* level_;
        };

        std::string symbol_;
        std::unordered_map<OrderId, OrderPtr> id_map_{};
        std::map<Price, PriceLevel,std::greater<Price>> buy_{};
        std::map<Price, PriceLevel> sell_{};

        void eraseFilledIds(const ExecutionReport& report) noexcept;

        public:
            Book(std::string symbol_in):symbol_(symbol_in){}
            ~Book() = default;

            Book(const Book&) = delete;
            Book& operator=(const Book&) = delete;

            Book(Book&&) noexcept = default;
            Book& operator=(Book&&) noexcept = default;


            [[nodiscard]]std::string_view symbol() const noexcept{return std::string_view{symbol_};}

            [[nodiscard]]std::vector<ExecutionReport> add(const OrderId id, const Price price, Quantity quantity, const Side side);
            [[nodiscard]]std::optional<CancelReport> cancel(const OrderId id) noexcept;



    };
}