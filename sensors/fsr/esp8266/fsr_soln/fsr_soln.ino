// FSR_SOLN.ino 
// Jan 2021 by dew.ninja
// course supplement for emb_iot
// - read voltage from force sentitive resistor
// convert to force in range 0 - 1 kg

#include <ESP8266WiFi.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


#define OLED_RESET -1
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

Adafruit_SSD1306 OLED(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


int advalue=0,advalue_prev = 0;    // A/D value read from A0
float ad2v = 3.3/1023.0;
float v=0;        // convert A/D value to voltage
float force=0;        // force in gram 



void setup() {

  pinMode(A0, INPUT);  
  Serial.begin(115200);
  Serial.println();

  OLED.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // setup OLED
  OLED.display(); // adafruit logo
  delay(2000);
  OLED.clearDisplay();
 
}

void loop() {
   advalue_prev = advalue;
   advalue = analogRead(A0);
   if (advalue != advalue_prev)  {  // print only when A/Dvalue changes
      Serial.print("A/D value = ");
      Serial.println(advalue);
      v = ad2v*advalue;
      Serial.print("Voltage = ");
      Serial.print(v);
      Serial.println(" Volts");
      force = -333.33*v + 1100;
      if (force<0) force=0;
      Serial.print("Force = ");
      Serial.print(force);
      Serial.println(" grams");      
   }

  showOLED();
  delay(1000);
}

// display on OLED
void showOLED()   {
  OLED.clearDisplay();
  OLED.setTextSize(2);
  OLED.setTextColor(WHITE);
  OLED.setCursor(0,0);
  OLED.print("V: ");
  OLED.print(v);
  OLED.println(" V");
  OLED.print("F: ");
  OLED.print(force);
  OLED.println(" g");
  OLED.setTextSize(1);
  OLED.setTextColor(WHITE);  
  OLED.println("----------");
  OLED.println("NETPIE IoT");
  OLED.display();

}
