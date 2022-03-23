#include "ipark.h"



ipark ipark_parameter = ipark_default;



void ipark_calc(ipark *v)
{
     _iq Cosine,Sine;
     
     Sine = _IQsinPU(v->Angle);
     Cosine = _IQcosPU(v->Angle);
     
     v->Alpha = _IQmpy(v->Ds,Cosine) - _IQmpy(v->Qs,Sine);
     v->Beta = _IQmpy(v->Qs,Cosine) + _IQmpy(v->Ds,Sine);
}