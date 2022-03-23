#include "park.h"


park park_parameter = park_default;


void park_calc(park *v)
{
     _iq Cosine,Sine;
     
     Sine = _IQsinPU(v->Angle);
     Cosine = _IQcosPU(v->Angle);
     
     v->Ds = _IQmpy(v->Alpha,Cosine) + _IQmpy(v->Beta,Sine);
     v->Qs = _IQmpy(v->Beta,Cosine) - _IQmpy(v->Alpha,Sine);
}