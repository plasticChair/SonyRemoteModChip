
#include "helper.h"


struct ControlStruct CntlParam;

void init_CntlParam()
{
    CntlParam.skip_sleep = 0;
    
}


void set_skip_sleep()
{
    CntlParam.skip_sleep = 1;
}