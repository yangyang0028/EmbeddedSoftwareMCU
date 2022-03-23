#ifndef __SVPWM_H
#define __SVPWM_H

/* =================================================================================
File name:       __SVPWM_H.H  
===================================================================================*/

#include "common.h"
#include "IQmathLib.h"

#define SV_DUTY		10
#define SV_MAX		95
#define SV_MIN		1

struct sv_mod{ 
	_iq  Ualpha; 			// Input: reference alpha-axis phase voltage 
	_iq  Ubeta;				// Input: reference beta-axis phase voltage 
	_iq  Ta;				// Output: reference phase-a switching function		
	_iq  Tb;				// Output: reference phase-b switching function 
	_iq  Tc;				// Output: reference phase-c switching function
	_iq  tmp1;				// Variable: temp variable
	_iq  tmp2;				// Variable: temp variable
	_iq  tmp3;				// Variable: temp variable
	uint16_t VecSector;		// Space vector sector
};
typedef struct 	sv_mod sv_mod_t;

struct sv_regs_mod{
	_iq ccr1;				// Output: pwm compare register 1
	_iq ccr2;				// Output: pwm compare register 2
	_iq ccr3;				// Output: pwm compare register 3
};

typedef	struct sv_regs_mod	sv_regs_mod_t;

/*-----------------------------------------------------------------------------
Default initalizer for the SVGEN object.
-----------------------------------------------------------------------------*/                     
#define SVGEN_DEFAULTS { 0,0,0,0,0 }                       
extern sv_mod_t sv;
/*------------------------------------------------------------------------------
	Space Vector  Generator (SVGEN) Macro Definition
------------------------------------------------------------------------------*/

#define SVGENDQ_MACRO(v)														\
	v.tmp1= v.Ubeta;															\
	v.tmp2= _IQdiv2(v.Ubeta) + (_IQmpy(_IQ(0.866),v.Ualpha));					\
    v.tmp3= v.tmp2 - v.tmp1;													\
																				\
	v.VecSector=3;																\
	v.VecSector=(v.tmp2> 0)?( v.VecSector-1):v.VecSector;						\
	v.VecSector=(v.tmp3> 0)?( v.VecSector-1):v.VecSector;						\
	v.VecSector=(v.tmp1< 0)?(7-v.VecSector) :v.VecSector;						\
																				\
	if     (v.VecSector==1 || v.VecSector==4)                                   \
      {     v.Ta= v.tmp2; 														\
      		v.Tb= v.tmp1-v.tmp3; 												\
      		v.Tc=-v.tmp2;														\
      }								    										\
   																				\
    else if(v.VecSector==2 || v.VecSector==5)                                   \
      {     v.Ta= v.tmp3+v.tmp2; 												\
      		v.Tb= v.tmp1; 														\
      		v.Tc=-v.tmp1;														\
      }																	   		\
   																				\
    else                                                                        \
      {     v.Ta= v.tmp3; 														\
      		v.Tb=-v.tmp3; 														\
      		v.Tc=-(v.tmp1+v.tmp2);												\
      }																	   		\
																				\

//end of SVGENDQ_MACRO
	 
void svpwm_calc(sv_mod_t* p);
sv_regs_mod_t svpwm_get_regs_mod(_iq vdc,_iq pwm_t, sv_mod_t* v);
void svpwm_update(_iq vdc, sv_mod_t* p);
	  
#endif