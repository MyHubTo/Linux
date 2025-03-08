#include "VideoTimingCalculator.h"
VideoTimingCaculator::VideoTimingCaculator(){
    addConfig();
}

VideoTimingCaculator::~VideoTimingCaculator(){
    cJSON_Delete(m_vic_timing);
}

void VideoTimingCaculator::addConfig(){
    fstream file(DEFAULT_CONFIG_FILE,ios::in);
    std::string content={};
    if (!file.is_open()){
        NLOGE("{} error\n",__FUNCTION__);
        return;
    }
    char buffer[MAX_BUFFER_SIZE];
    while (file.getline(buffer, MAX_BUFFER_SIZE)) {
        content += std::string(buffer);
    }
    file.close();
    m_vic_timing = cJSON_Parse(content.c_str());
    if (!m_vic_timing) {
        NLOGE("{}\n", "Parse error");
    }
    return;
}

void VideoTimingCaculator::calculateCvt(uint32_t horiz_pixels, uint32_t vert_pixels,double refresh_rate,
                                        uint32_t bpc,double color_fmt_multiplier,std::string reduced_blanking,
                                        bool margins, bool interlaced, bool video_optimized){
    std::string H_POL="?";
    std::string V_POL="?";
    double REFRESH_MULTIPLIER=0;
    int INT_RQD=0;
    if(reduced_blanking=="cvt"){
        CLOCK_STEP          =0.25;
        MIN_V_BPORCH        =6;
        RB_H_BLANK          = 160;
        RB_H_SYNC           = 32;
        RB_MIN_V_BLANK      = 460;
        RB_V_FPORCH         = 3;
        REFRESH_MULTIPLIER  = 1;
        H_POL               = "-";
        V_POL               = "+";
    }else if (reduced_blanking == "cvt_rb"){
        CLOCK_STEP          = 0.25;
        MIN_V_BPORCH        = 6;
        RB_H_BLANK          = 160;
        RB_H_SYNC           = 32;
        RB_MIN_V_BLANK      = 460;
        RB_V_FPORCH         = 3;
        REFRESH_MULTIPLIER  = 1;
        H_POL               = "+";
        V_POL               = "-";
    }else if (reduced_blanking == "cvt_rb2"){
        CLOCK_STEP          = 0.001;
        MIN_V_BPORCH        = 6;
        RB_H_BLANK          = 80;
        RB_H_SYNC           = 32;
        RB_MIN_V_BLANK      = 460;
        RB_V_FPORCH         = 1;
        REFRESH_MULTIPLIER  = video_optimized ? ((1000*1.0)/1001) : 1;
        H_POL               = "+";
        V_POL               = "-";
    }else if (reduced_blanking == "cea"){
        timing_t cea_timing;
        cea_timing=lookupVicTiming(horiz_pixels,vert_pixels,refresh_rate,bpc,color_fmt_multiplier,interlaced);
        if((cea_timing.pix_clock>0)&&(cea_timing.h_active>0)&&(cea_timing.v_active>0)){
            m_timing=cea_timing;
            return;
        }else{
            NLOGW("{} cea timing not found\n",__FUNCTION__);
            return;
        }
    }
        // Input parameters
        H_PIXELS            = horiz_pixels;
        V_LINES             = vert_pixels;
        IP_FREQ_RQD         = refresh_rate;
        MARGINS_RQD         = margins;
        INT_RQD             = interlaced;
        CELL_GRAN_RND       =std::floor(CELL_GRAN);

        // 5.2 Computation of Common Parameters
        V_FIELD_RATE_RQD    = INT_RQD ? IP_FREQ_RQD * 2.0 : IP_FREQ_RQD;
        H_PIXELS_RND        = std::floor(H_PIXELS / CELL_GRAN_RND) * CELL_GRAN_RND;

        LEFT_MARGIN         = MARGINS_RQD ? (std::floor((H_PIXELS_RND * MARGIN_PER / 100.0) / CELL_GRAN_RND) * CELL_GRAN_RND) : 0;
        RIGHT_MARGIN        = LEFT_MARGIN;

        TOTAL_ACTIVE_PIXELS = H_PIXELS_RND + LEFT_MARGIN + RIGHT_MARGIN;

        V_LINES_RND         = INT_RQD ? std::floor(V_LINES / 2.0) :std::floor(V_LINES);

        TOP_MARGIN          = MARGINS_RQD ? std::floor(V_LINES_RND * MARGIN_PER / 100.0) : 0;
        BOT_MARGIN          = TOP_MARGIN;

        INTERLACE           = INT_RQD ? 0.5 : 0;

        // Calculate Aspect Ratio
        double ver_pixels = INT_RQD ? 2 * V_LINES_RND : V_LINES_RND;
        double hor_pixels_4_3   = CELL_GRAN_RND * std::round(ver_pixels *  4 /  3) / CELL_GRAN_RND;
        double hor_pixels_16_9  = CELL_GRAN_RND * std::round(ver_pixels * 16 /  9) / CELL_GRAN_RND;
        double hor_pixels_16_10 = CELL_GRAN_RND * std::round(ver_pixels * 16 / 10) / CELL_GRAN_RND;
        double hor_pixels_5_4   = CELL_GRAN_RND * std::round(ver_pixels *  5 /  4) / CELL_GRAN_RND;
        double hor_pixels_15_9  = CELL_GRAN_RND * std::round(ver_pixels * 15 /  9) / CELL_GRAN_RND;
        double hor_pixels_43_18 = CELL_GRAN_RND * std::round(ver_pixels * 43 / 18) / CELL_GRAN_RND;
        double hor_pixels_64_27 = CELL_GRAN_RND * std::round(ver_pixels * 64 / 27) / CELL_GRAN_RND;
        double hor_pixels_12_5  = CELL_GRAN_RND * std::round(ver_pixels * 12 /  5) / CELL_GRAN_RND;
        ASPECT_RATIO = (hor_pixels_4_3   == H_PIXELS_RND) ? "4:3"   :
                               (hor_pixels_16_9  == H_PIXELS_RND) ? "16:9"  :
                               (hor_pixels_16_10 == H_PIXELS_RND) ? "16:10" :
                               (hor_pixels_5_4   == H_PIXELS_RND) ? "5:4"   :
                               (hor_pixels_15_9  == H_PIXELS_RND) ? "15:9"  :
                               // Following aspect ratios are not defined by the spec...
                               (hor_pixels_43_18 == H_PIXELS_RND) ? "43:18" :
                               (hor_pixels_64_27 == H_PIXELS_RND) ? "64:27" :
                               (hor_pixels_12_5  == H_PIXELS_RND) ? "12:5"  :
                                                                    "Unknown";
        // V_SYNC_RND depends on aspect ratio and reduced blanking
        if      (reduced_blanking == "cvt_rb2") V_SYNC_RND = 8;
        else if (ASPECT_RATIO == "4:3")       V_SYNC_RND = 4;
        else if (ASPECT_RATIO == "16:9")      V_SYNC_RND = 5;
        else if (ASPECT_RATIO == "16:10")     V_SYNC_RND = 6;
        else if (ASPECT_RATIO == "5:4")       V_SYNC_RND = 7;
        else if (ASPECT_RATIO == "15:9")      V_SYNC_RND = 7;
        else                                  V_SYNC_RND = 10;
        if (reduced_blanking == "cvt"){
            // 5.3 Computation of CRT Timing Parameters
            H_PERIOD_EST    = ((1 / V_FIELD_RATE_RQD) - MIN_VSYNC_BP / 1000000.0) / (V_LINES_RND + (2 * TOP_MARGIN) + MIN_V_PORCH_RND + INTERLACE) * 1000000.0;
            V_SYNC_BP       = std::floor(MIN_VSYNC_BP / H_PERIOD_EST) + 1;
            if (V_SYNC_BP < (V_SYNC_RND + MIN_V_BPORCH)){
                V_SYNC_BP = V_SYNC_RND + MIN_V_BPORCH;
            }
            V_BLANK             = V_SYNC_BP + MIN_V_PORCH_RND;
            V_FRONT_PORCH       = MIN_V_PORCH_RND;
            V_BACK_PORCH        = V_SYNC_BP - V_SYNC_RND;
            TOTAL_V_LINES       = V_LINES_RND + TOP_MARGIN + BOT_MARGIN + V_SYNC_BP + INTERLACE + MIN_V_PORCH_RND;
            IDEAL_DUTY_CYCLE    = C_PRIME - (M_PRIME * H_PERIOD_EST/1000.0);
            if (IDEAL_DUTY_CYCLE < 20){
                H_BLANK = std::floor(TOTAL_ACTIVE_PIXELS * 20 / (100-20) / (2 * CELL_GRAN_RND)) * (2 * CELL_GRAN_RND);
            }
            else{
                H_BLANK = std::floor(TOTAL_ACTIVE_PIXELS * IDEAL_DUTY_CYCLE / (100 - IDEAL_DUTY_CYCLE) / (2 * CELL_GRAN_RND)) * (2 * CELL_GRAN_RND);
            }
            TOTAL_PIXELS    = TOTAL_ACTIVE_PIXELS + H_BLANK;
            H_SYNC          = std::floor(H_SYNC_PER * TOTAL_PIXELS / CELL_GRAN_RND) * CELL_GRAN_RND;
            H_BACK_PORCH    = H_BLANK / 2;
            H_FRONT_PORCH   = H_BLANK - H_SYNC - H_BACK_PORCH;
            ACT_PIXEL_FREQ  = CLOCK_STEP * std::floor(TOTAL_PIXELS / H_PERIOD_EST / CLOCK_STEP);
        }else{
            H_PERIOD_EST    = ((1000000 / V_FIELD_RATE_RQD) - RB_MIN_V_BLANK) / (V_LINES_RND + TOP_MARGIN + BOT_MARGIN);
            H_BLANK         = RB_H_BLANK;
            VBI_LINES       = std::floor(RB_MIN_V_BLANK / H_PERIOD_EST) + 1;
            RB_MIN_VBI      = RB_V_FPORCH + V_SYNC_RND + MIN_V_BPORCH;
            ACT_VBI_LINES   = (VBI_LINES < RB_MIN_VBI) ? RB_MIN_VBI : VBI_LINES;
            TOTAL_V_LINES   = ACT_VBI_LINES + V_LINES_RND + TOP_MARGIN + BOT_MARGIN + INTERLACE;
            TOTAL_PIXELS    = RB_H_BLANK + TOTAL_ACTIVE_PIXELS;
            ACT_PIXEL_FREQ  = CLOCK_STEP *std::floor((V_FIELD_RATE_RQD * TOTAL_V_LINES * TOTAL_PIXELS / 1000000.0 * REFRESH_MULTIPLIER) / CLOCK_STEP);
            if (reduced_blanking == "cvt_rb2"){
                V_BLANK         = ACT_VBI_LINES;
                V_FRONT_PORCH   = ACT_VBI_LINES - V_SYNC_RND - 6;
                V_BACK_PORCH    = 6;
                H_SYNC          = 32;
                H_BACK_PORCH    = 40;
                H_FRONT_PORCH   = H_BLANK - H_SYNC - H_BACK_PORCH;
            }else{
                V_BLANK         = ACT_VBI_LINES;
                V_FRONT_PORCH   = 3;
                V_BACK_PORCH    = ACT_VBI_LINES - V_FRONT_PORCH - V_SYNC_RND;
                H_SYNC          = 32;
                H_BACK_PORCH    = 80;
                H_FRONT_PORCH   = H_BLANK - H_SYNC - H_BACK_PORCH;
            }
        }

        ACT_H_FREQ                  = 1000 * ACT_PIXEL_FREQ /TOTAL_PIXELS;
        ACT_FIELD_RATE              = 1000 * ACT_H_FREQ /TOTAL_V_LINES;
        ACT_FRAME_RATE              = INT_RQD ? ACT_FIELD_RATE / 2 : ACT_FIELD_RATE;
        uint32_t hactive            = TOTAL_ACTIVE_PIXELS;
        uint32_t hblank             = H_BLANK;
        uint32_t htotal             = TOTAL_PIXELS;
        uint32_t vactive            = V_LINES_RND;
        uint32_t vblank             = V_BLANK;
        uint32_t vtotal             = TOTAL_V_LINES;
        double pclock               = ACT_PIXEL_FREQ * 1000000;
        double hfreq                = pclock / htotal;
        double vfreq                = pclock / (vtotal * htotal);
        // double vfreq_round100       = std::round(vfreq*100)/100;

        double hperiod              = 1/hfreq;
        double vperiod              = 1/vfreq;
        double peak_bw              = pclock * bpc * color_fmt_multiplier;
        // double peak_bw_dsc          = pclock * 8;   // Minium BW required, when compressed to 8bpp.
        double line_bw              = peak_bw * hactive / htotal;
        double active_bw            = vfreq * bpc * vactive * hactive * color_fmt_multiplier;
        double vblank_duration      = vblank * hperiod;
        m_timing.type               = reduced_blanking;
        m_timing.pix_clock          = std::round(pclock/1000)/1000.0;
        m_timing.h_total            = htotal;
        m_timing.h_active           = hactive;
        m_timing.h_blank            = hblank;
        m_timing.h_front_porch      = H_FRONT_PORCH;
        m_timing.h_sync             = H_SYNC;
        m_timing.h_back_porch       = H_BACK_PORCH;
        m_timing.h_sync_polarity    = (H_POL=="+")?1:0;
        m_timing.h_freq             = std::round(hfreq)/1000.0;
        m_timing.h_period           = std::round(hperiod*1000000000)/1000.0;

        m_timing.v_total            = vtotal;
        m_timing.v_active           = vactive;
        m_timing.v_blank            = vblank;
        m_timing.vblank_duration    = std::round(vblank_duration * 1000000);
        m_timing.v_front_porch      = V_FRONT_PORCH;
        m_timing.v_sync             = V_SYNC_RND;
        m_timing.v_back_porch       = V_BACK_PORCH;
        m_timing.v_sync_polarity    = (V_POL=="+")?1:0;
        m_timing.v_freq             = std::round(vfreq*1000)/1000.0;
        m_timing.v_period           = std::round(vperiod*1000000)/1000.0;
        m_timing.peak_bw            = std::round(peak_bw/1000000);
        m_timing.line_bw            = std::round(line_bw/1000000);
        m_timing.active_bw          = std::round(active_bw/1000000);
        return;
}

