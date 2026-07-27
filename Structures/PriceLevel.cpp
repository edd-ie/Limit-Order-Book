#include "PriceLevel.hpp"
#include "Order.hpp"
#include "Report.hpp"
#include <vector>

namespace lob {
    PriceLevel::PriceLevel(const Price price):
        price_(price), quantity_(0){};

    void PriceLevel::pop() noexcept{
        if(!orders_.empty()){
            quantity_ -= orders_.front().quantity();
            orders_.pop_front();
        }
    }

    Order& PriceLevel::push(Order&& order) noexcept{
        orders_.push_back(std::move(order));
        quantity_ += orders_.back().quantity();
        return orders_.back();
    }

    Order& PriceLevel::peek() noexcept{
        return orders_.front();
    }

    std::pair<Quantity, std::vector<Report>> PriceLevel::execute(Quantity amount) noexcept{
        std::pair<Quantity, std::vector<Report>> report;
        if(quantity_ == 0){
            report.first = quantity_;
            report.second = {};
            return report;
        }
        
        Quantity fulfilled = 0;
        std::vector<Report> executions;
        executions.reserve(orders_.size());

        while(fulfilled < amount && !orders_.empty()){
            Order& current = orders_.front();

            if(current.is_filled()){
                orders_.pop_front();
                continue;
            }
            Report details{current.id(), price_, current.fill(amount), false};

            fulfilled += details.quantity;
            quantity_ -= details.quantity;
            if(current.is_filled()){
                details.filled = true;
                orders_.pop_front();
            }
            executions.push_back(std::move(details));
        }

        report.first = fulfilled;
        report.second = std::move(executions);

        return report;
    }
}