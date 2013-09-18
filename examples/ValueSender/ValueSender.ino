#include <ValueSync.h>

int led = 13;
int ledState = 0;

//Create a new sender here
//Put the number of variables you want to sync in the pointy brackets
ValueSender<1> sender;

void setup()
{
  pinMode(led, OUTPUT);
  Serial.begin(9600);
  
  //Tell the sender what variables you whish to sync
  //If you sync more than one variable 
  //the order in which you call addValue must be the same on the sending and the receiving side
  sender.addValue(&ledState);
}

int choice = 0;

void loop()
{
  //Update my LED
  ledState = !ledState;
  digitalWrite(led, ledState);
  
  //Sync my LED state
  switch(choice)
  {
    case 0: //BEST CHOICE HERE!
      //Will send over all the values that have changed since the last call to syncValues()
      //Use this all the time! Its the easiest!
      sender.syncValues();
      break;
    case 1:
      //Fill send over the first value (we start to count at 0) that has been added via addValue()
      sender.sendValue(0);
      break;
    case 2:
      //Sends the value of a variable that has been added via addValue
      //Be carefull! This can be slow if you added many values!!!
      sender.sendValue(&ledState);
      break;
    case 3:
      //Sends over all the values that have been added in a compressed package
      sender.sendAllValues();
      break;
  }

  delay(1000);
}
