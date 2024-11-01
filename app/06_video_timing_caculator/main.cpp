#include "VideoTimingCalculator.h"
#include <iostream>
#include <vector>
#ifdef FMT_OUTPUT
#include <fmt/core.h>
#include <fmt/color.h>
using namespace fmt;
#endif
using namespace std;
void Usage(){
    std::cout<<"Usage:./demo 640 480 60 8 3 cvt_rb 0 0 0 \n"\
                << "\t h_pixs v_pixs rate(Hz) bpc(5/6/8/10/12/16) color_fmt_multiplier(3/2/1.5) \n" \
                << "\t reduced_blanking(cvt/cvt_rb/cvt_rb2) margins(0/1) interlaced(0/1)  video_optimized(0/1)"<<std::endl;
}
int main(int argc ,char *argv[])
{
    VideoTimingCaculator caculator;
    uint32_t h_pixs;
    uint32_t v_pixs;
    uint32_t rate;
    uint32_t bpc;
    double   color_fmt_multiplier;
    std::string reduced_blanking;
    bool margins;
    bool interlaced;
    bool video_optimized;
    std::vector<std::string> standards={"cvt","cvt_rb","cvt_rb2","cea"};
    if(argc==3){
        h_pixs                  = atoi(argv[1]);
        v_pixs                  = atoi(argv[2]);
        for(auto it:standards){
            caculator.calculateCvt(h_pixs,v_pixs,60,8,3,it);
            caculator.dump();
        }
    }else if(argc>3&& argc<10){
        h_pixs                  = atoi(argv[1]);
        v_pixs                  = atoi(argv[2]);
        rate                    = atoi(argv[3]);
        bpc                     = atoi(argv[4]);
        color_fmt_multiplier    = atof(argv[5]);
        //reduced_blanking        = std::string(argv[6]);
        margins                 = atoi(argv[7]);
        interlaced              = atoi(argv[8]);
        video_optimized         = atoi(argv[9]);
        for(auto it:standards){
            caculator.calculateCvt(h_pixs,v_pixs,rate,bpc,color_fmt_multiplier,reduced_blanking,margins,interlaced,video_optimized);
            caculator.dump();
        }
    }else{
        Usage();
    }
    return 0;
}
