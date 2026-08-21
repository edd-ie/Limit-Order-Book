#include "Book.hpp"
#include <cassert>
#include <format>

namespace lob {

    void Book::eraseFilledIds(const ExecutionReport& report) noexcept {
        for (const auto& order : report.reports) {
            if (order.fully_filled) {
                id_map_.erase(order.id);
            }
        }
    }

    std::vector<ExecutionReport> Book::add(const OrderId id, const Price price, Quantity quantity, const Side side) {
        std::vector<ExecutionReport> execution{};

        auto fulfillOrder = [&](auto& data) {
            auto it = data.begin();
            auto& level = it->second;

            ExecutionReport result = level.execute(quantity);
            quantity -= result.fulfilled;

            eraseFilledIds(result);

            if (level.is_exhausted()) {
                data.erase(it); // Erase by iterator directly
            }
            execution.push_back(std::move(result)); // Move result into vector
        };

        auto createOrder = [&](auto& data) {
            auto [it, inserted] = data.try_emplace(price, price);
            Order& order = it->second.push(Order{id, price, quantity, side});
            
            // Register in id_map_
            auto [entry, success] = id_map_.emplace(id, OrderPtr{.order_ = &order, .level_ = &it->second});
            assert(success);
        };

        if (side == Side::Buy) {
            while (!sell_.empty() && price >= sell_.begin()->first && quantity > 0) {
                fulfillOrder(sell_);
            }
            if (quantity > 0) {
                createOrder(buy_);
            }
        } else {
            while (!buy_.empty() && price <= buy_.begin()->first && quantity > 0) {
                fulfillOrder(buy_);
            }
            if (quantity > 0) {
                createOrder(sell_);
            }
        }

        return execution;
    }

    std::optional<CancelReport> Book::cancel(const OrderId id) noexcept {
        auto it = id_map_.find(id);
        if (it == id_map_.end()) {
            return {};
        }

        auto [order_ptr, level_ptr] = it->second;
        const Price price = order_ptr->price();
        const Side side = order_ptr->side();

        const Quantity canceled_qty = level_ptr->cancel(*order_ptr);

        if (level_ptr->is_exhausted()) {
            if (side == Side::Buy) {
                buy_.erase(price);
            } else {
                sell_.erase(price);
            }
        }

        id_map_.erase(it); // Erase from map using iterator

        return CancelReport{id, price, canceled_qty, true};
    }

    std::optional<Price>  Book::best_bid() const {
        if (buy_.empty()) {
            return {};
        }
        return buy_.begin()->first;
    }
    
    std::optional<Price>  Book::best_ask() const {
        if (sell_.empty()) {
            return {};
        }
        return sell_.begin()->first;
    }

    std::optional<Quantity> Book::resting_quantity(OrderId id) const{
        auto order = id_map_.find(id);
        if ( order != id_map_.end()){
            assert(order->second.order_ != nullptr);
            return order->second.order_->quantity();
        }
        return {};
    }

    std::optional<Quantity> Book::level_quantity(Side side, Price price)const{
        if(side == Side::Sell){
            if(auto level =  sell_.find(price); level != sell_.end()){
                return level->second.quantity();
            }
        }
        else{
            if(auto level =  buy_.find(price); level != buy_.end()){
                return level->second.quantity();
            }
        }
        return {};
    }

    std::optional<std::string> Book::check_map_validity() const{
        for(const auto& order : id_map_){
            if(order.second.order_->is_filled()){
                return std::format("Error: Cancelled/Fulfilled OrderId# {} was not removed from id_map_", order.first);
            }
        }

        for (auto level = buy_.begin(); level != buy_.end(); level++) {
            auto &priceLevel = level->second;
            for(auto order = priceLevel.begin(); order != priceLevel.end(); order++){
                if(!order->is_filled() && !id_map_.contains(order->id())){
                    return std::format("Error: Live OrderId# {} was not found in the id_map_", order->id());
                }
            }
        }

        for (auto level = sell_.begin(); level != sell_.end(); level++) {
            auto &priceLevel = level->second;
            for(auto order = priceLevel.begin(); order != priceLevel.end(); order++){
                if(!order->is_filled() && !id_map_.contains(order->id())){
                    return std::format("Error: Live OrderId# {} was not found in the id_map_", order->id());
                }
            }
        }
        return {};        
    }

    std::optional<std::string> Book::check_levels_validity() const{
        if (!buy_.empty() && !sell_.empty()){
            if(buy_.begin()->first >=  sell_.begin()->first)
                return "Error: Best bid >= Best Ask";
        }
        for (auto level = buy_.begin(); level != buy_.end(); level++) {
            if(level->second.is_exhausted()) 
                return std::format("Error: Exhausted Price Level ${} was not removed from buy_", level->first);
        }
        for (auto level = sell_.begin(); level != sell_.end(); level++) {
            if(level->second.is_exhausted()) 
                return std::format("Error: Exhausted Price Level ${} was not removed from sell_", level->first);
        }
        return {};        
    }

    std::optional<std::string> Book::check_invariants() const{
        if(auto check = check_map_validity(); check){
            return check;
        }
        if(auto check = check_levels_validity(); check){
            return check;
        }
        return {};
    }
} // namespace lob