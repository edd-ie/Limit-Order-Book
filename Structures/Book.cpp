#include "Book.hpp"
#include <cassert>
#include <utility>

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

            auto result = level.execute(quantity);
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

        return CancelReport{id, price, canceled_qty};
    }

} // namespace lob