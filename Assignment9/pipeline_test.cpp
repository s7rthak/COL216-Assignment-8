#include <gtest/gtest.h>
#include "pipeline.hpp"
#include <bits/stdc++.h>

using namespace std;

vector<int>reg(32);
vector<int>mem(100, 3);
vector<string>inst_mem;


TEST(MIPS, Stall){

}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
