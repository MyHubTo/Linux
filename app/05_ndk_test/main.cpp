#include "VideoTimingCalculator.h"
#include <iostream>
#include <vector>
#include <chrono>
#include <string>
#ifdef FMT_OUTPUT
#include <fmt/core.h>
#include <fmt/color.h>
using namespace fmt;
#endif
void Usage(){
    std::cout<<"Usage:./demo 640 480 60 8 3 cvt_rb 0 0 0 "\
                << "\t h_pixs v_pixs rate(Hz) bpc(5/6/8/10/12/16) color_fmt_multiplier(3/2/1.5)" \
                << "\t reduced_blanking(cvt/cvt_rb/cvt_rb2) margins(0/1) interlaced(0/1)  video_optimized(0/1)"<<std::endl;
}
using namespace std;
int main(int argc ,char *argv[])
{
    VideoTimingCaculator caculator;
    uint32_t h_pixs=800;
    uint32_t v_pixs=600;
    uint32_t rate;
    uint32_t bpc;
    double   color_fmt_multiplier;
    std::string reduced_blanking;
    bool margins;
    bool interlaced;
    bool video_optimized;
    auto start = std::chrono::steady_clock::now();
    std::cout<<"max rate:"<<caculator.getMaxVFreq(1920,1080)<<std::endl;
    auto finish = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
    std::cout << duration.count() << "ms" << std::endl;
    return 0;
}
