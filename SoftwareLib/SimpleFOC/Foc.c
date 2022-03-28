#include "Foc.h"

static void setPhaseVoltage(FOC *foc, float Uq, float Ud, float angle_el) {
    float Uout;
    uint32_t sector;
    float T0,T1,T2;
    float Ta,Tb,Tc;
    if(Ud) {
        Uout = _sqrt(Ud*Ud + Uq*Uq) / foc->foc_config->voltage_power_supply;
        angle_el = _normalizeAngle(angle_el + atan2(Uq, Ud));
    } else {
        Uout = Uq / foc->foc_config->voltage_power_supply;
        angle_el = _normalizeAngle(angle_el + _PI_2);
    }
    if(Uout > 0.577)Uout= 0.577;
    if(Uout < -0.577)Uout=-0.577;

    sector = (angle_el / _PI_3) + 1;
    T1 = _SQRT3*_sin(sector*_PI_3 - angle_el) * Uout;
    T2 = _SQRT3*_sin(angle_el - (sector-1.0)*_PI_3) * Uout;
    T0 = 1 - T1 - T2;

    switch(sector) {
        case 1:
            Ta = T1 + T2 + T0/2;
            Tb = T2 + T0/2;
            Tc = T0/2;
            break;
        case 2:
            Ta = T1 +  T0/2;
            Tb = T1 + T2 + T0/2;
            Tc = T0/2;
            break;
        case 3:
            Ta = T0/2;
            Tb = T1 + T2 + T0/2;
            Tc = T2 + T0/2;
            break;
        case 4:
            Ta = T0/2;
            Tb = T1+ T0/2;
            Tc = T1 + T2 + T0/2;
            break;
        case 5:
            Ta = T2 + T0/2;
            Tb = T0/2;
            Tc = T1 + T2 + T0/2;
            break;
        case 6:
            Ta = T1 + T2 + T0/2;
            Tb = T0/2;
            Tc = T1 + T0/2;
            break;
        default:  // possible error state
            Ta = 0;
            Tb = 0;
            Tc = 0;
    }
    foc->foc_config->OutPutPWM(Ta, Tb, Tc);
}

static float ShaftAngle(FOC *foc) {
  foc->shaft_angle = (int)foc->sensor_direction * foc->foc_config->GetAnage();
  return foc->shaft_angle;
}

static float LowPassFilter(float input, float prev) {
	return 0.9*prev + 0.1*input;
}

static float ShaftTargetAngle(FOC *foc, float target_velocity) {
    uint64_t now_us = _micros();
    float Ts = 0;
    if(now_us > foc->time_prev) {
        Ts = (float)(now_us - foc->time_prev)*1e-6;
    } else {
        Ts = (float)(0xFFFFFFFF - foc->time_prev + now_us)*1e-6;
    }

    // float angle_now = foc->foc_config->GetAnage();
    // float temp = 0;
    // if (angle_now - foc->angle_prev < -_PI) temp = _2PI + angle_now - foc->angle_prev;
    // else if (angle_now - foc->angle_prev > _PI) temp = _2PI - angle_now + foc->angle_prev;
    // else temp = angle_now - foc->angle_prev;
    // foc->shaft_velocity = LowPassFilter(temp/Ts, foc->shaft_velocity);
    // OUTPUT("a=%0.3lf,\n",foc->shaft_velocity, temp, Ts);
    // foc->angle_prev = angle_now;

    foc->time_prev=now_us; 
    if(Ts == 0 || Ts > 0.5) Ts = 1e-3;
    foc->shaft_angle = _normalizeAngle(foc->shaft_angle + target_velocity*Ts); 
    return foc->shaft_angle;
}

static float ShaftVelocity(FOC *foc) {
    uint64_t now_us = _micros();
    float Ts = 0;
    float angle_now = 0;
    if(now_us > foc->time_prev) {
        Ts = (float)(now_us - foc->time_prev)*1e-6;
    } else {
        Ts = (float)(0xFFFFFFFF - foc->time_prev + now_us)*1e-6;
    }
    if(Ts == 0 || Ts > 0.5) Ts = 1e-3;
    foc->time_prev=now_us; 
    float temp = 0;
    angle_now = foc->foc_config->GetAnage();
    if (angle_now - foc->angle_prev < -_PI) temp = _2PI + angle_now - foc->angle_prev;
    else if (angle_now - foc->angle_prev > _PI) temp = _2PI - angle_now + foc->angle_prev;
    else temp = angle_now - foc->angle_prev;
    foc->shaft_velocity = LowPassFilter(temp/Ts, foc->shaft_velocity);
    // OUTPUT("a=%0.3lf,\n",foc->shaft_velocity, temp, Ts);
    foc->angle_prev = angle_now;
    return foc->shaft_velocity;
}

