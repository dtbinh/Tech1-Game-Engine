#include "SpecialMath.h"
void ConvertFloatToInt(int & integer,unsigned int & AfterDot, float num)
{
//AfterDot = unsigned  int(num * 0x100000000);
integer  = int(num++);
AfterDot = (num-(float)integer)* 1000000000;
}