timing_t VideoTimingCaculator::getCvtTiming(){
    return m_timing;
}

timing_t VideoTimingCaculator::lookupVicTiming(uint32_t horiz_pixels,uint32_t vert_pixels,double refresh_rate,uint32_t bpc,double color_fmt_multiplier,bool interlaced){
    timing_t vic_timing={};
    if(m_vic_timing){
        NLOGI("{} m_vic_timing case\n",__FUNCTION__);
        if(cJSON_IsArray(m_vic_timing)){
            int array_size = cJSON_GetArraySize(m_vic_timing);
            for(int i=0; i< array_size; i++){
                cJSON *item = cJSON_GetArrayItem(m_vic_timing,i);
                if(cJSON_IsObject(item)){
                    uint32_t h_pix = cJSON_GetObjectItem(item,"h_active")->valueint;
                    uint32_t v_pix = cJSON_GetObjectItem(item,"v_active")->valueint;
                    double   rate  = cJSON_GetObjectItem(item,"v_freq")->valuedouble;
                    if((h_pix==horiz_pixels) && (v_pix==vert_pixels) && (std::round(refresh_rate) == std::round(rate)) &&
                    ((interlaced && (std::string(cJSON_GetObjectItem(item,"scan")->valuestring)=="Int"))
                    ||((!interlaced ) && (std::string(cJSON_GetObjectItem(item,"scan")->valuestring)=="Prog")))){
                        vic_timing.type="cea";
                        vic_timing.h_active         = h_pix;
                        vic_timing.v_active         = v_pix;
                        vic_timing.v_freq           = rate;

                        vic_timing.h_total          = cJSON_GetObjectItem(item,"h_total")->valueint;
                        vic_timing.h_blank          = cJSON_GetObjectItem(item,"h_blank")->valueint;
                        vic_timing.h_front_porch    = cJSON_GetObjectItem(item,"h_front")->valueint;
                        vic_timing.h_sync           = cJSON_GetObjectItem(item,"h_sync")->valueint;
                        vic_timing.h_back_porch     = cJSON_GetObjectItem(item,"h_back")->valueint;
                        vic_timing.h_freq           = cJSON_GetObjectItem(item,"h_freq")->valuedouble;
                        vic_timing.h_sync_polarity  = (std::string(cJSON_GetObjectItem(item,"h_pol")->string)=="N")?0:1;

                        vic_timing.v_total          = cJSON_GetObjectItem(item,"v_total")->valueint;
                        vic_timing.v_blank          = cJSON_GetObjectItem(item,"v_blank")->valueint;
                        vic_timing.v_front_porch    = cJSON_GetObjectItem(item,"v_front")->valueint;
                        vic_timing.v_sync           = cJSON_GetObjectItem(item,"v_sync")->valueint;
                        vic_timing.v_back_porch     = cJSON_GetObjectItem(item,"v_back")->valueint;
                        vic_timing.v_freq           = cJSON_GetObjectItem(item,"v_freq")->valuedouble;
                        vic_timing.v_sync_polarity  = (std::string(cJSON_GetObjectItem(item,"v_pol")->string)=="N")?0:1;

                        double pclock               = cJSON_GetObjectItem(item,"pix_clock")->valuedouble * 1000000;
                        double hfreq = pclock / vic_timing.h_total;
                        double vfreq = pclock / (vic_timing.h_total * vic_timing.v_total);
                        double hperiod = 1 / hfreq;
                        double vperiod = 1 / vfreq;
                        double peak_bw = pclock * bpc * color_fmt_multiplier;
                        // double peak_bw_dsc = pclock * 8; // Minium BW required, when compressed to 8bpp.
                        double line_bw = peak_bw * vic_timing.h_active / vic_timing.h_total;
                        double active_bw = vfreq * bpc * vic_timing.v_active * vic_timing.h_active  * color_fmt_multiplier;
                        double vblank_duration = vic_timing.v_blank * hperiod;
                        vic_timing.vblank_duration  = std::round(vblank_duration * 1000000);
                        vic_timing.h_period         = std::round(hperiod * 1000000000) / 1000;
                        vic_timing.v_period         = std::round(vperiod * 1000000) / 1000;
                        vic_timing.peak_bw          = std::round(peak_bw / 1000000);
                        vic_timing.line_bw          = std::round(line_bw / 1000000);
                        vic_timing.active_bw        = std::round(active_bw / 1000000);
                        vic_timing.pix_clock        = std::round(pclock / 1000)/1000;
                        NLOGI("find cea timing vic is {}\n",cJSON_GetObjectItem(item,"vic")->valueint);
                    }
                }
            }
        }
    }else{
        NLOGE("{} m_vic_timing is null\n",__FUNCTION__);
    }
    return vic_timing;
}

