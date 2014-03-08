/*
    SimpleReceiver
        
    This example shows how to receive two values (mouseX and mouseY) from a
    Processing sketch.
    In this example the two values are used to control the brightness of two
    LEDs attached to pin 5 and 6 (see http://arduino.cc/en/Tutorial/Fade for
    a circuit), but you could use them for anything you want (e.g. to control
    a servo or the color of an RGB LED).
 
    This file should be used together with the SimpleSender example of the
    Processing version of the VSync Library 
    ( https://github.com/erniejunior/Processing-VSync )
    
    Author: Maximilian Ernestus
   
*/

#include <VSync.h>

//  Create a new receiver that can receive up to two values.
//  Always put the number of values to sync in the pointy brackets.
ValueReceiver<2> receiver;

//  The two variables that will be synced with the Processing sketch.
int mouseX, mouseY;

void setup()
{
  //  You need to call Serial.begin() in order for value syncing to work.
  //  Make sure to use the same baudrate on both ends. More baud = more speed
  Serial.begin(19200);


  //  Tell the receiver what variables to sync. It must be int variables!
  //  Also take care that you observe the variables in the same order
  //  as on the sending (Processing) side!
  receiver.observe(mouseX);
  receiver.observe(mouseY);
}

void loop()
{
  //  You need to call sync() once every loop.
  //  It does not matter where in the loop you call it, 
  //  but receiving at the start makes most sense.
  receiver.sync();
  
  //  Here we use the variables to control some LEDs.
  analogWrite(5, mouseX);
  analogWrite(6, mouseY);
}



