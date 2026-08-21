#pragma once

#include "Order.hpp"
#include "PriceLevel.hpp"
#include "Units.hpp"
#include <cstddef>
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

            [[nodiscard]] std::vector<ExecutionReport> add(const OrderId id, const Price price, Quantity quantity, const Side side);
            [[nodiscard]] std::optional<CancelReport> cancel(const OrderId id) noexcept;

            [[nodiscard]] std::optional<Price> best_bid() const;
            [[nodiscard]] std::optional<Price> best_ask() const;

            [[nodiscard]] size_t bid_levels() const {return buy_.size();}
            [[nodiscard]] size_t ask_levels() const {return sell_.size();}
            [[nodiscard]] bool contains(OrderId id) const {return id_map_.contains(id);}
            [[nodiscard]] std::optional<Quantity> resting_quantity(OrderId id) const;
            [[nodiscard]] std::optional<Quantity> level_quantity(Side side, Price price)const;

            [[nodiscard]] std::optional<std::string>  check_invariants() const;
            [[nodiscard]] std::optional<std::string>  check_map_validity() const;
            [[nodiscard]] std::optional<std::string>  check_levels_validity() const;
    };
}