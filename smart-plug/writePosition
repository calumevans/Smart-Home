#include <EEPROM.h> 

int address = 0;

void setup(){
  Serial.begin(9600);
}

void loop(){
  EEPROM.write(address, 1);         //change this to 1,2,3 (for each light)
  while(1){
    Serial.println("done");
  }
}
