#include <Arduino.h>
// pid_netpie_basic  PlatformIO project
// dew.ninja  Oct 2022


#include "common.h"
#include "cmd.h"
#include "netpie.h"
#include "pid.h"

// --- fill in your wifi and NETPIE information ----
char* ssid = "";
char* password = "";
char* mqtt_Client = "";
char* mqtt_username = "";
char* mqtt_password = "";
//------------------------------------------------

WiFiClient espClient;
PubSubClient client(espClient);
NETPIE netpie(mqtt_Client,mqtt_username,mqtt_password, client);  // create netpie object

unsigned long NETPIE_PERIOD = 1000;  // period to check netpie connection
unsigned long netpie_millis = 0,netpie_millis_prev = 0;
unsigned long t_current = 0, t_prev = 0;  // for sampling period control

char msg[255], cspmsg[255];
String datastr;       // data string used in command section
// -------- variable declarations  ----------
const int PWMOut = 16;    // use GPIO16 as PWM
const int ADCin = A3;   
int SQWOut = 5;  // 2 for NODEMCU-32S, DO-IT, 5 for WEMOS LOLIN

const int DATASIZEMAX = 5000;   // maximum data size
const int DATASAMPMAX = 100;     // maximum sampling selection

// voltage range
const float VMAX = 3.3;
const float VMID = 1.65;
const float VMIN = 0;

// PWM and ADC ranges 
const int PWMMAX = 4095;  // 12-bit PWM
const int PWMMID = 2047;
const int PWMMIN = 0;
const int ADCMAX = 4095;   // 12-bit ADC
const int ADCMID = 2047;
const int ADCMIN = 0;

float raw2v = VMAX/ADCMAX;   // 12-bit raw unit to volt

// controller parameter limits
const float KPMAX = 10000;  // proportional gain
const float KIMAX = 10000;  // integral gain
const float KDMAX = 10000;  // derivative gain
const float KTMAX = 10; 
const float WPMAX = 1;  // proportional weight
const float WDMAX = 1;  // derivative weight
const float NMAX = 500;  // derivative filter coefficient

// controller parameters
float kp = 4.8;   // proportional gain
float ki = 2.74;    // integral
float kd = 2.1;    // derivative
float kt = 0;    // back calculation gain
float wp = 1;    // proportional weight
float wd = 1;    // derivative weight
float N = 50;    // D filter coeffieient
float T = 0.08;       // sampling period

PID pid(kp,ki,kd,kt,N,wp,wd,T,(float)PWMMID);

// controller coefficients as functions of PID parameters
//float ad, bi, bd, bt;

//float ep0 = 0;              // proportional-term error
//float e1 = 0, e0 = 0;              // true error
//float eus0 = 0, eus1=0;              // u_sat error
//float ed1 = 0, ed0 = 0;    // derivative-term error
float u0 = 0, u0lim = 0, u0lim_raw=0;     

float y = 0;     // plant output (in volts)
float y_raw = 0;  // in ADC unit
float r, rold;     // command value (in volts)
float r_raw, r_raw_old;  // (in ADC value)

// --------------- plant simulation variables ----------------------
struct PlantSim
{
  float T;      // sampling period
  float Tau1,Tau2, Tau3;    // lag parameters
  float K;                  // overall gain
  float a11, a21, a31, b11,b21,b31;   // coefficients
  float x1_1,x1_0, x2_1, x2_0, x3_1, x3_0;       // states
  float u0, u1;           // input (from controller output)
  bool ulim;          // input limit flag
  
};

struct PlantSim PSim;   // create instance
int dataidx = 0, datasampidx = 0;          // data index
int datasize = 200;  // number of data points
int datasamp = 1;     // data sampling selection

int datacapt = 0;
int verboseflag = 1;    // response verbosity

int adcval = 0;         // ADC input value
int pwmval = 0;       // pwm value
int dacval = 0;       // DAC1 value

