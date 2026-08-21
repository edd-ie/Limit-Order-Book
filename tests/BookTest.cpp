#include "Book.hpp"
#include "Report.hpp"
#include "Units.hpp"
#include "gtest/gtest.h"
#include <gtest/gtest.h>


class BookTest : public testing::Test{
    protected:
        lob::Book book{"Stock"};

    void SetUp() override{
        auto report = book.add(2, 1001, 300, lob::Side::Buy);
        
        report = book.add(3, 1002, 50, lob::Side::Buy);
        report = book.add(4, 1002, 300, lob::Side::Buy);
        
        report = book.add(5, 1004, 20, lob::Side::Sell);
        report = book.add(6, 1004, 80, lob::Side::Sell);
        
        report = book.add(7, 1005, 70, lob::Side::Sell);
        report = book.add(8, 1005, 60, lob::Side::Sell);
    }

    void TearDown() override{
        auto v = book.check_invariants(); 
        EXPECT_FALSE(v.has_value()) << v.value_or("");
    }

};

TEST_F(BookTest, RestingAdd){
    EXPECT_EQ(book.best_ask(), 1004);

    auto report = book.add(9, 1006, 100, lob::Side::Sell);
    EXPECT_EQ(report.size(), 0);
    EXPECT_EQ(book.ask_levels(), 3);
    EXPECT_EQ(book.best_ask(), 1004);

    report = book.add(10, 1002, 10, lob::Side::Buy);
    EXPECT_EQ(book.level_quantity(lob::Side::Buy, 1002), 360);
}

TEST_F(BookTest, FullFill){
    auto report = book.add(9, 1004, 20, lob::Side::Buy);
    
    EXPECT_EQ(report.size(), 1);
    EXPECT_EQ(report.begin()->fulfilled, 20);
    EXPECT_TRUE(book.contains(6));
    EXPECT_FALSE(book.contains(5));
    EXPECT_FALSE(book.contains(9));
    EXPECT_EQ(book.level_quantity(lob::Side::Sell, 1004), 80);
    EXPECT_EQ(book.best_ask(), 1004);
}

TEST_F(BookTest, PartialFill){
    auto report = book.add(9, 1004, 10, lob::Side::Buy);
    
    EXPECT_EQ(report.size(), 1);
    EXPECT_EQ(report.begin()->fulfilled, 10);
    EXPECT_EQ(report.begin()->reports.size(), 1);

    EXPECT_TRUE(book.contains(6));
    EXPECT_TRUE(book.contains(5));
    EXPECT_FALSE(book.contains(9));

    EXPECT_EQ(book.resting_quantity(5), 10);
    EXPECT_EQ(book.level_quantity(lob::Side::Sell, 1004), 90);
    EXPECT_EQ(book.best_ask(), 1004);
}

TEST_F(BookTest, LevelConsumption){
    auto report = book.add(9, 1004, 100, lob::Side::Buy);
    
    EXPECT_EQ(report.size(), 1);
    EXPECT_EQ(report.begin()->fulfilled, 100);
    EXPECT_EQ(report.begin()->reports.size(), 2);
    
    EXPECT_FALSE(book.contains(6));
    EXPECT_FALSE(book.contains(5));
    EXPECT_FALSE(book.contains(9));
    
    EXPECT_EQ(book.ask_levels(), 1);
    EXPECT_EQ(book.best_ask(), 1005);
}

TEST_F(BookTest, BuyWalksAsks){
    auto report = book.add(9, 1005, 200, lob::Side::Buy);
    
    EXPECT_EQ(report.size(), 2);
    EXPECT_EQ(report.begin()->fulfilled, 100);
    EXPECT_EQ(report.at(1).fulfilled, 100);
    
    EXPECT_FALSE(book.contains(6));
    EXPECT_FALSE(book.contains(5));
    EXPECT_FALSE(book.contains(9));
    EXPECT_FALSE(book.contains(7));
    EXPECT_TRUE(book.contains(8));

    EXPECT_EQ(book.ask_levels(), 1);
    EXPECT_EQ(book.best_ask(), 1005);
    EXPECT_EQ(book.level_quantity(lob::Side::Sell, 1005), 30);

    report = book.add(9, 1005, 100, lob::Side::Buy);
    EXPECT_EQ(report.begin()->fulfilled, 30);
    EXPECT_FALSE(book.best_ask());
    EXPECT_FALSE(book.contains(8));
    EXPECT_TRUE(book.contains(9));
    EXPECT_EQ(book.resting_quantity(9), 70);
    EXPECT_EQ(book.best_bid(), 1005);
}

