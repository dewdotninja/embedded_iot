#include "cmd.h"

String rcvdstring;   // string received from serial
String cmdstring;   // command part of received string
String  parmstring; // parameter part of received string
float parmvalfloat;   //parameter variable (float)
int parmvalint;       // parameter variable (int)
bool newcmd;     // flag when new command received
int sepIndex;       // index of seperator
bool noparm;    // flag if no parameter is passed


void cmdInt(void)
{
 
    rcvdstring.trim();  // remove leading&trailing whitespace, if any
    // find index of separator ('=')
    sepIndex = rcvdstring.indexOf('=');
    if (sepIndex==-1) {  // no parameter in command
      cmdstring = rcvdstring;
      noparm = 1;   
    }
    else  {
    // extract command and parameter
      cmdstring = rcvdstring.substring(0, sepIndex);
      cmdstring.trim();
      parmstring = rcvdstring.substring(sepIndex+1); 
      parmstring.trim();
      noparm = 0;
    }
    // check if received command string is a valid command
    if ((cmdstring.equalsIgnoreCase("step"))|(cmdstring.equalsIgnoreCase("r")))   {
      // step without sending data
      if (noparm==1)   {  // step to 1
        r = 1;
        rold = r;
        r_raw_old = r_raw;
        r_raw = r/raw2v;
      }      
      else  {  // step to new spedified value
         parmvalfloat = parmstring.toFloat();

        //limit step command to 0 - 3 volts
        if (parmvalfloat > VMAX) parmvalfloat = VMAX; 
        else if (parmvalfloat< VMIN) parmvalfloat = VMIN;
        rold = r;   // save previous command
        r = parmvalfloat;
        r_raw_old = r_raw;     
        r_raw = r/raw2v;
      }
      if (netpie_flag)   {
       // update r value on shadow
       datastr = "{\"data\": {\"r\":" + String(r)+"}}";
       datastr.toCharArray(msg, (datastr.length() + 1));
       netpie.publish("@shadow/data/update", msg);
      }
    }
    
    else if ((cmdstring.equalsIgnoreCase("stepdata"))|(cmdstring.equalsIgnoreCase("rdata")))   {
    // step and send data to serial
      if (noparm==1)   {  // step to 1
        rold = r;
        r = 1;
        r_raw_old = r_raw;
        r_raw = r/raw2v;
      }    
      else  {  // step to new specified value
         parmvalfloat = parmstring.toFloat();

        //limit step command to 0 - 3 volts
        if (parmvalfloat > VMAX) parmvalfloat = VMAX; 
        else if (parmvalfloat< VMIN) parmvalfloat = VMIN;
        rold = r;   // save previous command
        r = parmvalfloat;
        r_raw_old = r_raw;        
        r_raw = r/raw2v;
      }
      if (netpie_flag)   {
       // update r value on shadow
       datastr = "{\"data\": {\"r\":" + String(r)+"}}";
       datastr.toCharArray(msg, (datastr.length() + 1));
       netpie.publish("@shadow/data/update", msg);
      }      
      //Serial.println("datamat = [");
      //Serial.println("datamat = np.array([");
      //tdata = 0;  // reset time data variable
      
      datacapt = 1;    // set the flag to capture data
      dataidx = 0;              // reset data index
    }
    else if (cmdstring.equalsIgnoreCase("stepmin"))   {
      // step to min r value of about 0.05. No serial data
      Serial.println("Set ref.cmd to minimum value of 0.1");
      rold = r;
      r = 0.1;
      r_raw_old = r_raw;   
      r_raw = r/raw2v;
      if (netpie_flag)   {
       // update r value on shadow
       datastr = "{\"data\": {\"r\":" + String(r)+"}}";
       datastr.toCharArray(msg, (datastr.length() + 1));
       netpie.publish("@shadow/data/update", msg);
      }
         
    }
    else if (cmdstring.equalsIgnoreCase("ryu"))   {
       // send r,y,u values
       Serial.print(r); 
       Serial.print(",");
       Serial.print(y);
       Serial.print(",");
       Serial.println(u0lim);   
 
    }    
    // else if (cmdstring.equalsIgnoreCase("psimdata"))   {
    //    // send PSim variables, for debugging purpose
    //    Serial.print("a11 = ");
    //    Serial.println(PSim.a11); 
    //    Serial.print("a21 = ");
    //    Serial.println(PSim.a21); 
    //    Serial.print("a31 = ");
    //    Serial.println(PSim.a31);  
    //    Serial.print("b11 = ");
    //    Serial.println(PSim.b11); 
    //    Serial.print("b21 = ");
    //    Serial.println(PSim.b21); 
    //    Serial.print("b31 = ");
    //    Serial.println(PSim.b31);    
    //    Serial.print("x1 = ");
    //    Serial.println(PSim.x1_0); 
    //    Serial.print("x2 = ");
    //    Serial.println(PSim.x2_0); 
    //    Serial.print("x3 = ");
    //    Serial.println(PSim.x3_0);            
    // }    
    else if (cmdstring.equalsIgnoreCase("datasize"))   {
      if (noparm==1)   {
        if (verboseflag) Serial.print("Current datasize = ");
        Serial.println(datasize);       
      }
      else   {  
        parmvalint = parmstring.toInt();
        if (parmvalint > DATASIZEMAX) parmvalint = DATASIZEMAX; // limit datasize to DATASIZEMAX
        else if (parmvalint<0) parmvalint = 0;
        datasize = parmvalint;  

      }
    }

    else if (cmdstring.equalsIgnoreCase("datasamp"))   {
      if (noparm==1)   {
        if (verboseflag) Serial.print("Current datasamp = ");
        Serial.println(datasamp);       
      }
      else   {  
        parmvalint = parmstring.toInt();
        if (parmvalint > DATASAMPMAX) parmvalint = DATASAMPMAX; // limit datasamp to DATASAMPMAX
        else if (parmvalint<0) parmvalint = 0;
        datasamp = parmvalint;  

      }
    }

    else if (cmdstring.equalsIgnoreCase("kp"))   {
      if (noparm == 1)   {
        if (verboseflag) Serial.print("Current kp = ");
        Serial.println(pid.getKp());           
      }
      else   {  
        parmvalfloat = parmstring.toFloat();
        if (parmvalfloat > KPMAX) parmvalfloat = KPMAX; // limit kp value
        else if (parmvalfloat<0) parmvalfloat = 0;
        float kp = parmvalfloat;  
        if (verboseflag)   {
          // echo value to console      
          Serial.print("new kp = ");
          Serial.println(kp);
        }
        pid.setKp(kp);    // update controller coefficients
        freeboardupdated = 0;
        update_freeboard();
      }
    }

    else if (cmdstring.equalsIgnoreCase("ki"))   {
      if (noparm == 1)   {
        if (verboseflag) Serial.print("Current ki = ");
        Serial.println(pid.getKi());           
      }
      else   {  
        parmvalfloat = parmstring.toFloat();
        if (parmvalfloat > KIMAX) parmvalfloat = KIMAX; // limit ki value
        else if (parmvalfloat<0) parmvalfloat = 0;
        float ki = parmvalfloat;  
        if (verboseflag)   {
          // echo value to console      
          Serial.print("new ki = ");
          Serial.println(ki);
        }
        pid.setKi(ki);    // update controller coefficients       
        freeboardupdated = 0;
        update_freeboard();
      }
         
    }
    else if (cmdstring.equalsIgnoreCase("kd"))   {
      if (noparm == 1)   {
        if (verboseflag) Serial.print("Current kd = ");
        Serial.println(pid.getKd());           
      }
      else   {  
        parmvalfloat = parmstring.toFloat();
        if (parmvalfloat > KDMAX) parmvalfloat = KDMAX; // limit kd value
        else if (parmvalfloat<0) parmvalfloat = 0;
        float kd = parmvalfloat;  
        if (verboseflag)   {
          // echo value to console      
          Serial.print("new kd = ");
          Serial.println(kd);
        }
        pid.setKd(kd);    // update controller coefficients
        freeboardupdated = 0;
        update_freeboard();
      } 
     }

    else if (cmdstring.equalsIgnoreCase("kt"))   {
      if (noparm == 1)   {
        if (verboseflag) Serial.print("Current kt = ");
        Serial.println(pid.getKt());           
      }
      else   {  
        parmvalfloat = parmstring.toFloat();
        if (parmvalfloat > KTMAX) parmvalfloat = KTMAX; // limit kt value
        else if (parmvalfloat<0) parmvalfloat = 0;
        float kt = parmvalfloat;  
        if (verboseflag)   {
          // echo value to console      
          Serial.print("new kt = ");
          Serial.println(kt);
        }
        pid.setKt(kt);    // update controller coefficients
        freeboardupdated = 0;
        update_freeboard();
      } 
     }     
     
       else if (cmdstring.equalsIgnoreCase("wp"))   {
      if (noparm == 1)   {
        if (verboseflag) Serial.print("Current wp = ");
        Serial.println(pid.getWp());           
      }
      else   {  
        parmvalfloat = parmstring.toFloat();
        if (parmvalfloat > WPMAX) parmvalfloat = WPMAX; // limit wp value
        else if (parmvalfloat<0) parmvalfloat = 0;
        float wp = parmvalfloat;  
        if (verboseflag)   {
          // echo value to console      
          Serial.print("new wp = ");
          Serial.println(wp);
        }
        pid.setWp(wp);    // update controller coefficients
        freeboardupdated = 0;
        update_freeboard();
      } 
     }  
     
     else if (cmdstring.equalsIgnoreCase("wd"))   {
      if (noparm == 1)   {
        if (verboseflag) Serial.print("Current wd = ");
        Serial.println(pid.getWd());           
      }
      else   {  
        parmvalfloat = parmstring.toFloat();
        if (parmvalfloat > WDMAX) parmvalfloat = WDMAX; // limit wd value
        else if (parmvalfloat<0) parmvalfloat = 0;
        float wd = parmvalfloat;  
        if (verboseflag)   {
          // echo value to console      
          Serial.print("new wd = ");
          Serial.println(wd);
        }
        pid.setWd(wd);    // update controller coefficients
        freeboardupdated = 0;
        update_freeboard();
      } 
     }        

     else if (cmdstring.equalsIgnoreCase("n"))   {
      if (noparm == 1)   {
        if (verboseflag) Serial.print("Current N = ");
        Serial.println(pid.getN());           
      }
      else   {  
        parmvalfloat = parmstring.toFloat();
        if (parmvalfloat > NMAX) parmvalfloat = NMAX; // limit N value
        else if (parmvalfloat<0) parmvalfloat = 0;
        float N = parmvalfloat;  
        if (verboseflag)   {
          // echo value to console      
          Serial.print("new N = ");
          Serial.println(N);
        }
        pid.setN(N);    // update controller coefficients
        freeboardupdated = 0;
        update_freeboard();
      } 
     } 
     else if (cmdstring.equalsIgnoreCase("adma"))   {
        if (noparm == 1)   {
          if (verboseflag) Serial.print("Current ADMA = ");
          if (adma==0) Serial.println("OFF");   
          else Serial.println("ON");        
        }
        else   {  
          if (parmstring.equalsIgnoreCase("off"))  {
            adma = 0;
            if (verboseflag) Serial.println("ADMA set to OFF");
          }
          else if (parmstring.equalsIgnoreCase("on"))  {
            adma = 1;
            if (verboseflag) Serial.println("ADMA set to ON");
          }
          freeboardupdated = 0;
          update_freeboard();
        }
    }  

   
    // turn plant simulation ON/OFF
     else if (cmdstring.equalsIgnoreCase("psim"))   {
        if (noparm == 1)   {
          if (verboseflag) Serial.print("Current plant simulation = ");
          if (plantsim==0) Serial.println("OFF");   
          else Serial.println("ON");        
        }
        else   {  
          if (parmstring.equalsIgnoreCase("off"))  {
            plantsim = 0;
            
            // vTaskSuspend(xTask7h);  // suspends task 6 that simulates the plant           
            if (verboseflag) Serial.println("Plant simulation task suspended");
          }
          else if (parmstring.equalsIgnoreCase("on"))  {
 

            // vTaskResume(xTask7h); // resumes plant simulation

            plantsim = 1;
            if (verboseflag) Serial.println("Plant simulation thask resumed");
            
          }
          
        }
    }  


     // ---- adjust controller sampling period --------------
    //  else if (cmdstring.equalsIgnoreCase("t0"))   {
    //   if (noparm == 1)   {
    //     if (verboseflag) Serial.print("Current t0 = ");
    //     Serial.println(T);           
    //   }
    //   else   {  
    //     parmvalfloat = parmstring.toFloat();
    //     if (parmvalfloat >T0MAX) parmvalfloat = T0MAX; // limit t0 value
    //     else if (parmvalfloat<T0MIN) parmvalfloat = T0MIN;
    //     T = parmvalfloat;  
    //     T0_ms = 1000*T;
    //     if (verboseflag)   {
    //       // echo value to console      
    //       Serial.print("New controller period  = ");
    //       Serial.println(T);
    //     }
    //     T0ticks = pdMS_TO_TICKS(T0_ms); 
    //     PID_update();    // update controller coefficients
    //   } 
    // } 
    
    //  else if (cmdstring.equalsIgnoreCase("t0ms"))   {
    //   if (noparm == 1)   {
    //     if (verboseflag) Serial.print("Current t0 (ms) = ");
    //     Serial.println(T0_ms);           
    //   }
    //   else   {  
    //     parmvalint = parmstring.toInt();
    //     if (parmvalint >T0MSMAX) parmvalint = T0MSMAX; // limit t0 value
    //     else if (parmvalint<T0MSMIN) parmvalfloat = T0MSMIN;
    //     T0_ms = parmvalint;  
    //     T = 0.001*float(T0_ms);
    //     if (verboseflag)   {
    //       // echo value to console      
    //       Serial.print("New controller period (ms) = ");
    //       Serial.println(T0_ms);
    //     }
    //     T0ticks = pdMS_TO_TICKS(T0_ms); 
    //     PID_update();    // update controller coefficients
    //   } 
    // } 

    //  else if (cmdstring.equalsIgnoreCase("t1ms"))   {
    //   if (noparm == 1)   {
    //     if (verboseflag) Serial.print("Current t1 (ms) = ");
    //     Serial.println(T1_ms);           
    //   }
    //   else   {  
    //     parmvalint = parmstring.toInt();
    //     if (parmvalint >T1MSMAX) parmvalint = T1MSMAX; // limit t1 value
    //     else if (parmvalint<T1MSMIN) parmvalfloat = T1MSMIN;
    //     T1_ms = parmvalint;  
        
    //     if (verboseflag)   {
    //       // echo value to console      
    //       Serial.print("New T1 (ms) = ");
    //       Serial.println(T1_ms);
    //     }
    //     T1ticks = pdMS_TO_TICKS(T1_ms); 

    //   } 
    // }     

    //  else if (cmdstring.equalsIgnoreCase("t2ms"))   {
    //   if (noparm == 1)   {
    //     if (verboseflag) Serial.print("Current t2 (ms) = ");
    //     Serial.println(T2_ms);           
    //   }
    //   else   {  
    //     parmvalint = parmstring.toInt();
    //     if (parmvalint >T2MSMAX) parmvalint = T2MSMAX; // limit t2 value
    //     else if (parmvalint<T2MSMIN) parmvalfloat = T2MSMIN;
    //     T2_ms = parmvalint;  
        
    //     if (verboseflag)   {
    //       // echo value to console      
    //       Serial.print("New T2 (ms) = ");
    //       Serial.println(T2_ms);
    //     }
    //     T2ticks = pdMS_TO_TICKS(T2_ms); 

    //   } 
    // }     

    // else if (cmdstring.equalsIgnoreCase("t3ms"))   {
    //   if (noparm == 1)   {
    //     if (verboseflag) Serial.print("Current t3 (ms) = ");
    //     Serial.println(T3_ms);           
    //   }
    //   else   {  
    //     parmvalint = parmstring.toInt();
    //     if (parmvalint >T3MSMAX) parmvalint = T3MSMAX; // limit t3 value
    //     else if (parmvalint<T3MSMIN) parmvalfloat = T3MSMIN;
    //     T3_ms = parmvalint;  
        
    //     if (verboseflag)   {
    //       // echo value to console      
    //       Serial.print("New T3 (ms) = ");
    //       Serial.println(T3_ms);
    //     }
    //     T3ticks = pdMS_TO_TICKS(T3_ms); 

    //   } 
    // }     

    // else if (cmdstring.equalsIgnoreCase("t4ms"))   {
    //   if (noparm == 1)   {
    //     if (verboseflag) Serial.print("Current t4 (ms) = ");
    //     Serial.println(T4_ms);           
    //   }
    //   else   {  
    //     parmvalint = parmstring.toInt();
    //     if (parmvalint >T4MSMAX) parmvalint = T4MSMAX; // limit t4 value
    //     else if (parmvalint<T4MSMIN) parmvalfloat = T4MSMIN;
    //     T4_ms = parmvalint;  
        
    //     if (verboseflag)   {
    //       // echo value to console      
    //       Serial.print("New T4 (ms) = ");
    //       Serial.println(T4_ms);
    //     }
    //     T4ticks = pdMS_TO_TICKS(T4_ms); 

    //   } 
    // }     

    //  else if (cmdstring.equalsIgnoreCase("t5ms"))   {
    //   if (noparm == 1)   {
    //     if (verboseflag) Serial.print("Current t5 (ms) = ");
    //     Serial.println(T5_ms);           
    //   }
    //   else   {  
    //     parmvalint = parmstring.toInt();
    //     if (parmvalint >T5MSMAX) parmvalint = T5MSMAX; // limit t5 value
    //     else if (parmvalint<T5MSMIN) parmvalfloat = T5MSMIN;
    //     T5_ms = parmvalint;  
        
    //     if (verboseflag)   {
    //       // echo value to console      
    //       Serial.print("New T5 (ms) = ");
    //       Serial.println(T5_ms);
    //     }
    //     T5ticks = pdMS_TO_TICKS(T5_ms); 

    //   } 
    // }     


    //  else if (cmdstring.equalsIgnoreCase("t6ms"))   {
    //   if (noparm == 1)   {
    //     if (verboseflag) Serial.print("Current t6 (ms) = ");
    //     Serial.println(T6_ms);           
    //   }
    //   else   {  
    //     parmvalint = parmstring.toInt();
    //     if (parmvalint >T6MSMAX) parmvalint = T6MSMAX; // limit t6 value
    //     else if (parmvalint<T6MSMIN) parmvalfloat = T6MSMIN;
    //     T6_ms = parmvalint;  
        
    //     if (verboseflag)   {
    //       // echo value to console      
    //       Serial.print("New T6 (ms) = ");
    //       Serial.println(T6_ms);
    //     }
    //     T6ticks = pdMS_TO_TICKS(T6_ms); 

    //   } 
    // } 
    // receive custom control coefficients from user        
    // else if (cmdstring.equalsIgnoreCase("cccoeff"))   {
    //   int parmIndex = parmstring.indexOf(',');
    //   String numcoeffstr = parmstring.substring(0, parmIndex);
    //   numcoeffstr.trim();
    //   parmstring = parmstring.substring(parmIndex+1);
    //   int numcoeff = numcoeffstr.toInt();
    //   int parmcount = 0;
    //   while (parmcount < numcoeff)   { // a[i] coefficient
    //     parmIndex = parmstring.indexOf(',');
    //     coeffstr_a[parmcount] = parmstring.substring(0, parmIndex);
    //     parmstring = parmstring.substring(parmIndex+1);
    //     coeffstr_a[parmcount++].trim();
    //   }
    //   parmcount = 0;
    //   while (parmcount < numcoeff)   { // b[i] coefficient
    //     parmIndex = parmstring.indexOf(',');
    //     coeffstr_b[parmcount] = parmstring.substring(0, parmIndex);
    //     parmstring = parmstring.substring(parmIndex+1);
    //     coeffstr_b[parmcount++].trim();
    //   }
    //   // update control coefficients
    //   taskENTER_CRITICAL(&myMutex);
    //   parmcount = 0;
    //   while (parmcount < numcoeff)    {
    //     a[parmcount] = coeffstr_a[parmcount].toFloat();
    //     b[parmcount] = coeffstr_b[parmcount].toFloat();
    //     x[parmcount] = 0;    // reset controller state
    //     parmcount++; 
    //   }      
    //   taskEXIT_CRITICAL(&myMutex);
    //   Serial.println("Custom control coefficients updated");
    //   // for debug purposes
    //   Serial.println("==========================================");
    //   Serial.print("numcoeff = ");
    //   Serial.println(numcoeff);
    //   parmcount = 0;
    //   while (parmcount < numcoeff)   {
    //     Serial.print("a[");
    //     Serial.print(parmcount);
    //     Serial.print("]=");
    //     Serial.print(a[parmcount],12);
        
    //     Serial.print(", b[");
    //     Serial.print(parmcount);
    //     Serial.print("]=");
    //     Serial.println(b[parmcount],12);     
    //     parmcount++;   
    //   }

    // }
    else   {
      Serial.println("Invalid command");
    }  
}
