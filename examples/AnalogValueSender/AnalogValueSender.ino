#include <ValueSync.h>

/*
    AnalogValueSender

    This example demonstrates how to keep a value from an 
    analog pin in sync with a processing sketch.
    It can be used together with the AnalogValueReceiver example of the 
    Processing version of this library.

    Author: Maximilian Ernestus
    
*/

//  Create a new sender here
//  Put the number of variables you want to sync in the pointy brackets
//  Here we just want to sync one value
ValueSender<1> sender;

//  here we store the value we read from analog bin A0
int analogValue;

void setup()
{
  //  You need to call Serial.begin() in order for value syncing to work.
  //  Make sure to use the same baudrate on both ends. More baud = more speed
  Serial.begin(19200);

  //  Tell the sending syncronizer what value needs to be kept in sync.
  //  In this case it is just the variable 'analogValue' but if you sync more,
  //  you need to make sure to use the same order on the sending and the receiving side.
  sender.observe(analogValue);
}

void loop()
{
  // Read a value from analog pin A0. You could hook up a LDR or a potmeter.
  analogValue = analogRead(A0);
  
  //  Keep the outgoing value in sync with the processing sketch.
  //  You need to call syncValues() for the ValueSender and for the ValueReceiver once in every loop.
  //  It does not matter where you call it but it makes sense 
  //  to call the ValueReceiver at the top and the value Sender at the bottom.
  sender.syncValues();
}