// ---- flag variables -------------------
bool adma=0;            // adma flag
bool oled=0, rgbled = 1;    // flag for OLED panel and RGBLED
bool netpie_flag = 1;            // NETPIE flags
bool plantsim = 0;      // plant simulation flag
bool freeboardupdated = 0; // flag whether freeboard is updated or not
// ---------------------------------------


int ADVal[4]={0,0,0,0};  // keep 4 samples for averaging
//int ADVala;            // average of A/D value

//int SQWstatus = 0;      // status of square wave
int RLED, GLED, BLED;   // pin numbers for RGB led
int Rval=PWMMAX, Gval=0, Bval=0;   // values for RGB color
unsigned int RGBtindex = 0;

int sqwcnts = 0;    // counter for SQW LED
// timer assignment
//const byte Timer0=0;
const byte PWMch=1;
const byte REDch = 2;
const byte GREENch = 3;
const byte BLUEch = 4;


float tdata = 0; // time data sent to output

//bool controltype = PID;    // controller type 0 = PID, 1 = CC

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String message;
  for (unsigned int i = 0; i < length; i++) {
    message = message + (char)payload[i];
  }
  Serial.println(message);
  if(String(topic) == "@msg/cmd") {
    rcvdstring=message; 
    cmdInt();  // call command interpreter
  }
}


// void PID_update(void) // update controller coefficients
// {
//   double ad1, ad2, x;

//   // coefficents of integral term
//   bi = 0.5*T*ki;
//   bt = 0.5*T*kt;
//   x = 0.5*N*T;
  
//   ad1 = 1+x;
//   ad2 = x -1;
//   ad = -ad2/ad1;
//   bd = kd*N/ad1;
  
//   Serial.println("PID coefficients updated");
//   freeboardupdated = 0;
//   update_freeboard();
// }  

// initialize plant simulation variables
// at present, implement only P(s) = 1/(s+1)^3
void PSim_init(void) 
{
  float Tau1_T, Tau2_T, Tau3_T;

  PSim.T = T;      //sampling period (sec)
  PSim.ulim = 1;      // input limit OFF/ON
  PSim.Tau1 = 1;
  PSim.Tau2 = 1;
  PSim.Tau3 = 1; 
  PSim.K = 1;
  Tau1_T = PSim.Tau1*PSim.T;
  Tau2_T = PSim.Tau2*PSim.T;
  Tau3_T = PSim.Tau3*PSim.T;
  PSim.a11 = (2 - Tau1_T)/(2 + Tau1_T);
  PSim.a21 = (2 - Tau2_T)/(2 + Tau2_T);
  PSim.a31 = (2 - Tau3_T)/(2 + Tau3_T);

  PSim.b11 = PSim.T/(2+Tau1_T);
  PSim.b21 = PSim.T/(2+Tau2_T);  
  PSim.b31 = PSim.T/(2+Tau3_T);  
  PSim.x3_0 = 0;
  PSim.x2_0 = 0;
  PSim.x1_0 = 0;
  PSim.x3_1 = 0;
  PSim.x2_1 = 0;
  PSim.x1_1 = 0;
}

void PSim_compute(void)
{
    // update plant states
    PSim.u1 = PSim.u0;
    PSim.x3_1 = PSim.x3_0;
    PSim.x2_1 = PSim.x2_0;
    PSim.x1_1 = PSim.x1_0;
    PSim.x1_0 = PSim.a11*PSim.x1_1 + PSim.b11*(PSim.u0 + PSim.u1);
    PSim.x2_0 = PSim.a21*PSim.x2_1 + PSim.b21*(PSim.x1_0 + PSim.x1_1);
    PSim.x3_0 = PSim.a31*PSim.x3_1 + PSim.b31*(PSim.x2_0 + PSim.x2_1);
 
}

