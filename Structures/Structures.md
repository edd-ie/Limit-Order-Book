# Structures

This document tries to capture some of the design decision made in creating the `Limit Order Book` data-structures

All data structures are fall under the `lob` namespace.

## Custom numeric data-types

- Currency values are represented as integer ticks to avoid floating point calculations.
- The default integer type is `std::uint32_t` for prices and quantities and `std::uint64_t` for id numbers specifically order ids

1. `Price`
2. `Quantity`
3. `OrderId`

## Order

Class members:

```c++
OrderId id_;
Price price_;
Quantity quantity_;
Side side_;
```

Every order is unique and thus cannot be copied, the only mutable member in the order class is `quantity_`.

Accessors:

```c++
OrderId id()
Price price()
Quantity quantity()
Side side()
bool is_filled()
```

Filling an order, an amount is requested if the amount is below the available quantity, requested amount is removed from the available and the amount fulfilled is returned. If the requested amount is `>=` the current quantity, the quantity is zeroed and the amount fulfilled is returned.

- **clamping semantics**: over-fill is legal, returns actual filled

## PriceLevel

Class members:

```c++
Price price_;
Quantity quantity_;
std::deque<Order> orders_{};
```
