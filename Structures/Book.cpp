#include "Book.hpp"
#include "Order.hpp"
#include "PriceLevel.hpp"

namespace lob {
    std::vector<ExecutionReport> Book::add(const OrderId id, const Price price, Quantity quantity, const Side side) noexcept{
        std::vector<ExecutionReport> execution{};

        auto fulfillOrder = [&](auto& data){
            auto& level = data.begin()->second;
            Price orderPrice = level.price();

            auto result = level.execute(quantity);
            quantity -= result.fulfilled;

            if(level.is_exhausted())
                data.erase(orderPrice);
            execution.push_back(result);
        };

        auto createOrder = [&](auto& data){
            if(!data.contains(price))
                data.try_emplace(price, PriceLevel(price));
            data[price].push(Order{id, price, quantity, side});
            
        };

        if (side == Side::Buy) {
            while (!sell_.empty() && price >= sell_.begin()->first && quantity > 0) {
                fulfillOrder(sell_);
            }
            if(quantity > 0)
                createOrder(buy_);
            
        }else {
            while (!buy_.empty() && price <= buy_.begin()->first && quantity > 0) {
                fulfillOrder(buy_);
            }
            if(quantity > 0)
                createOrder(sell_);
        }

        return execution;
    }

    CancelReport Book::cancel(const OrderId id) noexcept{
        if(id_map_.contains(id)){
            auto& level = id_map_[id].level_;
            auto& order = *id_map_[id].order_;
            Price price = order.price();
            
            Quantity qty = level->cancel(order);
            
            if(level->is_exhausted()){
                if(order.side() == Side::Buy)
                    buy_.erase(price);
                else
                    sell_.erase(price);
            }
            id_map_.erase(id);

            return {id, price, qty};
        }

        return {};
    }
}