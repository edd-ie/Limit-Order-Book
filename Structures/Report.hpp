#pragma once

#include "Units.hpp"
#include <sstream>
#include <string>
#include <vector>
namespace lob {
    struct OrderReport{
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

    struct CancelReport{
        OrderId id;
        Price price;
        Quantity quantity;

         // Convert CancelReport to string
        std::string to_string() const {
            std::ostringstream oss;
            oss << "{id: " << id
                << ", price: " << price
                << ", quantity: " << quantity
                << "}";
            return oss.str();
        }
    } ;

    struct ExecutionReport{
        Quantity fulfilled;
        std::vector<OrderReport> reports;
        
        // Convert ExecutionReport to string
        std::string to_string() const {
            std::ostringstream oss;
            oss << "ExecutionReport: \n{ \n\tfulfilled: " << fulfilled
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