static void alignSensor(FOC *foc) {
	float angle = 0;
	float mid_angle = 0;
	float end_angle = 0;
	float moved = 0;
	for (int i = 0; i <= 500; i++) {
		angle = _2PI * i / 500.0;
		setPhaseVoltage(foc, foc->foc_config->voltage_sensor_align, 0, angle);
		foc->foc_config->DelayMs(2);
	}
	mid_angle = foc->foc_config->GetAnage();
	for (int i = 500; i >= 0; i--) {
		angle = _2PI * i / 500.0;
		setPhaseVoltage(foc, foc->foc_config->voltage_sensor_align, 0, angle);
		foc->foc_config->DelayMs(2);
	}
	end_angle = foc->foc_config->GetAnage();
	setPhaseVoltage(foc, 0, 0, 0);
	foc->foc_config->DelayMs(200);
	DBG_OUTPUT(INFORMATION, "mid_angle=%lf, end_angle=%lf,", mid_angle, end_angle);
	moved = fabs(mid_angle - end_angle);
    DBG_OUTPUT(INFORMATION, "moved=%lf", moved);
	if ((mid_angle == end_angle) || (moved < 0.02)) {
        DBG_OUTPUT(ERROR, "Failed to notice movement.\r\n");
		return ;
	}
    else if (mid_angle < end_angle) {
        DBG_OUTPUT(INFORMATION, "sensor_direction==CCW\r\n");
        foc->sensor_direction = CCW;
    } else {
        DBG_OUTPUT(INFORMATION, "sensor_direction==CW\r\n");
        foc->sensor_direction = CW;
    }
    if (fabs(moved * foc->foc_config->pole_pairs - _2PI) > 0.5) {
        foc->foc_config->pole_pairs = _2PI / moved + 0.5; //浮点数转整形，四舍五入
    }
    DBG_OUTPUT(INFORMATION, "pole_pairs=%ld,", foc->foc_config->pole_pairs);
    setPhaseVoltage(foc, foc->foc_config->voltage_sensor_align, 0, _3PI_2);
    foc->foc_config->DelayMs(700);
    foc->zero_electric_angle = _normalizeAngle(ShaftAngle(foc)*foc->foc_config->pole_pairs);
    DBG_OUTPUT(INFORMATION, "zero_electric_angle=%lf,", foc->zero_electric_angle);
    foc->foc_config->DelayMs(20);
    setPhaseVoltage(foc, 0, 0, 0);
    foc->foc_config->DelayMs(20);
}

RETURN_CODE FOCInit(FOC *foc) {
    if (foc->foc_config == NULL){
        return EEMPTY;
    }
    if (foc->foc_config->voltage_sensor_align > foc->foc_config->voltage_limit){
		foc->foc_config->voltage_sensor_align = foc->foc_config->voltage_limit;
	}
	foc->sensor_direction = UNKNOWN ;
	alignSensor(foc);
    foc->angle_prev=foc->foc_config->GetAnage();
	foc->foc_config->DelayMs(5);
	foc->shaft_velocity = ShaftVelocity(foc);  //必须调用一次，进入主循环后速度为0
	foc->foc_config->DelayMs(5);
	foc->shaft_angle = ShaftAngle(foc);
	return EOK;
}

RETURN_CODE FOCMove(FOC *foc, float target) {
    switch(foc->foc_config->controller){
        case Type_velocity_openloop:
            foc->shaft_angle = ShaftTargetAngle(foc, target);
            foc->voltage.q = foc->foc_config->voltage_limit;
            foc->voltage.d = 0;
            break;
        case Type_velocity:
            foc->shaft_angle = ShaftAngle(foc);
            foc->foc_config->velocity_pid->target = target;
            foc->foc_config->velocity_pid->feedback = ShaftVelocity(foc);
            foc->voltage.q = PIDCalc(foc->foc_config->velocity_pid);  
            foc->voltage.d = 0;
            break;
    }
    float electrical_angle = foc->shaft_angle * foc->foc_config->pole_pairs - foc->zero_electric_angle;
    OUTPUT("%lf %lf %lf\n", foc->foc_config->velocity_pid->target, foc->foc_config->velocity_pid->feedback,foc->voltage.q);
    setPhaseVoltage(foc, foc->voltage.q, foc->voltage.d, electrical_angle);
    return EOK;
}
