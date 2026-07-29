#pragma once

#include "Order.hpp"
#include <vector>
namespace lob {
    struct Report{
        OrderId id;
        Price price;
        Quantity quantity;
        bool fully_filled;
    } ;

    struct ExecutionResult{
        Quantity fulfilled;
        std::vector<Report> reports;
    };
}