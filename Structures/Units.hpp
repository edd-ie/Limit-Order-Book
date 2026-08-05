#pragma once
#include <cstdint>

namespace lob {
    enum class Side{
        Buy = 0,
        Sell = 1
    };

    using OrderId = std::uint64_t;
    using Price = std::uint32_t;
    using Quantity = std::uint32_t;
}
    