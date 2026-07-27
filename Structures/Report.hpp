#pragma once

#include "Order.hpp"
namespace lob {
    struct Report{
        OrderId id;
        Price price;
        Quantity quantity;
        bool filled;
    } ;
};