#ifndef __IPARK_H
#define __IPARK_H

#include "IQmathLib.h"

typedef struct
{
     _iq  Alpha;  		
     _iq  Beta;		
     _iq  Angle;		
     _iq  Ds;		
     _iq  Qs;		
} ipark;

#define ipark_default {0, 0, 0, 0, 0}

extern ipark ipark_parameter;

void ipark_calc(ipark *v);


#endif