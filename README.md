VSync Library
=========

This is the VSync library for the Arduino platform. It can 'magically' synchronize variables on your Arduino with variables on other Arduinos or in your Processing sketch.
Look [here](https://github.com/erniejunior/VSync-for-Processing) for it's Processing counterpart.


Quick start
-----------
1. Upload the SimpleSender example to your Arduino.
2. Connect something interesting to Analog Pin A0.
3. Open the SimpleReceiver example in Processing.
4. Change the serial port at the head of the setup() to match your Arduino.
5. Hit Run (ctrl+r).


Use cases
---------
For example if you read the heading off a digital compass and store it in a variable
on your Arduino, you can sync that variable with a Processing sketch. In the Processing
sketch you can use it to visualize the digital compass reading.

Maybe you have many settings (color of a RGB LED; angle of a servo motor; parameters
for a PID algorithm ...), that often need to be changed during the runtime of your Arduino.
Simply synchronize the position of a sliders in your Processing sketch
with the Arduino to transfer transfer a parameter.


Concepts
--------
The idea of this library is to make variables in one sketch (either Arduino or Processing) available in a different sketch. A variable is always synchronized from one side to another side. 
Let's assume we have a variable called `servoAngle` on our Arduino [A] and want to use it in our Processing sketch [P]. To synchronize the variable from [A] to [P] we need a ValueSender on the [A] side and a ValueReceiver on the [P] side. 

[A] : ValueSender  -------------~servoAngle~------------->  [P] : ValueReceiver

Now every time the value of `servoAngle` changes on [A], that change will propagate to [P]. This is a one-way-thing: changing `servoAngle` on the [P] side will not induce any changes at [A]. As soon as [A] sends an update on `servoAngle` the change is overridden.
<sub>(You could use another ValueSender on [P] and a ValueReceiver on [A] to synchronize in the other direction, but VSync is neither built nor tested for that use case. Bad things might happen, if the variable is changed on both sides at the same time!)</sub>

These principles apply to all possible synchronization directions:
* Arduino ---> Processing
* Arduino <--- Processing
* Arduino ---> Arduino
* Arduino <--- Arduino


Usage
-----
Let's go back to the example in 'Concepts' where we synchronize `servoAngle` from an Arduino to a Processing sketch.

### On the Arduino side

First you would need to make yourself a new ValueSender object like this:
```C++
ValueSender<1> sender;
```
You are probably confused by the pointy brackets with the 1 in between. That is just a special notation to tell the ValueSender how many variables you want to synchronize.

After you created the ValueSender object you need to tell it what variable you want to synchronize and observe for changes. For this you need to call it's `observe()` function in your `setup()`:
```C++
sender.observe(servoAngle);
```

Finally you need to make the ValueSender do it's actual synchronization by calling its `sync()` function somewhere in your `loop()`:
```C++
sender.sync();
```

The complete sketch would then look like this:
```C++
#include <VSync.h>

ValueSender<1> sender;
int servoAngle;

void setup()
{
  Serial.begin(19200);
  sender.observe(servoAngle);
}

void loop()
{
  ... your code; possibly changing servoAngle ...

  sender.sync();
}
```

### On the Processing side

After everything is set up on the Arduino we take care of the receiving end in the Processing sketch.
Get the Processing library [here](https://github.com/erniejunior/VSync-for-Processing).

Because Processing is not tightly integrated with serial communication and because there might be more than one Serial interface on your computer we first need to configure a serial object using the serial library:
```Java
Serial serial = new Serial(this, "<SERIAL_PORT_NAME_HERE>", 19200);
```
The easiest way to figure out the right name for the serial port is to have a look in your Arduino IDE and see what port is checked under Tools->Serial Port when you upload code to your Arduino.
On Linux it often starts with /dev/ttyUSB oder /dev/ttyACM.

After the serial object is set up we can proceed to create the ValueReceiver object like so:
```Java
ValueReceiver receiver = new ValueReceiver(this, serial);
```
This looks a bit different than on the Arduino side because the Processing syntax is slightly different and because we need to somehow pass the serial object to the receiver.

After that everything is quite similar to the Arduino code. We tell the ValueReceiver what variable to observe and synchronize with the `observe()` function `receiver.observe("servoAngle")` (note the quotes around the variable name; another special Processing thing) and we are done because the call to `sync()` is done automagically for you by Processing.

The complete Processing sketch would look like this:
```Java
import processing.serial.*;
import vsync.*;

public int servoAngle;

void setup() 
{
  size(400, 400);

  Serial serial = new Serial(this, "<SERIAL_PORT_NAME_HERE>", 19200);
  ValueReceiver receiver = new ValueReceiver(this, serial);
  receiver.observe("servoAngle");
}

void draw() 
{
  ... your drawing code, possibly using servoAngle ...
}

```

Tip for debugging
-----------------

To make two Arduinos that are connected to your Linux machine via USB talk to each other just type:

`dd if=/dev/tty<ALICE> bs=1 of=/dev/tty<BOB> bs=1`

(Adapt the <ALICE> and <BOB> part according to the addresses on your computer. Alice is the sending arduino while Bob is the receiving end)

If this does not work right away try `screen /dev/tty<ALICE>` and exit it with ctrl+a, k, y. Then try the above line again.



Technical details
-----------------

(boring, don't read if you just want to use the library)
    
VSync uses Serial to synchronize int variables, which means that it will work through your USB cable as well as with your XBee. 
It also means it can only do integers and will block any other (e.g. debugging) uses of the Serial interface.
    
The library uses a very simple human readable protocol that is optimized for minimal data traffic (e.g. only when a variable changes it is sent over the line). The protocol is package based with two kinds of packages:
  * Full-Sync-Package: contains the values of all variables
  * Diff-Sync-Package: contains the values only of variables that changed
  
The Full-Sync-Package looks like this: A<val1>|<val1>|<val3># if you are syncing three variables. E.g if the variables have the values 4, -8, 555 the package would look like:
```
A|4|-8|555#
```
The A at the beginning stands for 'all values', the # is the end-character.
The Diff-Sync-Package looks like this: 0|4|2|555# if value 0 and value 2
changed to 4 and 555. If only few (1 or 2) variables changed, a Diff-Sync-Package is sent; if lots of variables changed it is more efficient to send a Full-Sync-Package, because the id's of the variables do not need to be sent.
