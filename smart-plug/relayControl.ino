#include <EEPROM.h> 

#define input     12
#define relay      2

int incomingByte;
int pos, state;
int switchON, switchOFF;

int posAddress = 0;
int stateAddress = 1;

void checkInput(){        //used for wired control and direct buttons
  if(digitalRead(input)){
    digitalWrite(relay,HIGH);
  }else if(!digitalRead(input)){
    digitalWrite(relay,LOW);
  }
  delay(100);
}

void receiveXBEE(){       //receives serial data from XBee module
  while(Serial.available()){
    incomingByte = Serial.read();
    
    if(incomingByte == switchON){            //on
      digitalWrite(relay,HIGH);
      EEPROM.write(stateAddress, 1);
    }else if(incomingByte == switchOFF){     //off
      digitalWrite(relay,LOW);
      EEPROM.write(stateAddress, 0);
    }
    Serial.print("Received: ");
    Serial.println(incomingByte); 
  }
}

void checkPosition(){     //reads data from memory to establish which light it is
  pos = EEPROM.read(posAddress);
  switch(pos){
    case 1:   //floor 2,3
      switchON = 50;
      switchOFF = 51;
      Serial.println("Position: Floor");
      break;
    case 2:   //desk 4,5
      switchON = 52;
      switchOFF = 53;
      Serial.println("Position: Desk");
      break;
    case 3:   //bed 6,7
      switchON = 54;
      switchOFF = 55;
      Serial.println("Position: Bed");
      break;
    default:
      Serial.println("Incorrect position read");
      break;
  }
}

void initialState(){                    //when the device is turned on, go to previous state
  state = EEPROM.read(stateAddress);
  digitalWrite(relay,state);
}


void setup(){
  Serial.begin(9600);
  pinMode(input,INPUT);
  pinMode(relay,OUTPUT);

  checkPosition();
  initialState();
  
}

void loop(){
  //checkInput();
  receiveXBEE();
}
