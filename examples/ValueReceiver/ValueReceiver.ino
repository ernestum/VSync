#include <ValueSync.h>

int led = 13;
int ledState = 1;

//Create a new receiver here
//Put the number of variables you want to sync in the pointy brackets
ValueReceiver<1> receiver;

void setup()
{
  pinMode(led, OUTPUT);
  Serial.begin(9600);
  
  //Tell the receiver what variables you whish to sync
  //If you sync more than one variable 
  //the order in which you call addValue must be the same on the sending and the receiving side
  receiver.addValue(&ledState);
}

void loop()
{
  //receives one value at a time. Call this often enough so you don't miss any values!
  receiver.receiveValue();
  
  digitalWrite(led, ledState);
}
