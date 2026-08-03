#include "gtest/gtest.h"
#include <gtest/gtest.h>
#include <utility>
#include "PriceLevel.hpp"
#include "Report.hpp"

class LevelTest : public testing::Test{
    protected:
    lob::PriceLevel p1{1001};
    lob::PriceLevel p2{1002};

    void SetUp() override{
        p1.push(lob::Order {1, 1001, 50, lob::Side::Buy});
        p1.push(lob::Order {2, 1001, 300, lob::Side::Buy});
        
        p2.push(lob::Order {3, 1002, 50, lob::Side::Buy});
        p2.push(lob::Order{4, 1002, 300, lob::Side::Buy});
    }

};


TEST_F(LevelTest, WrongPriceLevel){
    lob::Order order{1, 1001, 50, lob::Side::Buy};
    
    EXPECT_DEBUG_DEATH(p2.push(std::move(order)), ".*");
}

TEST_F(LevelTest, AddOrders){

    auto& front = p1.peek();
    EXPECT_EQ(front.id(), 1);
    EXPECT_EQ(p1.price(), 1001);
    EXPECT_EQ(p1.quantity(), 350);
}

TEST_F(LevelTest, ExactExecution){
    auto result = p1.execute(350);

    EXPECT_EQ(result.fulfilled, 350);
    EXPECT_EQ(result.reports.size(), 2);
    EXPECT_EQ(result.reports[0].id, 1);
    EXPECT_EQ(result.reports[0].quantity, 50);
    EXPECT_TRUE(result.reports[0].fully_filled);
    EXPECT_EQ(result.reports[1].id, 2);
    EXPECT_EQ(result.reports[1].quantity, 300);
    EXPECT_TRUE(result.reports[1].fully_filled);
    
    EXPECT_EQ(p1.quantity(), 0);
    EXPECT_TRUE(p1.empty_queue());
    EXPECT_TRUE(p1.is_exhausted());

}

TEST_F(LevelTest, UnderExecution){
    auto result = p1.execute(100);

    EXPECT_EQ(result.fulfilled, 100);
    EXPECT_EQ(result.reports.size(), 2);
    EXPECT_EQ(result.reports[0].id, 1);
    EXPECT_EQ(result.reports[0].quantity, 50);
    EXPECT_TRUE(result.reports[0].fully_filled);
    EXPECT_EQ(result.reports[1].id, 2);
    EXPECT_EQ(result.reports[1].quantity, 50);
    EXPECT_FALSE(result.reports[1].fully_filled);

    EXPECT_EQ(p1.quantity(), 250);
    EXPECT_FALSE(p1.empty_queue());
    EXPECT_FALSE(p1.is_exhausted());
    EXPECT_EQ(p1.peek().quantity(), 250);
    EXPECT_EQ(p1.peek().id(), 2);
}

TEST_F(LevelTest, OverExecution){
    auto result = p1.execute(400);

    EXPECT_EQ(result.fulfilled, 350);
    EXPECT_EQ(result.reports.size(), 2);
    EXPECT_EQ(result.reports[0].id, 1);
    EXPECT_EQ(result.reports[0].quantity, 50);
    EXPECT_TRUE(result.reports[0].fully_filled);
    EXPECT_EQ(result.reports[1].id, 2);
    EXPECT_EQ(result.reports[1].quantity, 300);
    EXPECT_TRUE(result.reports[1].fully_filled);

    EXPECT_EQ(p1.quantity(), 0);
    EXPECT_EQ(p1.price(), 1001);
    EXPECT_TRUE(p1.empty_queue());
    EXPECT_TRUE(p1.is_exhausted());
}


TEST_F(LevelTest, MoveSemantics){

    auto pLvl1{std::move(p1)};

    auto& front = pLvl1.peek();
    EXPECT_EQ(front.id(), 1);
    EXPECT_EQ(pLvl1.price(), 1001);
    EXPECT_EQ(pLvl1.quantity(), 350);
    
    p2 = std::move(pLvl1);

    auto& front2 = p2.peek();
    EXPECT_EQ(front2.id(), 1);
    EXPECT_EQ(p2.price(), 1001);
    EXPECT_EQ(p2.quantity(), 350);
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

    auto report = pLvl.execute(100);
    EXPECT_EQ(pLvl.quantity(), 200);
    EXPECT_FALSE(pLvl.empty_queue());
    EXPECT_FALSE(pLvl.is_exhausted());
    EXPECT_EQ(pLvl.peek().quantity(), 200);
    EXPECT_EQ(pLvl.peek().id(), 2);
}

TEST(PriceLevelTest, TrailingCancel){
    lob::Order order{1, 1001, 50, lob::Side::Buy};
    lob::Order order2{2, 1001, 300, lob::Side::Buy};
    lob::PriceLevel pLvl{1001};

    pLvl.push(std::move(order));
    auto& exe = pLvl.push(std::move(order2));

    pLvl.cancel(exe);
    EXPECT_EQ(pLvl.quantity(), 50);

    auto result = pLvl.execute(50);
    EXPECT_EQ(result.fulfilled, 50);
    EXPECT_EQ(result.reports.size(), 1);
    EXPECT_EQ(result.reports[0].id, 1);
    EXPECT_TRUE(pLvl.empty_queue());
    EXPECT_TRUE(pLvl.is_exhausted());

    result = pLvl.execute(50);
    EXPECT_EQ(result.fulfilled, 0);
    EXPECT_EQ(result.reports.size(), 0);
    EXPECT_TRUE(pLvl.empty_queue());
    EXPECT_TRUE(pLvl.is_exhausted());
}