#pragma once

#include "Units.hpp"
#include <format>
#include <vector>

namespace lob {

struct OrderReport {
    OrderId id{0};
    Price price{0};
    Quantity quantity{0};
    bool fully_filled{false};
};

struct CancelReport {
    OrderId id{0};
    Price price{0};
    Quantity quantity{0};
    bool cancelled{false};
};

struct ExecutionReport {
    Quantity fulfilled{0};
    std::vector<OrderReport> reports{};
};

} // namespace lob

// ============================================================================
// Formatter Specializations
// ============================================================================

template <>
struct std::formatter<lob::OrderReport> : std::formatter<std::string_view> {
    auto format(const lob::OrderReport& report, format_context& ctx) const {
        return std::format_to(
            ctx.out(),
            "[id: {}, price: {}, quantity: {}, fully_filled: {}]",
            report.id, report.price, report.quantity, report.fully_filled
        );
    }
};

template <>
struct std::formatter<lob::CancelReport> : std::formatter<std::string_view> {
    auto format(const lob::CancelReport& report, format_context& ctx) const {
        return std::format_to(
            ctx.out(),
            "[id: {}, price: {}, quantity: {}, cancelled: {}]",
            report.id, report.price, report.quantity, report.cancelled
        );
    }
};

template <>
struct std::formatter<lob::ExecutionReport> : std::formatter<std::string_view> {
    auto format(const lob::ExecutionReport& exec, format_context& ctx) const {
        auto out = std::format_to(ctx.out(), "ExecutionReport: \n[ \n\tfulfilled: {}, \n\treports: [\n\t\t", exec.fulfilled);
        
        for (size_t i = 0; i < exec.reports.size(); ++i) {
            out = std::format_to(out, "{}", exec.reports[i]);
            if (i + 1 < exec.reports.size()) {
                out = std::format_to(out, ", \n\t\t");
            }
        }
        
        return std::format_to(out, "\n\t] \n]");
    }
};