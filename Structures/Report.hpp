#pragma once

#include "Order.hpp"
#include <sstream>
#include <string>
#include <vector>
namespace lob {
    struct Report{
        OrderId id;
        Price price;
        Quantity quantity;
        bool fully_filled;

         // Convert Report to string
        std::string to_string() const {
            std::ostringstream oss;
            oss << "{id: " << id
                << ", price: " << price
                << ", quantity: " << quantity
                << ", fully_filled: " << (fully_filled ? "true" : "false")
                << "}";
            return oss.str();
        }
    } ;

    struct ExecutionResult{
        Quantity fulfilled;
        std::vector<Report> reports;
        
        // Convert ExecutionResult to string
        std::string to_string() const {
            std::ostringstream oss;
            oss << "ExecutionResult: \n{ \n\tfulfilled: " << fulfilled
                << ", \n\treports: [\n\t\t";

            for (size_t i = 0; i < reports.size(); ++i) {
                oss << reports[i].to_string();
                if (i + 1 < reports.size()) {
                    oss << ", \n\t\t";
                }
            }
            oss << "\n\t] \n}";
            return oss.str();
        }
    };
}