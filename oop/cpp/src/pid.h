// pid.h
// dew.ninja  October 2022
// pid class defintion.

#ifndef PID_H
#define PID_H
#include <Arduino.h>

class PID   {
    private:
        // controller parameters
        float _kp;   // proportional gain
        float _ki;    // integral
        float _kd;    // derivative
        float _kt;    // back calculation gain
        float _wp;    // proportional weight
        float _wd;    // derivative weight
        float _n;    // D filter coeffieient
        float _Ts;   // sampling time

        // controller coefficients as functions of PID parameters
        float ad, bi, bd, bt;

        float ep0;              // proportional-term error
        float e1, e0;              // true error
        float eus0, eus1;              // u_sat error
        float ed1, ed0;    // derivative-term error
        float u0, up0, ui0, ui1, ud0, ud1, u0lim;     

        float y;     // plant output
        float r;     // ref. command value 
    
        // variables to set proper offset
        float _output_offset; 

    public:
        PID(float Kp,float Ki,float Kd,float Kt,float N,float Wp,float Wd,float Ts, float offset);

        // get parameters
        float getKp();
        float getKi();
        float getKd();
        float getKt();
        float getN();
        float getWp();
        float getWd();
        float getTs();

        // set parameters
        void setKp(float Kp);
        void setKi(float Ki);
        void setKd(float Kd);
        void setKt(float Kt);
        void setN(float N);
        void setWp(float Wp);
        void setWd(float Wd);
        void setTs(float Ts);

        void coeffUpdate();  // update controller coefficients
        float out(float r,float y);  // compute output
        float getLinearU();  // get pure linear output (used for plant simulation in linear mode)




};


#endif