void VideoTimingCaculator::dump(){
#ifdef FMT_OUTPUT
    NLOGI("{}_pix_clock        {} MHz\n",      m_timing.type, m_timing.pix_clock);
    NLOGI("{}_h_total          {} Pixels\n",   m_timing.type, m_timing.h_total);
    NLOGI("{}_h_active         {} Pixels\n",   m_timing.type, m_timing.h_active);
    NLOGI("{}_h_blank          {} Pixels\n",   m_timing.type, m_timing.h_blank);
    NLOGI("{}_h_front_porch    {} Pixels\n",   m_timing.type, m_timing.h_front_porch);
    NLOGI("{}_h_sync           {} Pixels\n",   m_timing.type, m_timing.h_sync);
    NLOGI("{}_h_back_porch     {} Pixels\n",   m_timing.type, m_timing.h_back_porch);
    NLOGI("{}_h_sync_polarity  {} \n",         m_timing.type, m_timing.h_sync_polarity);
    NLOGI("{}_h_freq           {} KHz\n",      m_timing.type, m_timing.h_freq);
    NLOGI("{}_h_period         {} us\n",       m_timing.type, m_timing.h_period);

    NLOGI("{}_v_total          {} Lines\n",   m_timing.type, m_timing.v_total);
    NLOGI("{}_v_active         {} Lines\n",   m_timing.type, m_timing.v_active);
    NLOGI("{}_v_blank          {} Lines\n",   m_timing.type, m_timing.v_blank);
    NLOGI("{}_vblank_duration  {} us\n",       m_timing.type, m_timing.vblank_duration);
    NLOGI("{}_v_front_porch    {} Lines\n",   m_timing.type, m_timing.v_front_porch);
    NLOGI("{}_v_sync           {} Lines\n",   m_timing.type, m_timing.v_sync);
    NLOGI("{}_v_back_porch     {} Lines\n",   m_timing.type, m_timing.v_back_porch);
    NLOGI("{}_v_sync_polarity  {} \n",         m_timing.type, m_timing.v_sync_polarity);
    NLOGI("{}_v_freq           {} Hz\n",       m_timing.type, m_timing.v_freq);
    NLOGI("{}_v_period         {} ms\n",       m_timing.type, m_timing.v_period);
    NLOGI("{}_peak_bw          {} Mbit/s\n",   m_timing.type, m_timing.peak_bw);
    NLOGI("{}_line_bw          {} Mbit/s\n",   m_timing.type, m_timing.line_bw);
    NLOGI("{}_active_bw        {} Mbit/s\n",   m_timing.type, m_timing.active_bw);
#endif
    return;
}