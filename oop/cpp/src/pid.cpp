// pid.cpp
// dew.ninja  Oct 2022
// implement pid controller using object-oriented approach

#include "pid.h"

PID::PID(float Kp,float Ki,float Kd,float Kt,float N,float Wp,float Wd,float Ts,float offset)   {
    _kp = Kp;
    _ki = Ki;
    _kd = Kd;
    _kt = Kt;
    _n = N;
    _wp = Wp;
    _wd = Wd;
    _Ts = Ts;
    _output_offset = offset;
    this->coeffUpdate();
}

float PID::getKp()
{
    return _kp;
}

float PID::getKi()
{
    return _ki;
}

float PID::getKd()
{
    return _kd;
}

float PID::getKt()
{
    return _kt;
}


float PID::getN()
{
    return _n;
}

float PID::getWp()
{
    return _wp;
}

float PID::getWd()
{
    return _wd;
}

float PID::getTs()
{
    return _Ts;
}

void PID::setKp(float Kp)
{
    _kp = Kp;
    this->coeffUpdate();
}

void PID::setKi(float Ki)
{
    _ki = Ki;
    this->coeffUpdate();
}

void PID::setKd(float Kd)
{
    _kd = Kd;
    this->coeffUpdate();
}

void PID::setKt(float Kt)
{
    _kt = Kt;
    this->coeffUpdate();
}

void PID::setN(float N)
{
    _n = N;
    this->coeffUpdate();
}

void PID::setWp(float Wp)
{
    _wp = Wp;
    this->coeffUpdate();
}

void PID::setWd(float Wd)
{
    _wd = Wd;
    this->coeffUpdate();
}

void PID::setTs(float Ts)
{
    _Ts = Ts;
    this->coeffUpdate();
}


void PID::coeffUpdate()  // update PID coefficients
{
  double ad1, ad2, x;

  // coefficents of integral term
  bi = 0.5*_Ts*_ki;
  bt = 0.5*_Ts*_kt;
  x = 0.5*_n*_Ts;
  
  ad1 = 1+x;
  ad2 = x-1;
  ad = -ad2/ad1;
  bd = _kd*_n/ad1;

}

float PID::out(float r,float y)  // compute output
{
    e1 = e0;    // true error
    ed1 = ed0;    // derivative term error
    eus1 = eus0;  // saturation output difference
    
    ui1 = ui0;   // integral term output
    ud1 = ud0;   // derivative term output
  
    e0 = r - y;        // compute true error
    ep0 = _wp*r - y;    // proportional-term error
    ed0 = _wd*r - y;    // derivative-term error

    up0 = _kp*ep0;  // output of P tern
    ui0 = ui1 +bi*(e0+e1) + bt*(eus0+eus1); // output of I term
    ud0 = ad*ud1 + bd*(ed0-ed1);  // output of D term
    u0 = up0 + ui0 + ud0;
 
    if (u0>_output_offset)  {
      u0lim= _output_offset;       // limit PWM output
      eus0 = _output_offset - u0;   // back calculation (u-sat)
    }
    else if (u0<-_output_offset) {
      u0lim = -_output_offset;      // limit PWM output
      eus0 = -u0 - _output_offset;  // back calculation (u-sat)
    }
    else {
      u0lim = u0;
      eus0 = 0;
    }
    u0lim += _output_offset;
    return u0lim;
}

float PID::getLinearU()
{
    return u0;
}