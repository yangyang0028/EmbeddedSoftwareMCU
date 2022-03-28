#include "Pid.h"
#include "common.h"

float PIDCalc(PID *pid){
	float out;
	float ep, ei, ed;
	pid->sum_e += pid->e_0;

	pid->e_0 = pid->target - pid->feedback;
	ep = pid->e_0;
	ei = pid->sum_e;
	ed = pid->e_0  - pid->e_1;

	out = pid->Kp*ep + pid->Ki*ei + pid->Kd*ed;
	pid->e_1 = pid->e_0;
	return out;
}
