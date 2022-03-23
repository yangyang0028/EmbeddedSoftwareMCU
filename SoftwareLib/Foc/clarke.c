#include "clarke.h"

clarke clarke_parameter = clarke_default;

void clarke_calc(clarke *v)
{
     v->Alpha = v->As;
     //v->Beta = _IQmpy((v->As + _IQmpy(_IQ(2),v->Bs)) , _IQ(0.57735026918963));
     v->Beta = _IQmpy((v->As + 2* v->Bs) , _IQ(0.57735026918963)); 
}
