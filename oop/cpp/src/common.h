#ifndef COMMON_H
    #define COMMON_H
    #include <WiFi.h>
    #include <PubSubClient.h>
     #include <Wire.h>
 
//    #define PID 0      // controller = PID
//    #define CC 1      // controler = CC (custom controller)

    extern char msg[],cspmsg[];
    extern String datastr;       // data string used in command section

    // -------- variable declarations  ----------
    extern const int PWMOut;    // use GPIO16 as PWM
    extern const int ADCin;   
    extern int SQWOut;  // 2 for NODEMCU-32S, DO-IT, 5 for WEMOS LOLIN

    extern const int DATASIZEMAX;   // maximum data size
    extern const int DATASAMPMAX;     // maximum sampling selection

    // voltage range
    extern const float VMAX;
    extern const float VMID;
    extern const float VMIN;

    // PWM and ADC ranges 
    extern const int PWMMAX;  // 12-bit PWM
    extern const int PWMMID;
    extern const int PWMMIN;
    extern const int ADCMAX;   // 12-bit ADC
    extern const int ADCMID;
    extern const int ADCMIN;

    extern float raw2v;   // 12-bit raw unit to volt

    // controller parameter limits
    extern const float KPMAX;  // proportional gain
    extern const float KIMAX;  // integral gain
    extern const float KDMAX;  // derivative gain
    extern const float KTMAX; 
    extern const float WPMAX;  // proportional weight
    extern const float WDMAX;  // derivative weight
    extern const float NMAX;  // derivative filter coefficient

    // controller parameters
    extern float kp;   // proportional gain
    extern float ki;    // integral
    extern float kd;    // derivative
    extern float kt;    // back calculation gain
    extern float wp;    // proportional weight
    extern float wd;    // derivative weight
    extern float N;    // D filter coeffieient

    // controller coefficients as functions of PID parameters
    // extern float ad, bi, bd, bt;

    // extern float ep0;              // proportional-term error
    // extern float e1, e0;              // true error
    // extern float eus0 = 0, eus1=0;              // u_sat error
    // extern float ed1 = 0, ed0 = 0;    // derivative-term error
    // extern float u0 = 0, up0=0, ui0=0, ui1=0, ud0 = 0, ud1=0, u0lim = 0, u0lim_raw=0;     
    extern float u0lim;
    extern float y;     // plant output (in volts)
    extern float y_raw;  // in ADC unit
    extern float r, rold;     // command value (in volts)
    extern float r_raw, r_raw_old;  // (in ADC value)

    // --------------- plant simulation variables ----------------------
    // struct Psim;
 

    // extern struct PlantSim PSim;   // create instance
    extern int dataidx, datasampidx;          // data index
    extern int datasize;  // number of data points
    extern int datasamp;     // data sampling selection

    extern int datacapt;
    extern int verboseflag;    // response verbosity

    extern int adcval;         // ADC input value
    extern int pwmval;       // pwm value
    extern int dacval;       // DAC1 value

    // ---- flag variables -------------------
    extern bool adma;            // adma flag
    extern bool oled, rgbled;    // flag for OLED panel and RGBLED
    extern bool netpie_flag;            // NETPIE usage flags
    extern bool plantsim;      // plant simulation flag
    extern bool freeboardupdated; // flag whether freeboard is updated or not
    // ---------------------------------------


    extern int ADVal[4];  // keep 4 samples for averaging

    extern int RLED, GLED, BLED;   // pin numbers for RGB led
    extern int Rval, Gval , Bval;   // values for RGB color
    extern unsigned int RGBtindex;

    extern int sqwcnts;    // counter for SQW LED
    // timer assignment
    //const byte Timer0=0;
    extern const byte PWMch;
    extern const byte REDch;
    extern const byte GREENch;
    extern const byte BLUEch;

    extern float T;       // sampling period
    extern float tdata; // time data sent to output

    extern void update_freeboard(void);

#endif
