#include <iostream>
#include "nlog.h"
using namespace std;
#define TAG "test"
int main(){
    std::cout<<"Hello,Android!"<<std::endl;
    NLOGV("%s : %s\n",__func__,"this is a VERBOSE message");
    NLOGD("%s : %s\n",__func__,"this is a DEBUG message");
    NLOGI("%s : %s\n",__func__,"this is a INFO message");
    NLOGW("%s : %s\n",__func__,"this is a WARN message");
    NLOGE("%s : %s\n",__func__,"this is a ERROR message");
    NLOGF("%s : %s\n",__func__,"this is a FATAL message");
    return 0;
}