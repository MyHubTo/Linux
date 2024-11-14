#include <iostream>
#include <string>
#include <gtest/gtest.h>
#include "video/video.h"
using namespace std;
TEST(video,test1){
    Video video;
    EXPECT_EQ(video.getNumber(),0);
    EXPECT_FALSE(video.isRunning());
    EXPECT_STREQ(video.getName().c_str(),string().c_str());
}

int main(int argc,char *argv[]){
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
    return 0;
}