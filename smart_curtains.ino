#define THRESH               500  //not sure about this threshold yet
#define stepsPerRevolution  1500

#include <Stepper.h>
#include "IRremote.h"

//-------------------------------------------------PINS
#define LED         13
#define limitBottom 12
#define buttonO      7
#define buttonC      6
#define receiver     5
Stepper motor(stepsPerRevolution, 8, 10, 9, 11);

IRrecv irrecv(receiver);    // create instance of 'irrecv'
decode_results results;     // create instance of 'decode_results'

#define upperLimit  A0      //photoresistor, which is the upper limit switch


//-------------------------------------------------VARIABLES
char state;         //denotes if the curtains are fully opened or closed


//-------------------------------------------------FUNCTIONS
void FOOC(char dir){          //goes fully opened/closed
  if(dir == 'O'){     //told to open, and curtains are not open already
    while(state != 'O'){
      moveMotor('O');
      limitChecker();
    }
    Serial.println("Fully Opened");
    blinkX(3);
  }else if(dir == 'C'){   //told to close, and curtains are not closed already
    while(state != 'C'){
      moveMotor('C');
      limitChecker();
    }
    Serial.println("Fully Closed");
    blinkX(3);
  }else{
    Serial.println("Incorrent parameter passed to FOOC");
  }
}


void moveMotor(char dir){                  //tells motor to move
  if(dir =='O' && state != 'O'){        //move motors, as long as it is not already in that state
    motor.step(2);          //constant for moving, dictates how "fast" it goes
    Serial.println("moveMotor: Opening");
  }else if(dir == 'C' && state != 'C'){
    motor.step(-2);
    Serial.println("moveMotor: Closing");
  }else{
    Serial.print("moveMotor: Cannot move because curtains are already in that state");
    Serial.print("State: ");
    Serial.println(state);
  }
}


void checkButtons(){                        //buttons for "manually" opening or closing the curtains
  if(!digitalRead(buttonO) && digitalRead(buttonC)){        //opening
    moveMotor('O'); 
    Serial.println("Button: Opening");
  }else if(digitalRead(buttonO) && !digitalRead(buttonC)){  //closing
    moveMotor('C'); 
    Serial.println("Button: Closing");
  }
}


void checkRemote(){                        //checks the IR receiver
  if(irrecv.decode(&results)){
    switch(results.value){
      case 0xFFA857: // VOL+
        moveMotor('O');
        Serial.println("Remote: Opening");
        break;
      case 0xFF629D: // VOL-
        moveMotor('C'); 
        Serial.println("Remote: Closing");
        break;
      case 0xFFC23D: //FAST FORWARD
        Serial.println("Remote: Fully Opening");
        FOOC('O');
        delay(200);     //need this delay here because we do not want multiple "presses" to register
        break;
      case 0xFF22DD:  //FAST BACKWARDS
        Serial.println("Remote: Fully Closing");
        FOOC('C');
        delay(200);
        break;  
    }
    irrecv.resume(); // receive the next value
      digitalWrite(8, LOW);
      digitalWrite(9, LOW);
      digitalWrite(10, LOW);
      digitalWrite(11, LOW);       
  } 
}


void blinkX(int num){         //blinks the led X number of times (I know there is already a function for this but I like mine)
  for(int i=0;i<num;i++){
    digitalWrite(LED,HIGH);
    delay(100);
    digitalWrite(LED,LOW);
    delay(100);
  }
}

void limitChecker(){                      //changes the global variable to state "open" or "closed"
  if(!digitalRead(limitBottom)){
    state = 'C';
    blinkX(1);
    Serial.println("Limit: Bottom achieved");
  }else if(analogRead(upperLimit) > THRESH){
    state = 'O';
    blinkX(1);
    Serial.println("Limit: Top achieved");
  }
}


void setup(){
  Serial.begin(9600);
  motor.setSpeed(100);    // set the speed at X rpm, maybe higher?? appearantly can go up to 500
  irrecv.enableIRIn();        // Start the receiver
  
  pinMode(LED,OUTPUT);
  pinMode(limitBottom,INPUT);
  pinMode(buttonO, OUTPUT);
  pinMode(buttonC, OUTPUT);
}


void loop(){
  checkRemote();
  checkButtons();
}

