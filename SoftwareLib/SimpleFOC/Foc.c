#include "Foc.h"

static void SetPhaseVoltage(FOC *foc, float Uq, float Ud, float angle_el) {
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
  return foc->foc_config->GetShaftAnage();
}

static void DelayMs(FOC *foc, uint16_t ms) {
    foc->foc_config->DelayMs(ms);
}

static float LowPassFilter(float input, float prev) {
	return 0.9*prev + 0.1*input;
}

static float ShaftTargetAngle(FOC *foc, float target_velocity, uint64_t time_prev, uint64_t *update_time) {
    uint64_t now_us = _micros();
    float Ts = 0;
    if(now_us > time_prev) 
        Ts = (float)(now_us - time_prev)*1e-6;
    else
        Ts = (float)(0xFFFFFFFF - time_prev + now_us)*1e-6;
    if (update_time != NULL) *update_time=now_us;
    if(Ts == 0 || Ts > 0.5) Ts = 1e-3;
    return _normalizeAngle(foc->shaft_angle + target_velocity * Ts);
}

static float ShaftVelocity(FOC *foc, int64_t time_prev, uint64_t *update_time, float angle_prev, float *updata_angle) {
    uint64_t now_us = _micros();
    float Ts = 0;
    float angle_now = 0;
    if(now_us > time_prev) {
        Ts = (float)(now_us - time_prev)*1e-6;
    } else {
        Ts = (float)(0xFFFFFFFF - time_prev + now_us)*1e-6;
    }
    if (update_time != NULL) *update_time=now_us;
    if(Ts == 0 || Ts > 0.5) Ts = 1e-3;

    float temp = 0;
    angle_now = ShaftAngle(foc);
    if (angle_now - angle_prev < -_PI) temp = _2PI + angle_now - angle_prev;
    else if (angle_now - angle_prev > _PI) temp = _2PI - angle_now + angle_prev;
    else temp = angle_now - angle_prev;
    if (updata_angle !=  NULL) *updata_angle = angle_now;
    OUTPUT("A=%l0.3f, B=%0.3lf, C=%0.3lf,\n", temp/Ts, temp, Ts);
    return LowPassFilter(temp/Ts, foc->shaft_velocity);
}

RETURN_CODE FOCInit(FOC *foc) {
    if (foc->foc_config == NULL) {
        return EEMPTY;
    }
    if (foc->foc_config->voltage_sensor_align > foc->foc_config->voltage_limit) {
		foc->foc_config->voltage_sensor_align = foc->foc_config->voltage_limit;
	}
    if (ShaftAngle(foc) == 0.0) {
        return EBUSY;
    }
	SetPhaseVoltage(foc, 0, 0, 0);
	DelayMs(foc, 10);
    ShaftVelocity(foc, foc->time_prev, &foc->time_prev, foc->angle_prev, &foc->angle_prev);
	foc->foc_config->DelayMs(5);
	foc->shaft_angle = ShaftAngle(foc);
	return EOK;
}

RETURN_CODE FOCMove(FOC *foc, float target) {
    switch(foc->foc_config->controller){
        case Type_velocity_openloop:
            foc->shaft_angle = ShaftTargetAngle(foc, target, foc->time_prev, &foc->time_prev);
            foc->voltage.q = foc->foc_config->voltage_limit;
            foc->voltage.d = 0;
            break;
        case Type_velocity_openloop_angle:
            foc->shaft_angle = ShaftAngle(foc);
            foc->voltage.q = target;
            foc->voltage.d = 0;
            break;
    }
    float electrical_angle = foc->shaft_angle * foc->foc_config->pole_pairs - foc->foc_config->zero_electric_angle;
    // ShaftVelocity(foc, foc->time_prev, &foc->time_prev, foc->angle_prev, &foc->angle_prev);
    // OUTPUT("e = %f,\n", electrical_angle);
    // OUTPUT("a = %f,\n", foc->shaft_angle);
    SetPhaseVoltage(foc, foc->voltage.q, foc->voltage.d, electrical_angle);
    return EOK;
}

void FOCTestPolePairs(FOC *foc, int pole_pairs) {
    int num = pole_pairs * 500;
    float num_anage = _2PI * pole_pairs ;
    SetPhaseVoltage(foc, foc->foc_config->voltage_sensor_align, 0, 0);
    foc->foc_config->DelayMs(100);
	for (int i = 0; i <= num; i++) {
		float angle = num_anage * i / num;
		SetPhaseVoltage(foc, foc->foc_config->voltage_sensor_align, 0, angle);
		foc->foc_config->DelayMs(2);
	}
    SetPhaseVoltage(foc, 0, 0, 0);
}

float FOCTestZeroElectricAngle(FOC *foc) {
    if (foc->foc_config->pole_pairs == 0) {
        return -1;
    }
    SetPhaseVoltage(foc, foc->foc_config->voltage_sensor_align, 0, _3PI_2);
    foc->foc_config->DelayMs(700);
    SetPhaseVoltage(foc, 0, 0, 0);
    return _normalizeAngle(ShaftAngle(foc)*foc->foc_config->pole_pairs);
}
