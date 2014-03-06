/*
    BidirectionalSync
 
    If you are just getting started with the VSync library, the SimpleSender
    and SimpleReceiver examples are recommended first.
    
    This example shows how to synchronize variables in two directions:
    from the Arduino to this sketch and from this sketch to the Arduino.
    The mouseX and mouseY variables are synchronized from this sketch to
    the Arduino. The variables doubleMouseX and doubleMouseY are synchronized
    from the Arduino to this sketch. 
    As the name already suggests, the Arduino always sets the two outgoing 
    variables to the double of the two incoming variables. 
    This way the arduino is used by this sketch to compute the double of two values.
    
    To demonstrate how fast this is a white circle is drawn at the position, that is
    "double" the mouse position. On my old Duemilanove there is no visible lag.
    If you look at the TX/RX LEDs on your arduino you can observe how the only light
    up if you move your mouse because only when variables change they need to be synced.
    
    Use this sketch together with the ValueDoubler example of the Arduino version of the 
    VSync library ( https://github.com/erniejunior/VSync )
 
    Author: Maximilian Ernestus
 
 */


import vsync.*;
import processing.serial.*;

//  Create a sender and a receiver for incoming and outgoing synchronization.
ValueReceiver receiver;
ValueSender sender;

//  This are the variables to be synced from the arduino into this sketch
public int doubleMouseX, doubleMouseY;

void setup() {
  size(600,600);
  smooth();
  
  //  Create a serial object; the port might have a different name on your machine
  Serial serial = new Serial(this, "/dev/ttyUSB0", 19200);
  
  //  Here we initialize the sender and the receiver while defining what values to sync
  //  all in one line! This is a shortcut notation for convenience
  receiver = new ValueReceiver(this, serial).observe("doubleMouseX").observe("doubleMouseY");
  sender = new ValueSender(this, serial).observe("mouseX").observe("mouseY");
}

void draw() 
{
  //  Painting setup
  noStroke();
  ellipseMode(CENTER);
  
  //  Draw the background
  fill(0, 5);
  rect(0, 0, width, height);
  
  
  fill(255);
  
  //  Draw a circle at the position calculated by the arduino
  ellipse(doubleMouseX, doubleMouseY, 10, 10);
  
  //  Draw a circle at the actual mouse position (for comparision)
  ellipse(mouseX, mouseY, 10, 10);
}
