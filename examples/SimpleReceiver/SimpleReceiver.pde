/*
    SimpleReceiver
 
    This example shows how to synchronize a value from an Arduino to a 
    Processing sketch. The value is used to control the brightness of 
    the  background.
 
    Use it together with the SimpleSender example of the Arduino version
    of the VSync library.
    ( https://github.com/erniejunior/VSync )
 
    Author: Maximilian Ernestus
 
 */

//  You always need to import the serial library in addition to the VSync library
import processing.serial.*;
import vsync.*;

//  We create a new ValueReceiver to receive values from the arduino
ValueReceiver receiver;

//  This is the variable we want to sync from the Arduino to this sketch
public int analogValue;

void setup() 
{
  size(400, 400);

  //  Hint: "/dev/ttyUSB0" is the serial port on my system. It might have
  //  a different name on yours. 
  //  It should be the same one that is checked under Tools->Serial Port in you Arduino IDE
  //  when uploading Arduino sketches.
  //  Look at http://processing.org/reference/libraries/serial/Serial.html if you still have trouble.
  Serial serial = new Serial(this, "/dev/ttyUSB0", 19200);

  //  Ininialize the ValueReceiver with this (to hook it to your sketch)
  //  and the serial interface you want to use.
  receiver = new ValueReceiver(this, serial);

  // Tell the ValueReceiver what variable you want to synchronize from the arduino to this sketch.
  receiver.observe("analogValue");
}

void draw() 
{
  //  Draw the background using the variable that was synced from the Arduino to this sketch
  background(analogValue/4);
}