// set RGB led to specified color
void lidRGBled(int rval, int gval, int bval)
{ 
  
  ledcWrite(REDch, rval);
  ledcWrite(GREENch, gval);
  ledcWrite(BLUEch, bval); 
}



// lid RGB led according to output level
void esp_y2rgb(void)
{
  float yt;
  int ypwm, rval, gval, bval;

  if (y<=1.5)   {
    yt=y/1.5;
    rval =0;
    bval = int(PWMMAX*(1-yt));
    gval = int(PWMMAX*yt);
    lidRGBled(rval, gval, bval);
    
  }
  else if (y>1.5)  {
    yt = (y-1.5)/1.5;
    if (yt>1) yt=1;
    bval = 0;
    gval = int(PWMMAX*(1-yt));
    rval = int(PWMMAX*yt);
    lidRGBled(rval, gval, bval);
  }
}

// publish all parameters to update freeboard widgets
// whenever a parameter is changed
void update_freeboard(void)
{

  if (netpie_flag & (!freeboardupdated))   {
         String cspdatastr = "{\"data\": {\"ts\":" + String(pid.getTs())
         +", \"adma\":"+String(adma) +", \"kp\":"+String(pid.getKp())
         +", \"ki\":"+String(pid.getKi()) +", \"kd\":"+String(pid.getKd())
         +", \"kt\":"+String(pid.getKt()) +", \"wp\":"+String(pid.getWp())+", \"wd\":"+String(pid.getWd())
         +"}}";
         cspdatastr.toCharArray(cspmsg, (cspdatastr.length()+1));
         netpie.publish("@shadow/data/update", cspmsg);
         Serial.print("Publishing to shadow : ");
         Serial.println(cspmsg);  
         freeboardupdated = 1;  
  }

}


void setup() {
  // RGB led
  RLED = 19;
  GLED = 18;
  BLED = 17;
  pinMode(PWMOut, OUTPUT);
  pinMode(SQWOut, OUTPUT);
  pinMode(RLED, OUTPUT);
  pinMode(GLED, OUTPUT);
  pinMode(BLED, OUTPUT);    

  ledcSetup(PWMch, 5000, 12);   // PWM output
  ledcAttachPin(PWMOut, PWMch);


  ledcSetup(REDch, 5000, 12);   // Red LED
  ledcSetup(GREENch, 5000, 12);   // Green LED
  ledcSetup(BLUEch, 5000, 12);   // Blue LED

  ledcAttachPin(RLED, REDch);
  ledcAttachPin(GLED, GREENch);
  ledcAttachPin(BLED, BLUEch);  

  Serial.begin(115200);
  Serial.println();
  if (netpie_flag)   {
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    netpie.setServer();
    netpie.setCallback(callback);
  }
  //pid.coeffUpdate()
  //PID_update();  // update controller coefficients
  PSim_init();   // initialized plant simulation variables
  noparm = 0;
  newcmd = 0;

  r = 1;
  rold = 1;
  r_raw = r/raw2v;
  r_raw_old = r_raw;


  delay(5000);
  freeboardupdated = 0;
  update_freeboard();

}