TEST_F(BookTest, SellWalksBids){
    auto report = book.add(9, 1001, 400, lob::Side::Sell);
    
    EXPECT_EQ(report.size(), 2);
    EXPECT_EQ(report.begin()->fulfilled, 350);
    EXPECT_EQ(report[1].fulfilled, 50);
    
    EXPECT_EQ(book.level_quantity(lob::Side::Buy, 1001) , 250);
    EXPECT_FALSE(report[1].reports.begin()->fully_filled);
    EXPECT_TRUE(book.best_ask());
    EXPECT_FALSE(book.contains(3));
    EXPECT_FALSE(book.contains(4));
    EXPECT_TRUE(book.contains(2));
    EXPECT_FALSE(book.contains(9));
    EXPECT_EQ(book.best_bid(), 1001);
}

TEST_F(BookTest, LimitStop){
    auto report = book.add(9, 1004, 500, lob::Side::Buy);

    EXPECT_EQ(report.begin()->fulfilled, 100);
    EXPECT_TRUE(book.best_ask());
    EXPECT_FALSE(book.contains(5));
    EXPECT_FALSE(book.contains(6));
    EXPECT_TRUE(book.contains(7));
    EXPECT_TRUE(book.contains(9));
    EXPECT_EQ(book.resting_quantity(9), 400);
    EXPECT_EQ(book.best_bid(), 1004);
}


TEST_F(BookTest, CancelFamily){
    auto report = book.cancel(3);

    EXPECT_EQ(report->id, 3);
    EXPECT_EQ(report->price, 1002);
    EXPECT_EQ(report->quantity, 50);
    EXPECT_TRUE(report->cancelled);
    EXPECT_FALSE(book.contains(3));
    EXPECT_EQ(book.best_bid(), 1002);

    report = book.cancel(2);
    EXPECT_FALSE(book.contains(2));
    EXPECT_EQ(book.bid_levels(), 1);
    
    EXPECT_EQ(book.best_bid(), 1002);
    EXPECT_EQ(book.best_ask(), 1004);
    
    auto v = book.check_invariants(); 
    EXPECT_FALSE(v.has_value()) << v.value_or("");

    auto best_bid = book.best_bid();
    auto best_ask = book.best_ask();
    auto bid_size = book.bid_levels();
    auto ask_size = book.ask_levels();
    report = book.cancel(99);
    EXPECT_FALSE(report);
    
    v = book.check_invariants(); 
    EXPECT_FALSE(v.has_value()) << v.value_or("");

    EXPECT_EQ(book.best_bid(), best_bid);
    EXPECT_EQ(book.best_ask(), best_ask);
    EXPECT_EQ(book.bid_levels(), bid_size);
    EXPECT_EQ(book.ask_levels(), ask_size);
}

TEST_F(BookTest, Tombstone){
    auto report = book.cancel(5);
    EXPECT_EQ(report->id, 5);
    EXPECT_FALSE(book.contains(5));
    EXPECT_EQ(book.ask_levels(), 2);
    
    auto v = book.check_invariants(); 
    EXPECT_FALSE(v.has_value()) << v.value_or("");

    auto report2 = book.add(9, 1004, 80, lob::Side::Buy);

    EXPECT_EQ(report2.begin()->fulfilled, 80);
    EXPECT_EQ(book.best_ask(), 1005);
    EXPECT_EQ(book.ask_levels(), 1);
    EXPECT_FALSE(book.contains(6));
    EXPECT_EQ(report2[0].reports.size(), 1);
    EXPECT_EQ(report2[0].reports[0].id, 6);
}

TEST_F(BookTest, CancelKillsLevel_ThenAddRests){
    auto report = book.cancel(5);
    report = book.cancel(6);
    EXPECT_EQ(book.best_ask(), 1005);

    auto report2 = book.add(9, 1004, 50, lob::Side::Buy);
    EXPECT_EQ(report2.size(), 0);
    EXPECT_EQ(book.best_bid(), 1004);
    EXPECT_EQ(book.best_ask(), 1005);
}

TEST_F(BookTest, DuplicateIds){
    std::vector<lob::ExecutionReport> x;
    EXPECT_DEBUG_DEATH(x = book.add(3, 1005, 10, lob::Side::Sell), ".*");
}

TEST_F(BookTest, CancelAfterFill){
    auto x=book.add(9, 1004, 20, lob::Side::Buy );
    EXPECT_TRUE(x.begin()->reports.begin()->fully_filled);
    EXPECT_FALSE(book.cancel(5));

    auto x1 = book.cancel(3);
    EXPECT_FALSE(book.contains(3));
    EXPECT_FALSE(book.cancel(3));
}





