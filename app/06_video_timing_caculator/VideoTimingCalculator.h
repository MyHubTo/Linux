#ifndef _VIDEO_TIMING_CACULATOR_H_
#define _VIDEO_TIMING_CACULATOR_H_
#include <iostream>
#include <string>
#include <math.h>
#include <fstream>
#include <cjson/cJSON.h>
#include "VLog.h"
#ifdef  FMT_OUTPUT
#include <fmt/core.h>
#include <fmt/color.h>
#endif
using namespace std;
#define     MAX_BUFFER_SIZE         4096
#define     DEFAULT_CONFIG_FILE     "/root/Linux/app/06_video_timing_caculator/config/vic_timings.json"
#define     STRINGIFY(x)            #x
#define     TOSTRING(x)             STRINGIFY(x)
struct timing_t{
    std::string type;
    double      pix_clock;          //MHz

    uint32_t    h_total;            //Pixels
    uint32_t    h_active;           //Pixels
    uint32_t    h_blank;            //Pixels
    uint32_t    h_front_porch;      //Pixels
    uint32_t    h_sync;             //Pixels
    uint32_t    h_back_porch;       //Pixels
    uint32_t    h_sync_polarity;    //'+':1 '-':0
    double      h_freq;             //KHz
    double      h_period;           //us

    uint32_t    v_total;            //Pixels
    uint32_t    v_active;           //Pixels
    uint32_t    v_blank;            //Pixels
    uint32_t    vblank_duration;    //us
    uint32_t    v_front_porch;      //Pixels
    uint32_t    v_sync;             //Pixels
    uint32_t    v_back_porch;       //Pixels
    uint32_t    v_sync_polarity;    //'+':1 '-':0
    double      v_freq;             //KHz
    double      v_period;           //ms

    uint32_t    peak_bw;            //Mbit/s
    uint32_t    line_bw;            //Mbit/s
    uint32_t    active_bw;          //Mbit/s

    string      mode_lines;
};

struct cea_timing_t{
    double   pix_clock;
    uint32_t h_total;
};

//TODO
// double color_fmt_multiplier = (color_fmt == "rgb444") ? 3 :
//                 (color_fmt == "yuv444") ? 3 :
//                 (color_fmt == "yuv422") ? 2 :
//                 (color_fmt == "yuv420") ? 1.5 : 0;

class VideoTimingCaculator{
public:
    VideoTimingCaculator();
    void addConfig();
    void calculateCvt(  uint32_t horiz_pixels,
                        uint32_t vert_pixels,
                        double refresh_rate=60.0,
                        uint32_t bpc=8,  //Bits per Component.(eg:5/6/8/10/12/16)
                        double color_fmt_multiplier=3,// Color Format.(eg:rgb444/yuv444:3,yuv422:2,yuv420:1.5,other:0)
                        std::string reduced_blanking="cvt_rb",
                        bool margins=false,
                        bool interlaced=false,
                        bool video_optimized=false);

    timing_t getCvtTiming();
    std::string getModeLine();
    timing_t lookupVicTiming(uint32_t horiz_pixels,uint32_t vert_pixels,double refresh_rate,uint32_t bpc,double color_fmt_multiplier,bool interlaced);
    void dump();
    ~VideoTimingCaculator();
private:
    timing_t m_timing;
    cJSON*   m_vic_timing;

    double CLOCK_STEP;                          //Pixel clock resolution
    double H_SYNC_PER                   = 0.08; //Percentage of the horizontal total period that defines horizontal sync width
    uint32_t MIN_V_PORCH_RND            = 3;    //Standard "CRT" Timing vertical front porch
    uint32_t MIN_V_BPORCH;
    uint32_t MIN_VSYNC_BP               = 550;  //Minimum time for Vertical Blanking period for "CRT" Timings. Set to 550us
    uint32_t RB_H_BLANK                 = 160;  /*Specifies the fixed number of pixel clock cycles in the Horizontal Blanking period for Reduced Blanking timings.
                                                Measured as the number of pixels from the last active pixel of one line to the first active pixel of the next line.*/
    uint32_t RB_H_SYNC                  = 32;   //Horizontal sync period for Reduced Blanking timings, expressed as the number of pixel clock cycles.
    uint32_t RB_MIN_V_BLANK             = 460;  /*Specifies the minimum vertical blanking period for Reduced Blanking timings. Measured as the number of lines.
                                                from the last line of active video to the first line of active video.*/
    uint32_t RB_V_FPORCH                = 3;    //Reduced Blanking vertical front porch.
    double MARGIN_PER                   = 1.8;  //Margin width, expressed as a percentage of the horizontal number of pixels (H_PIXELS_RND).
    uint32_t C_PRIME                    = 30;   //C_PRIME = ((C – J) * K / 256) + J.( set to 30)
    uint32_t M_PRIME                    = 300;  //M_PRIME = K / 256 * M. (set to 300)
    double CELL_GRAN                    = 8;    //Input number of pixel clock cycles in each character cell.Typically set to 8
    uint32_t CELL_GRAN_RND;                     //Character cell width in pixel clock cycles, rounded down to the nearest integer.
    uint32_t IP_FREQ_RQD;
    double   V_FIELD_RATE_RQD;                  //Specifies the desired vertical frame rate. The actual frame rate will be within +/- 0.5Hz of this value.
    uint32_t MARGINS_RQD;                       //margine requried
    uint32_t H_PIXELS;                          //Number of desired visible horizontal pixels per line.
    uint32_t H_PIXELS_RND;                      //Number of desired visible horizontal pixels rounded to the nearest character cel
    uint32_t LEFT_MARGIN;
    uint32_t RIGHT_MARGIN;
    uint32_t TOTAL_ACTIVE_PIXELS;
    uint32_t V_LINES;
    uint32_t V_LINES_RND;
    uint32_t TOP_MARGIN;
    uint32_t BOT_MARGIN;
    std::string ASPECT_RATIO;
    uint32_t V_SYNC_RND;
    uint32_t V_SYNC_BP;
    uint32_t V_BACK_PORCH;
    uint32_t TOTAL_V_LINES;
    uint32_t H_BLANK;
    uint32_t TOTAL_PIXELS;
    uint32_t V_BLANK;
    uint32_t VIC;
    uint32_t V_FRONT_PORCH;
    uint32_t H_SYNC;
    uint32_t H_BACK_PORCH;
    uint32_t H_FRONT_PORCH;
    uint32_t VBI_LINES;
    uint32_t RB_MIN_VBI;
    uint32_t ACT_VBI_LINES;
    double DMT_ID;
    double DMT_TYPE;
    double DMT_2BYTE_CODE;
    double DMT_3BYTE_CODE;
    double ACT_PIXEL_FREQ;
    double ACT_H_FREQ;
    double ACT_FIELD_RATE;
    double ACT_FRAME_RATE;
    double IDEAL_DUTY_CYCLE;
    double H_PERIOD_EST;
    double INTERLACE;
};
#endif //_VIDEO_TIMING_CACULATOR_H_