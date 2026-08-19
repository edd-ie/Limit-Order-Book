#pragma once

#include "Units.hpp"
#include <format>
#include <string>
#include <vector>
#include <ostream>
namespace lob {
    struct OrderReport{
        OrderId id;
        Price price;
        Quantity quantity;
        bool fully_filled;

         // Convert OrderReport to string
        std::string to_string() const {
            return std::format("[id: {}, price: {}, quantity: {}, fully_filled: {}]", id, price, quantity, fully_filled);
        }
    } ;

    struct CancelReport{
        OrderId id{0};
        Price price{0};
        Quantity quantity{0};
        bool cancelled{false};

         // Convert CancelReport to string
        std::string to_string() const {
            return std::format("[id: {}, price: {}, quantity: {}, cancelled: {}]", id, price, quantity, cancelled);;
        }

        friend void PrintTo(const CancelReport& report, std::ostream* os) {
            *os << report.to_string();
        }
    } ;

    struct ExecutionReport{
        Quantity fulfilled;
        std::vector<OrderReport> reports;
        
        // Convert ExecutionReport to string
        std::string to_string() const {

            std::string view = std::format("ExecutionReport: \n[ \n\tfulfilled: {}, \n\treports: [\n\t\t", fulfilled);

            for (size_t i = 0; i < reports.size(); ++i) {
                view += reports[i].to_string();
                if (i + 1 < reports.size()) {
                    view +=  ", \n\t\t";
                }
            }

            view +=  "\n\t] \n]";
            return view;
        }

        friend void PrintTo(const ExecutionReport& report, std::ostream* os) {
            *os << report.to_string();
        }
    };
}