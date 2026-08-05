#include "Book.hpp"
#include "Order.hpp"
#include "gtest/gtest.h"
#include <gtest/gtest.h>

class BkTest : public testing::Test{
    protected:
    lob::Book bk{"Stock"};
    lob::Order ord2{2, 1001, 300, lob::Side::Buy};
    lob::Order ord3{3, 1002, 50, lob::Side::Buy};
    lob::Order ord4{4, 1002, 300, lob::Side::Buy};
    lob::Order ord5{5, 1004, 20, lob::Side::Sell};
    lob::Order ord6{6, 1004, 80, lob::Side::Sell};
    lob::Order ord7{7, 1005, 70, lob::Side::Sell};
    lob::Order ord8{8, 1005, 60, lob::Side::Sell};

};

TEST(BookTest, BookSetup){
    lob::Book bk{"Stock"};
    auto add = bk.add(1, 1001, 300, lob::Side::Sell);

    EXPECT_EQ(bk.symbol(), "Stock");
    EXPECT_EQ(add.size(), 0);

    add = bk.add(1, 1001, 300, lob::Side::Buy);
    EXPECT_EQ(add[0].fulfilled, 300);
    EXPECT_EQ(add[0].reports[0].quantity, 300);
    EXPECT_TRUE(add[0].reports[0].fully_filled);
}
