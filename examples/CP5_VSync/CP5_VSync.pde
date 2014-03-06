/*
    CP5_VSync
        
    This example shows how to synchronize the position of two ControlP5
    sliders to an Arduino.
    It should be used together with the SimpleReceiver example of the Arduino
    version of the VSync Library
    ( https://github.com/erniejunior/VSync )
    
    Author: Maximilian Ernestus
   
*/

import controlP5.*;
import vsync.*;
import processing.serial.*;

ValueSender sender;

//  Annotate the synced variables for ControlP5
@ControlElement (x=10, y=10, properties = {"max=255"})
public int led1;

@ControlElement (x=10, y=40, properties = {"max=255"})
public int led2;

void setup() 
{
  size(150,60);

  Serial serial = new Serial(this, "/dev/ttyUSB0", 19200);
  
  sender = new ValueSender(this, serial);
  sender.observe("led1");
  sender.observe("led2");
  
  //  Create controllers for our two variables.
  ControlP5 cp5 = new ControlP5(this);
  cp5.addControllersFor(this);
}

void draw() 
{
  //  CP5 looks better with a darker background.
  background(80);
}
