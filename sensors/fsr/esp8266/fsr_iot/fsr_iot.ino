// FSR_IOT.ino 
// Jan 2021 by dew.ninja
// course supplement for emb_iot
// - read voltage from force sentitive resistor
// convert to force in range 0 - 1 kg
// and publish to NETPIE 2020 


#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


#define OLED_RESET -1
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Fill in your WiFi and NETPIE information
const char* ssid = "";
const char* password = "";
const char* mqtt_server = "broker.netpie.io";
const int mqtt_port = 1883;
const char* mqtt_Client = "";
const char* mqtt_username = "";
const char* mqtt_password = "";

WiFiClient espClient;
PubSubClient client(espClient);

Adafruit_SSD1306 OLED(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

char msg[100];

int advalue=0,advalue_prev = 0;    // A/D value read from A0
float ad2v = 3.3/1023.0;
float v=0;        // convert A/D value to voltage
float force=0;        // force in gram 


void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection…");
    if (client.connect(mqtt_Client, mqtt_username, mqtt_password)) {
      Serial.println("connected");
    
    }
    else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println("try again in 5 seconds");
      delay(5000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String message;
  for (int i = 0; i < length; i++) {
    message = message + (char)payload[i];
  }
  Serial.println(message);
//  if(String(topic) == "@msg/cmd") {
//    rcvdstring=message; 
//    cmdInt();  // call command interpreter
//  }
}



void setup() {

  pinMode(A0, INPUT);  
  Serial.begin(115200);
  Serial.println();
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
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
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
   // write data to NETPIE 
   if (client.connected())  {
      client.loop();
      
      String data = "{\"data\": {\"force\":" + String(force)+"}}";
      Serial.println(data);
      data.toCharArray(msg, (data.length() + 1));
      client.publish("@shadow/data/update", msg);

  }   
  else reconnect(); 
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
