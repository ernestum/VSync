/*
    LatencyTester
 
    This example is just a tool to easily test the latency in your particular setup.
    The latency is tested with two synchronized values in both directions at 19200 baud.
    The values randomly wander around the initial value which is 0. If you increase the
    initial value the latency will get worse (because more digits).
    
    Use this sketch with an Arduino, that is loaded with the ValueDouble example of the
    Arduino version of the VSync library.
 
    Author: Maximilian Ernestus
 
 */
 
import vsync.*;
import processing.serial.*;

ValueReceiver receiver;
ValueSender sender;


public int valueA = 0, valueB = 0;
public int doubleA, doubleB;

public int doubleMouseX, doubleMouseY;

void setup() {
  size(10, 10);
  Serial serial = new Serial(this, "/dev/ttyUSB0", 19200);

  receiver = new ValueReceiver(this, serial).observe("doubleA").observe("doubleB");
  sender = new ValueSender(this, serial).observe("valueA").observe("valueB");
  
  //  Use a high frame rate to eliminate any artificial delays
  //  Warning: some graphics cards will just stall at 60fps which will badly influence
  //  the measurements!
  frameRate(3000);
}

boolean waiting = false;
long timeSent = 0;

//  For computing the average
long sum;
long num;

void draw() 
{
  if (waiting)
  {
    if (doubleA == valueA*2 && doubleB == valueB*2)
    {
      long roundtrip = millis() - timeSent;
      sum += roundtrip;
      num++;
      println("Roundtrip: " + roundtrip + "ms\tavg: " + (sum/(float)num) + "ms"); 

      waiting = false;
    }
    
    if(millis() - timeSent > 1000)
      waiting = false;
  }

  if (!waiting)
  {
    //  Make valueA and valueB wander randomly around the initial values
    valueA += random(100) > 50 ? 1 : -1;
    valueB += random(100) > 50 ? 1 : -1;
    doubleA = 0;
    doubleB = 0;
    waiting = true;
    timeSent = millis();
  }
}

