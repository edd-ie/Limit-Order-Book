#pragma once

#include <chrono>
#include <cstdint>

enum class Side{
    Buy = 0,
    Sell = 1
};

using TimePoint = std::chrono::high_resolution_clock::time_point;
using uint64 = std::uint64_t;
using uint32 = std::uint32_t;

class Order{
    // Aliases
    

    // Members
    uint64 id_;
    uint32 price_;
    uint32 quantity_;
    TimePoint arrival_time_;
    Side side_;

    public:
        static inline uint64 next_id = 0;

        Order(uint32 price_in, uint32 quantity_in, Side side_in);
        ~Order() = default;

        // Copy        
        Order(const Order& order) = delete;
        Order& operator=(const Order&) = delete;

        // Move
        Order(Order&& order) noexcept = default;
        Order& operator=(Order&&) noexcept = default;

        // Operators
        bool operator==(const Order &other) const noexcept;
        bool operator<(const Order &other) const noexcept;
        bool operator>(const Order &other) const noexcept;
        
        Order& operator-=(const uint32 amount) noexcept;

        // Accessors
        [[nodiscard]] uint64 id() const noexcept;
        [[nodiscard]] uint32 price() const noexcept;
        [[nodiscard]] uint32 quantity() const noexcept;
        [[nodiscard]] TimePoint arrival_time() const noexcept;
        [[nodiscard]] Side side() const noexcept;
        [[nodiscard]] bool is_filled() const noexcept;
        
        // Mutator
        void update_quantity(const uint32 amount) noexcept;
        
};