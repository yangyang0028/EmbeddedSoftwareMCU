#ifndef __CLARKE_H
#define __CLARKE_H


#include "IQmathLib.h"


typedef struct
{
     _iq  As;  		
     _iq  Bs;			 
     _iq  Alpha;		 
     _iq  Beta;		
} clarke;

#define clarke_default {0, 0, 0, 0}

extern clarke clarke_parameter;

void clarke_calc(clarke *v);



#endif