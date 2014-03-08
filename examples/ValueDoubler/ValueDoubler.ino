/*
    ValueDoubler
        
    This example demonstrates how to sync both incomming and outgoing variables.
    Four variables are kept in sync; two are outgoing variables and two are incoming variables.
    The outgoing variables are always set to the double of the incoming variables.
    This way you can abuse your Arduino as a simple "times-two-calculator".
   
    Author: Maximilian Ernestus
   
*/

#include <VSync.h>

//  Create a new sender here
//  Put the number of variables you want to sync in the pointy brackets
ValueSender<2> sender;

//  Create a new receiver here
//  It can receive up to two values
ValueReceiver<2> receiver;

//  valueA and valueB are the incoming values
int valueA, valueB;

//  doubleA and doubleB are the outgoing variables
int doubleA, doubleB;

void setup()
{
  //  You need to call Serial.begin() in order for value syncing to work.
  //  Make sure to use the same baudrate on both ends. More baud = more speed
  Serial.begin(19200);

  //  Tell the sender what variables to observe and keep in sync.
  //  Be sure to keep the order of variables the same on the receiving and the sending end!
  sender.observe(doubleA);
  sender.observe(doubleB);

  //  Tell the receiver what values to sync
  //  Here you need to take care of the order as well!
  receiver.observe(valueA);
  receiver.observe(valueB);
}

void loop()
{
  //  for the receiver and for the sender you must call sync() once in every loop.
  //  It does not matter where in the loop you call it, 
  //  but receiving at the start and sending at the end makes most sense.
  receiver.sync();
  doubleA = valueA * 2;
  doubleB = valueB * 2;
  sender.sync();
}


