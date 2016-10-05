/**
 * ESP8266-12E GoPro Wifi Remote
 * Lance Brummett 
 * This sketch is a modification of a work by mg-x (youtube) - which 
 * after many many searches I just happened to stumble upon.
 * 
 * Many, Many, Many thanks to you mg-x for the elegant and far simpler 
 * mode switch function. I've had 10 revisions of similar sw/case code
 * and (not being a coder) couldn't quite get the mode switch function reliably working. 
 * You've reminded me of a lesson I've stressed a million times in my years - keep it simple.
 * This saved me much stress.
 * Once again, many thanks.
 * 
 * I've added the Wake On lan function, and fixed the mode switch URL from ...xmode to ...mode, which fixed it
 * at least the version I had downloaded.
 * 
 * WOL Library = https://github.com/koen-github/WakeOnLan-ESP8266
 * Thank you!
 * 
 * You must add the ESP8266 Arduino Boards Manager from - File - Preferences - Settings.
 * Add the following to the "Additional Boards Manager Urls" Box:
 * http://arduino.esp8266.com/stable/package_esp8266com_index.json
 * 
 * 
 * This version will successfully :
 * Upon Power Up - connect to gp AP.
 * Set to Video Mode.
 * 
 * This assumes you're using the default Gopro AP address.
 * 
 * You MUST find your GP Mac Address. You can likely find an app somewhere.
 * The simplest way, if you're knowledgable about your internet router, and it's
 * advanced enough for you to log into and see connected devices (it should be at this point) - 
 * log into router, find connected devices, inspect the properties of "yourgoproname".
 * Mac must be enteried as shown below, with commas.
 * 
 * To Flash (since again - reliable info on the simplest things for these incredible little boards
 * is so difficult to find):
 * Select Generic ESP8266 as your board in boards mgr. 
 * Flash Mode - Dio. Flash Mem - whatever yours is. Flash Freq - 40mhz. Cpu Freq - 80mhz.
 * If your chip is new & never powered - start with baud at 9600. They seem to all have different
 * rates but I've found 9600 oo 115200 will most likely work.
 * 
 * Tie V+ to VCC and jumper to CP_EN/Chip_EN/EN - whichever your's is called.
 * Tie V- to Gnd and jumper to IO15.
 * Install a button, switch, or RELIABLY CONNECTED jumper between Gnd & IO2.
 * When ready to flash, be sure I02 is shorted to Gnd, and held low throughout the whole flash procedure; 
 * apply power; pull RST LOW/short to grd - momentarily.
 * This resets the chip into flash mode.
 * 
 */

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <WakeOnLan.h>

const int buttonPin1 = 4;
const int buttonPin2 = 5;
int buttonState1 = 0; 
int buttonState2 = 0; 

const char* ssid     = "insertyourgoproname";
const char* password = "insertyourgopropass";

const char* host = "10.5.5.9";

/**
 * Required for WOL
 */
WiFiUDP UDP;
IPAddress computer_ip(10, 5, 5, 9); 

/**
 * WOL - The targets MAC address to send the packet to
 * FIND AND REPLACE THE BELOW WITH YOUR GP MAC ADDRESS!!!
 */
byte mac[] = { 0xA1, 0xB2, 0xC3, 0xD4, 0xE5, 0x00 };

void setup() {
    digitalWrite(BUILTIN_LED, HIGH);
  Serial.begin(115200);
  delay(10);

  pinMode(buttonPin1, INPUT);
  pinMode(buttonPin2, INPUT);
  pinMode(BUILTIN_LED, OUTPUT);
  
  // We start by connecting to a WiFi network

  Serial.println();
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

Serial.println("");
  Serial.println("Turn On GP");  
  delay(500);

  /**
   * Send WOL Command to Turn on GP
   */

gpON(); 
return;
 delay(4000);
  
triggerModusVideo();
  digitalWrite(BUILTIN_LED, LOW);
}

int value = 0;
int sw_funct = 1;
int sw_trigger = 0;
void gpON(){
if (WiFi.status() ==WL_CONNECTED) 
    Serial.println("Sending WOL Packet...");

    WakeOnLan::sendWOL(computer_ip, UDP, mac, sizeof mac);
 //   delay(4000); //sending WOL packets every 4th second.
}
void loop() {

  buttonState1 = digitalRead(buttonPin1);
  buttonState2 = digitalRead(buttonPin2);

// FUNKTIONEN umschalten zw foto und video

  if (buttonState2 == 1){

switch (sw_funct){
  case 1:
       triggerModusPhoto();
       sw_funct = 2;
       delay(300);
  break;
  case 2:
       triggerModusVideo();
       sw_funct = 1;
       delay(300); 
  break;
}
}

  if (buttonState1 == 1){

switch (sw_funct){
  case 1:

  switch (sw_trigger){
    case 0:
    triggerStart();
    sw_trigger = 1;
    break;
    case 1:
    triggerStop();
    sw_trigger = 0;
    break;
    }

delay(300); 
  break;
  case 2:
triggerStart();
delay(300); 
  break;
}
}

  }

void triggerStart(){


  ++value;
  Serial.print("connecting to ");
  Serial.println(host);
  
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
   return;
  }
  
  // We now create a URI for the request
  String url = "/gp/gpControl/command/shutter?p=1";
  //String url = "/gp/gpControl/status"; 
  Serial.print("Requesting URL: ");
  Serial.println(url);
  
  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
  delay(10);
  
  // Read all the lines of the reply from server and print them to Serial
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
  
  Serial.println();
  Serial.println("closing connection");
  //return(1);
  //delay(5000);
}

void triggerStop(){


  ++value;
  Serial.print("connecting to ");
  Serial.println(host);
  
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
   return;
  }
  
  // We now create a URI for the request
  String url = "/gp/gpControl/command/shutter?p=0";
  //String url = "/gp/gpControl/status"; 
  Serial.print("Requesting URL: ");
  Serial.println(url);
  
  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
  delay(10);
  
  // Read all the lines of the reply from server and print them to Serial
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
  
  Serial.println();
  Serial.println("closing connection");
  //return(1);
  //delay(5000);
}

void triggerModusVideo(){


  ++value;
  Serial.print("connecting to ");
  Serial.println(host);
  
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
   return;
  }
  
  // We now create a URI for the request
  String url = "/gp/gpControl/command/mode?p=0";
  //String url = "/gp/gpControl/status"; 
  Serial.print("Requesting URL: ");
  Serial.println(url);
  
  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
  delay(10);
  
  // Read all the lines of the reply from server and print them to Serial
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
  
  Serial.println();
  Serial.println("SWITCH to VIDEO");
  //return(1);
  //delay(5000);

}

void triggerModusPhoto(){


  ++value;
  Serial.print("connecting to ");
  Serial.println(host);
  
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
   return;
  }
  
  // We now create a URI for the request
  String url = "/gp/gpControl/command/mode?p=1";
  //String url = "/gp/gpControl/status"; 
  Serial.print("Requesting URL: ");
  Serial.println(url);
  
  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
  delay(10);
  
  // Read all the lines of the reply from server and print them to Serial
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
  
  Serial.println();
  Serial.println("SWITCH to PHOTO");
  //return(1);
  //delay(5000);
}
