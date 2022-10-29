#ifndef CMD_H
    #define CMD_H
    #include <Arduino.h>
    #include <PubSubClient.h>
    #include "common.h"
    #include "netpie.h"
    #include "pid.h"

    extern NETPIE netpie;
    extern PID pid;
    // extern PubSubClient client;
    // -------- global variables for command interpreter ----------
    extern String rcvdstring;   // string received from serial
    extern String cmdstring;   // command part of received string
    extern String  parmstring; // parameter part of received string
    extern float parmvalfloat;   //parameter variable (float)
    extern int parmvalint;       // parameter variable (int)
    extern bool newcmd;     // flag when new command received
    extern int sepIndex;       // index of seperator
    extern bool noparm;    // flag if no parameter is passed

    void cmdInt(void);     
#endif