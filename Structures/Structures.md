# Structures

## Order

Instruction to buy or sell a set quantity of an asset. It is composed of:

- `side` - buy/sell (_Enum_)
- `price` - value of the asset (_Uint_)
- `quantity` - amount of the asset.
- `OrderID` - unique identifier.

### Operations

`fill(quantity)` - fullfil a request of a specified quantity of the order.
`cancel()` - exhaust the order by making its quantity 0.

## Price Level

A _FIFO_ queue of orders of the same `price`, composition:

- `price` - value of the orders (_Uint_)
- `quantity` - total amount of the orders in the queue.
- `queue` - a collection of orders with priority given to order of insertion.

### Operations

1. `push(order)`

- add an order of the same price to the back of the queue and increase the level quantity by the order's quantity.

2. `peek()`

- read only view of the order at the front of the queue.

3.  `execute(quantity)`

- Fullfil a requested amount of an order and return the `ExecutionReport`. Execution is by priority, orders from the front of the queue as filled first.

**Fully Filled** - when the order is exhausted it is marked as exhausted on the report and removed from the queue, while the request is not yet fully fulfilled, the next order in the queue is filled.
**Partially Filled** - when a request is fulfilled but the current order being executed is not exhausted, the order will remain at the front of the queue and report marks the order as partially filled but the request is fully filled.

When the level is exhausted, no orders remain, but the the request is not yet completed, the amount serviced will be in the report together with a flag on the fulfillment status.

4. `cancel(order reference)`

- As `std::deque` has _O(n)_ deletion of elements, a tombstone approach was take to try and achieve a _O(1)_ deletion (_ver:0_). On a cancel request, the order is referenced and marked as exhausted, the price level quantity reduced but the order quantity but the order is not removed from the queue. The order will eventually be popped from the queue during an execute request or if the price level's quantity gets to zero and the queue is cleared.

## Book

Creates orders, own and manages the id-map and price levels.

- `symbol` - representation of the asset being traded.
- `id map` - an unsorted hashmap of live `OrderPtr`s referencing the order and price level indexed by order id.
- `buy` - a sorted map of price levels of bids ordered in descending Prices
- `sell` - a sorted map of price levels of asks ordered in ascending Prices

### Operations

1. `add(id, price, quantity, side)`

Core invariant: an order only ever rests because it couldn't be traded; _i.e. a pricelevel of 1001 cannot be created in buy while sell has a pricelevel of 1001_.

- On an incoming request, when the best of the opposing side crosses (_best ask <= price (sell crosses) or best bid >= price (buy crosses)_), `execute()` is called on the best level and subtracts the fulfilled from the quantity.
- Reports are appended for the trades accumulated, for each report stating the an order was exhausted the order is erased from the id map and finally if a pricelevel's quantity equals 0, the price level is erased from the map.
- If the execution results in a remainder a new order will be created, added to the end of the appropriate price level and the order & pricelevel addresses used to create an `OrderPtr` that will be added to the id-map.

2. `cancel(order id)`

- Orders with quantity equal to zero are marked as cancelled, the order is fetched from the id map, the price level associated with it has its quantity decrease by the order's quantity then the order quantity is set to zero and the order entry is deleted from the id-map. During execution, the order will be removed from the pricelevel's queue. Tombstoning / lazy deletion (ver:0) was used to try and achieve _O(1)_ cancellation

## Testing

The project utilizes _google test suite_ to create unit test to ensure the implementation works as intended. The book object employs an invariant checker to ensure:

1. Map validity - every order in the id map is not exhausted and contained in the correct price level. It also checks that every live order in a price level is in the id map.
2. Level validity - Checks best bid is less than best ask and that no price level is exhausted.
