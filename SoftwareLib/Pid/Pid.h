#ifndef _PID_H
#define _PID_H

typedef struct PID {
	float target;
	float feedback;
	float Kp;
	float Ki;
	float Kd;
	float e_0;	
	float e_1;
	float sum_e;
}PID;

float PIDCalc(PID *pid);
#endif
