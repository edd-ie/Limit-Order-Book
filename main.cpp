#include "Book.hpp"
#include "Report.hpp"
#include <print>
#include <string_view>

// Helper to log execution vectors directly
void print_executions(std::string_view label, const std::vector<lob::ExecutionReport>& executions) {
    std::println("\n>>> Action: {} <<<", label);
    if (executions.empty()) {
        std::println("    [Resting] No immediate executions. Order added to book.");
        return;
    }
    for (size_t i = 0; i < executions.size(); ++i) {
        std::println("  Execution Level #{}:\n{}", i + 1, executions[i]);
    }
}

// Helper to print Top-of-Book state using std::println and std::optional formatting
void print_book_snapshot(const lob::Book& book) {
    auto best_bid = book.best_bid();
    auto best_ask = book.best_ask();

    std::println("  [Book Snapshot] Best Bid: {:>5} | Best Ask: {:>5} | Bid Levels: {} | Ask Levels: {}",
                 best_bid ? std::format("{}", *best_bid) : "NONE",
                 best_ask ? std::format("{}", *best_ask) : "NONE",
                 book.bid_levels(), 
                 book.ask_levels());
}

int main() {
    std::println("==================================================");
    std::println("    Limit Order Book (LOB) Demonstration Driver    ");
    std::println("==================================================\n");

    lob::Book book{"AAPL"};

    // 1. Seed Passive Orders
    std::println("1. Seeding liquidity into the order book...");
    
    // Bids (Buy side)
    auto report = book.add(101, 150, 100, lob::Side::Buy);  // Order 101: Buy 100 @ 150
    report = book.add(102, 149, 200, lob::Side::Buy);  // Order 102: Buy 200 @ 149

    // Asks (Sell side)
    report = book.add(201, 152, 50,  lob::Side::Sell); // Order 201: Sell 50 @ 152
    report = book.add(202, 152, 100, lob::Side::Sell); // Order 202: Sell 100 @ 152
    report = book.add(203, 155, 300, lob::Side::Sell); // Order 203: Sell 300 @ 155

    print_book_snapshot(book);

    // 2. Partial Fill Execution
    auto exec1 = book.add(301, 152, 30, lob::Side::Buy);
    print_executions("Aggressive Buy (Qty 30 @ 152)", exec1);
    print_book_snapshot(book);

    // 3. Multi-Level Sweep
    auto exec2 = book.add(302, 155, 150, lob::Side::Buy);
    print_executions("Aggressive Buy Sweeping 152 & 155 Levels (Qty 150 @ 155)", exec2);
    print_book_snapshot(book);

    // 4. Order Cancellation (Prints CancelReport directly using C++23 formatter)
    std::println("\n>>> Action: Cancelling Resting Order ID 101 @ 150 <<<");
    if (auto cancel_report = book.cancel(101); cancel_report) {
        std::println("    Result: {}", *cancel_report);
    } else {
        std::println("    Order not found for cancellation.");
    }
    print_book_snapshot(book);

    // 5. Cross the Spread and Rest Remainder
    auto exec3 = book.add(303, 148, 500, lob::Side::Sell);
    print_executions("Large Aggressive Sell (Qty 500 @ 148)", exec3);
    print_book_snapshot(book);

    // 6. Validate LOB Invariants
    std::println("\n>>> Action: Verifying Order Book Invariants <<<");
    if (auto error = book.check_invariants(); error) {
        std::println("    [FAIL] Invariant Check Error: {}", *error);
    } else {
        std::println("    [PASS] All internal invariants verified successfully.");
    }

    std::println("\n==================================================");
    std::println("                Demo Completed                    ");
    std::println("==================================================");

    return 0;
}