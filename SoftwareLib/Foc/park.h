#ifndef __PARK_H
#define __PARK_H

#include "IQmathLib.h"


typedef struct
{
  _iq  Alpha;  		 
  _iq  Beta;	 	 
  _iq  Angle;		 
  _iq  Ds;			 
  _iq  Qs;			 
} park;	            

#define park_default {0, 0, 0, 0, 0}

extern park park_parameter;

void park_calc(park *v);
////////////////////////////////////////////////////////////////////////////////


#endif