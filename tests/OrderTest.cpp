#include "gtest/gtest.h"
#include <gtest/gtest.h>
#include "Order.hpp"

TEST(OrderTest, Initialization) {
    lob::Order order{12,100, 50, lob::Side::Buy};
    
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



