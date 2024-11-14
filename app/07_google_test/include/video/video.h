#ifndef _V_VIDEO_H_
#define _V_VIDEO_H_

#include <string>
#include <vector>

#include <fmt/color.h>
#include <fmt/format.h>
using namespace std;
class Video{
public:
    Video();
    bool    isRunning();
    string  getName();
    int     getNumber();
    ~Video();
private:
    string m_filename;
};
#endif