void loop() {


  t_current = millis();
  if ((t_current - t_prev)>=1000*T)   { // sampling period exceeded
    t_prev = t_current;
    sqwcnts++;
    if (sqwcnts == 20)   { // toggle LED each 200 ms
      digitalWrite(SQWOut,!digitalRead(SQWOut)); // toggle square wave output
      sqwcnts=0;
    }

    if (netpie_flag)   {
       if ((t_current-netpie_millis_prev)>NETPIE_PERIOD) {
        netpie.stayconnected();
        netpie_millis_prev = t_current;
        // write NETPIE shadow
        String data = "{\"data\": {\"y\":" + String(y) + ", \"u\":" + String(u0lim)+"}}";
        //Serial.println(data);
        data.toCharArray(msg, (data.length() + 1));
        netpie.publish("@shadow/data/update", msg);        

      }
    }
    if (plantsim)   {
      PSim_compute();
      y_raw = PSim.x3_0;
    }
    else   {
     // A/D averaging
      ADVal[3]=ADVal[2];
      ADVal[2]=ADVal[1];
      ADVal[1]=ADVal[0];
      ADVal[0]=analogRead(ADCin);   // read new analog input
      if (adma)
        adcval = (ADVal[0]+ADVal[1]+ADVal[2]+ADVal[3])>>2;  // average of 4 samples
      else adcval = ADVal[0];
    
      y_raw = adcval;
      y = raw2v*adcval;          // convert to volt. This is sent as data for plotting.
    }    

    u0lim_raw = pid.out(r_raw,y_raw); 

    //compute PID

/*     e1 = e0;    // true error
    ed1 = ed0;    // derivative term error
    eus1 = eus0;  // saturation output difference
    
    ui1 = ui0;   // integral term output
    ud1 = ud0;   // derivative term output
  
    e0 = r_raw - y_raw;        // compute true error
    ep0 = wp*r_raw - y_raw;    // proportional-term error
    ed0 = wd*r_raw - y_raw;    // derivative-term error

    if (controltype==PID)   {
      up0 = kp*ep0;  // output of P tern
      ui0 = ui1 +bi*(e0+e1) + bt*(eus0+eus1); // output of I term
      ud0 = ad*ud1 + bd*(ed0-ed1);  // output of D term
      u0 = up0 + ui0 + ud0;
    }
    // else   {    // controltype == CC
    //   if(cctype == OFB)   {
    //     x[2] = x[1];
    //     x[1] = x[0];
    //     x[0]= e0 - a[1]*x[1] - a[2]*x[2];
    //     u0 = b[0]*x[0] + b[1]*x[1] + b[2]*x[2];   
    //   }
    // }
    if (u0>PWMMID)  {
      u0lim_raw = PWMMID;       // limit PWM output
      eus0 = PWMMID - u0;   // back calculation (u-sat)
    }
    else if (u0<-PWMMID) {
      u0lim_raw = -PWMMID;      // limit PWM output
      eus0 = -u0 - PWMMID;  // back calculation (u-sat)
    }
    else {
      u0lim_raw = u0;
      eus0 = 0;
    }
    u0lim_raw += PWMMID; */
    pwmval = int(u0lim_raw);
    u0lim = raw2v*u0lim_raw;  // in volts    
 
    if (plantsim)   {
      if (PSim.ulim)  {  // use limiter 
        //PSim.u0 = u0lim;
        PSim.u0 = u0lim_raw;
      }
      else  {  // pure linear system

        PSim.u0 = pid.getLinearU();
      }
    }
    else   {
      // send output to both DAC1 and PWM16
      ledcWrite(PWMch, pwmval);
      dacval = pwmval>>4; //int(pwmval/8);
      dacWrite(DAC1, dacval);
    }

    if (rgbled) esp_y2rgb();  // change RGB led color

    if (datacapt)  // send data to serial output
    {
      Serial.print("datamat = np.array([");
      datasampidx++;
      tdata = 0;
      dataidx = 0;
      if (datasampidx == datasamp) {
        tdata += T;  // update time data
        Serial.print("[");
        Serial.print(tdata);
        Serial.print(", ");
        Serial.print(r);
        Serial.print(", ");
        Serial.print(y);   
        Serial.print(", ");
        Serial.print(u0lim);
        Serial.println("],");   
      }
      datasampidx = 0;
      dataidx++;

    }
    if (dataidx == datasize)  // end data capture
    {
      Serial.println("])");
      datacapt = 0;
      dataidx = 0; // reset data index
    }

    while (Serial.available() > 0)   {  // detect new input
      rcvdstring = Serial.readString();
      newcmd = 1;
    }
    if (newcmd)   { // execute this part only when new command received
      cmdInt();   // invoke the interpreter
      newcmd = 0;
    }    
  }
}

