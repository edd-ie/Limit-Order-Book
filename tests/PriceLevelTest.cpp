#include "gtest/gtest.h"
#include <algorithm>
#include <gtest/gtest.h>
#include <print>
#include <string>
#include <utility>
#include "PriceLevel.hpp"
#include "Report.hpp"

TEST(PriceLevelTest, WrongPriceLevel){
    lob::Order order{1, 1001, 50, lob::Side::Buy};
    lob::PriceLevel pLvl{1002};

    EXPECT_DEATH(pLvl.push(std::move(order)), ".*");
}

TEST(PriceLevelTest, AddOrders){
    lob::Order order{1, 1001, 50, lob::Side::Buy};
    lob::Order order2{2, 1001, 300, lob::Side::Buy};
    lob::PriceLevel pLvl{1001};

    pLvl.push(std::move(order));
    pLvl.push(std::move(order2));

    auto& front = pLvl.peek();
    EXPECT_EQ(front.id(), 1);
    EXPECT_EQ(pLvl.price(), 1001);
    EXPECT_EQ(pLvl.quantity(), 350);
}

TEST(PriceLevelTest, ExactExecution){
    lob::Order order{1, 1001, 50, lob::Side::Buy};
    lob::Order order2{2, 1001, 300, lob::Side::Buy};
    lob::PriceLevel pLvl{1001};

    pLvl.push(std::move(order));
    pLvl.push(std::move(order2));

    pLvl.execute(350);
    EXPECT_EQ(pLvl.quantity(), 0);
    EXPECT_TRUE(pLvl.empty_queue());
    EXPECT_TRUE(pLvl.is_exhausted());
}

TEST(PriceLevelTest, UnderExecution){
    lob::Order order{1, 1001, 50, lob::Side::Buy};
    lob::Order order2{2, 1001, 300, lob::Side::Buy};
    lob::PriceLevel pLvl{1001};

    pLvl.push(std::move(order));
    pLvl.push(std::move(order2));

    pLvl.execute(100);
    EXPECT_EQ(pLvl.quantity(), 250);
    EXPECT_FALSE(pLvl.empty_queue());
    EXPECT_FALSE(pLvl.is_exhausted());
    EXPECT_EQ(pLvl.peek().quantity(), 250);
    EXPECT_EQ(pLvl.peek().id(), 2);
}

TEST(PriceLevelTest, OverExecution){
    lob::Order order{1, 1001, 50, lob::Side::Buy};
    lob::Order order2{2, 1001, 300, lob::Side::Buy};
    lob::PriceLevel pLvl{1001};

    pLvl.push(std::move(order));
    pLvl.push(std::move(order2));

    auto result = pLvl.execute(400);
    EXPECT_EQ(pLvl.quantity(), 0);
    EXPECT_EQ(pLvl.price(), 1001);
    EXPECT_TRUE(pLvl.empty_queue());
    EXPECT_TRUE(pLvl.is_exhausted());

    EXPECT_EQ(result.fulfilled, 350);
    EXPECT_EQ(result.reports.size(), 2);
}


TEST(PriceLevelTest, MoveSemantics){
    lob::Order order{1, 1001, 50, lob::Side::Buy};
    lob::Order order2{2, 1001, 300, lob::Side::Buy};
    lob::PriceLevel pLvl{1001};

    pLvl.push(std::move(order));
    pLvl.push(std::move(order2));

    auto pLvl1{std::move(pLvl)};

    auto& front = pLvl1.peek();
    EXPECT_EQ(front.id(), 1);
    EXPECT_EQ(pLvl1.price(), 1001);
    EXPECT_EQ(pLvl1.quantity(), 350);
    
    auto pLvl2 = std::move(pLvl1);

    auto& front2 = pLvl2.peek();
    EXPECT_EQ(front2.id(), 1);
    EXPECT_EQ(pLvl2.price(), 1001);
    EXPECT_EQ(pLvl2.quantity(), 350);
}

TEST(PriceLevelTest, CancelOrder){
    lob::Order order{1, 1001, 50, lob::Side::Buy};
    lob::Order order2{2, 1001, 300, lob::Side::Buy};
    lob::PriceLevel pLvl{1001};

    auto& exe = pLvl.push(std::move(order));
    pLvl.push(std::move(order2));

    auto& front = pLvl.peek();
    EXPECT_EQ(front.id(), 1);
    EXPECT_EQ(pLvl.price(), 1001);
    EXPECT_EQ(pLvl.quantity(), 350);
    
    EXPECT_EQ(pLvl.cancel(exe), 50);
    EXPECT_TRUE(exe.is_filled());

    auto& front2 = pLvl.peek();
    EXPECT_EQ(front2.id(), 1);
    EXPECT_EQ(pLvl.quantity(), 300);

    pLvl.execute(100);
    EXPECT_EQ(pLvl.quantity(), 200);
    EXPECT_FALSE(pLvl.empty_queue());
    EXPECT_FALSE(pLvl.is_exhausted());
    EXPECT_EQ(pLvl.peek().quantity(), 200);
    EXPECT_EQ(pLvl.peek().id(), 2);
}