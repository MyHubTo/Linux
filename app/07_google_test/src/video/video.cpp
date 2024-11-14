#include "video/video.h"
#include "log/log.h"

Video::Video(){}
Video::~Video(){}

bool Video::isRunning(){
    NLOGI("{}\n",__FUNCTION__);
    return false;
}
int Video::getNumber(){
    NLOGI("{}\n",__FUNCTION__);
    return 0;
}

string Video::getName(){
    NLOGI("{}\n",__FUNCTION__);
    return string();
}
