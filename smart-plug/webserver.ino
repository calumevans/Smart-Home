
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>   

//------------------------DIGITAL PINS
#define annoyingLED     2  //D4
#define buttonTOGGLE   16  //D0
#define buttonALL       4  //D1
#define buttonSCENE     5  //D2

//for testing using LEDs
#define floorPIN       0   //D3
#define bedPIN         14  //D5
#define deskPIN        12  //D6


int cAddr = 0;                  //address where state information will be help
int data;                      //variable for the state information
int state = 1;

int masterSTATE;
int scene = 0;

const char* ssid = "XXXXXXXX";    //enter the WiFi SSID and password 
const char* password = "XXXXXXXXX";

ESP8266WebServer server(80); //Declaring a global object variable from the ESP8266WebServer on port 80

//---------------------------------------------------------------HTML SOURCE CODE
const char MAIN_page[] PROGMEM = R"=====(
<!doctype html>
<html lang="en">
<head>
  <TITLE>Smart Home</TITLE>
  <CENTER>
    <FONT FACE="arial"><H1>The Smart Home -Summer 2019</H1></FONT>
    <FONT FACE="arial"><H2>Calum Evans</H2></FONT>
  </CENTER>
</head>

<body>  
  <center>
    
    <h2><FONT FACE="arial"><a href="floor">Floor Lamp</a></FONT><h2><br>
    
    <h2><FONT FACE="arial"><a href="bed">Bedside Lamp</a></FONT><h2><br>
    
    <h2><FONT FACE="arial"><a href="desk">Desk Lamp</a></FONT><h2><br>
    
    <h2><FONT FACE="arial"><a href="all">Everything On</a></FONT><h2><br>

    <h2><FONT FACE="arial"><a href="off">Everything Off</a></FONT><h2><br>
    
  </center>  
  <hr>
</body>

</html>
)=====";
 
void handleRoot(){                       //when the ESP8266's IP address is opened
 String s = MAIN_page; //Read HTML contents
 server.send(200, "text/html", s); //Send web page
}

void handleFLOOR(){
 scene = 0;
 masterSTATE = 1;
 server.send(200, "text/html", "Floor lamp on");
}

void handleBED(){
 scene = 1;
 masterSTATE = 1;
 server.send(200, "text/html", "Bed lamp on");
}

void handleDESK(){
 scene = 2;
 masterSTATE = 1;
 server.send(200, "text/html", "Desk lamp on");
}

void handleALL(){
 scene = 3;
 masterSTATE = 1;
 server.send(200, "text/html", "All lights on");
}

void handleOFF(){                     //turn off ALL lights
 masterSTATE = 0;
 server.send(200, "text/html", "All lights off");
}

void checkButtons(){
  if(!digitalRead(buttonTOGGLE)){           //toggles the lights on and off
    if(masterSTATE){
      masterSTATE = 0;
    }else{
      masterSTATE = 1;
    }
    Serial.println("Button: TOGGLE");
  }else if(!digitalRead(buttonALL)){        //turning ALL lights on
    masterSTATE = 1;
    scene = 3;
  }else if(!digitalRead(buttonSCENE)){      //this cycles through individual lights
    scene++;
    if(scene >= 3){
      scene = 0;  
    }
  }
  delay(150);
}

void doRelays(){
  switch(scene){
    case 0:   //floor
      floorCONTROL(masterSTATE);
      deskCONTROL(0);
      bedCONTROL(0);
      
      digitalWrite(floorPIN, masterSTATE);
      digitalWrite(deskPIN, LOW);
      digitalWrite(bedPIN, LOW);
      break;
    case 1:   //bed
      floorCONTROL(0);
      deskCONTROL(0);
      bedCONTROL(masterSTATE);
      
      digitalWrite(floorPIN, LOW);
      digitalWrite(deskPIN, LOW);
      digitalWrite(bedPIN, masterSTATE);
      break;
    case 2:   //desk
      floorCONTROL(0);
      deskCONTROL(masterSTATE);
      bedCONTROL(0);
      
      digitalWrite(floorPIN, LOW);
      digitalWrite(deskPIN, masterSTATE);
      digitalWrite(bedPIN, LOW);
      break;
    case 3:   //all
      floorCONTROL(masterSTATE);
      deskCONTROL(masterSTATE);
      bedCONTROL(masterSTATE);
      
      digitalWrite(floorPIN, masterSTATE);
      digitalWrite(deskPIN, masterSTATE);
      digitalWrite(bedPIN, masterSTATE);
      break;
  }
}

void floorCONTROL(int state){
  if(state){
    Serial.write(2);    //50
  }else if(!state){
    Serial.write(3);    //51
  }
}

void deskCONTROL(int state){
  if(state){
    Serial.write(4);    //52
  }else if(!state){
    Serial.write(5);    //53
  }
}

void bedCONTROL()int state{
    if(state){
    Serial.write(6);    //54
  }else if(!state){
    Serial.write(7);    //55
  }
}

//---------------------------------------------------------------SETUP
void setup(){
  EEPROM.begin(512);
  
  pinMode(buttonTOGGLE, INPUT);
  pinMode(buttonALL, INPUT);
  pinMode(buttonSCENE, INPUT);
  
  pinMode(floorPIN, OUTPUT);
  pinMode(deskPIN, OUTPUT);
  pinMode(bedPIN, OUTPUT);
  
  WiFi.begin(ssid, password);     //Connect to your WiFi router

  while (WiFi.status() != WL_CONNECTED) {     // Wait for connection
    delay(500);
  }
 
  server.on("/", handleRoot);
  server.on("/floor", handleFLOOR);
  server.on("/bed", handleBED);
  server.on("/desk", handleDESK);
  server.on("/all", handleALL);
  server.on("/off", handleOFF);
 
  server.begin();                  //Start server
  pinMode(annoyingLED, OUTPUT);    //turn off blue led 
  digitalWrite(annoyingLED, HIGH);
}

//---------------------------------------------------------------LOOP
void loop(){
  server.handleClient();
  checkButtons();
  doRelays();
}
