#include "Book.hpp"
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

TEST_F(BkTest, BookSetup){
    
}
