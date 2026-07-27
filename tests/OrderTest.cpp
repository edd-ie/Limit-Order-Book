#include <utility>
#include <gtest/gtest.h>
#include "Order.hpp"

TEST(OrderTest, Initialization) {
    lob::Order order{12,100, 50, lob::Side::Buy};
    
    EXPECT_EQ(order.id(), 12);
    EXPECT_EQ(order.price(), 100);
    EXPECT_EQ(order.quantity(), 50);
    EXPECT_EQ(order.side(), lob::Side::Buy);
    EXPECT_FALSE(order.is_filled());
}

TEST(OrderTest, QuantityReduction) {
    lob::Order order{13, 100, 50, lob::Side::Buy};
    
    
    EXPECT_EQ(order.fill(20), 20);
    EXPECT_EQ(order.quantity(), 30);
    EXPECT_FALSE(order.is_filled());

    EXPECT_EQ(order.fill(30), 30);
    EXPECT_EQ(order.quantity(), 0);
    EXPECT_TRUE(order.is_filled());
}

// Test exact full fill
TEST(OrderTest, FillExactQuantity) {
    lob::Order order{12, 100, 50, lob::Side::Buy};
    
    EXPECT_EQ(order.fill(50), 50);
    EXPECT_EQ(order.quantity(), 0);
    EXPECT_TRUE(order.is_filled());
}

// Test fill with 0 amount (edge case)
TEST(OrderTest, FillZeroQuantity) {
    lob::Order order{12, 100, 50, lob::Side::Buy};
    
    EXPECT_EQ(order.fill(0), 0);
    EXPECT_EQ(order.quantity(), 50);
}


// Test fill more than available quantity
TEST(OrderTest, FillOverQuantity) {
    lob::Order order{12, 100, 50, lob::Side::Buy};
    
    EXPECT_EQ(order.fill(100), 50);
    EXPECT_EQ(order.quantity(), 0);
}

// Test Order cancellation
TEST(OrderTest, CancelOrder) {
    lob::Order order{12, 100, 50, lob::Side::Buy};
    
    EXPECT_EQ(order.cancel(), 50);
    EXPECT_TRUE(order.is_filled());
}

TEST(OrderTest,MoveSemantics){
    lob::Order order{12, 100, 50, lob::Side::Buy};
    auto newOrder = std::move(order);
    
    EXPECT_EQ(newOrder.id(), 12);
    EXPECT_EQ(newOrder.price(), 100);
    EXPECT_EQ(newOrder.quantity(), 50);
    EXPECT_EQ(newOrder.side(), lob::Side::Buy);

    lob::Order order2{13, 130, 50, lob::Side::Sell};
    newOrder = std::move(order2);
    
    EXPECT_EQ(newOrder.id(), 13);
    EXPECT_EQ(newOrder.price(), 130);
    EXPECT_EQ(newOrder.side(), lob::Side::Sell);
}