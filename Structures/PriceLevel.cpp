#include "PriceLevel.hpp"
#include "Order.hpp"
#include "Report.hpp"
#include <vector>

namespace lob {
    PriceLevel::PriceLevel(const Price price):
        price_(price){}



    Order& PriceLevel::push(Order&& order){
        assert(order.price() == price_);
        orders_.push_back(std::move(order));
        quantity_ += orders_.back().quantity();
        return orders_.back();
    }


    /**
     * @brief cancels an order and decrements the cached total.
     * If the total goes to 0, the queue will be cleared
     * 
     * @param order to be cancelled. Object may be destroyed; caller must not dereference its pointer after this returns
     * @return Quantity of the cancelled order
     */
    Quantity PriceLevel::cancel(Order& order) noexcept{
        assert(order.price() == price_);
        const Quantity amount =  order.cancel();
        quantity_ -= amount;
        if(quantity_ == 0)
            orders_.clear();
        return amount;
    }

    ExecutionResult PriceLevel::execute(Quantity amount) {
        ExecutionResult report{};
        if(quantity_ == 0){
            assert(orders_.empty());
            report.fulfilled = 0;
            return report;
        }
        
        Quantity fulfilled = 0;
        std::vector<Report> executions;

        while(fulfilled < amount && !orders_.empty()){
            Order& current = orders_.front();

            if(current.is_filled()){
                orders_.pop_front();
                continue;
            }
            Report details{current.id(), price_, current.fill(amount-fulfilled), false};

            fulfilled += details.quantity;
            quantity_ -= details.quantity;
            if(current.is_filled()){
                details.fully_filled = true;
                orders_.pop_front();
            }
            executions.push_back(std::move(details));
        }

        report.fulfilled = fulfilled;
        report.reports = std::move(executions);

        if(quantity_ == 0)
            orders_.clear();

        return report;
    }
}