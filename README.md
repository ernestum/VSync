VSync Library
=========

This is the VSync library for the Arduino platform. It can 'magically' synchronize variables on your Arduino with variables on other Arduinos or in your Processing sketch.

Use cases:
----------
For example if you read the heading off a digital compass and store it in a variable
on your Arduino, you can sync that variable with a Processing sketch. In the Processing
sketch you can use it to visualize the digital compass reading.

Maybe you have many settings (color of a RGB LED; angle of a servo motor; parameters
for a PID algorithm ...), that often need to be changed during the runtime of your Arduino.
Simply synchronize the position of a sliders in your Processing sketch
with the Arduino to transfer transfer a parameter.


Concepts:
---------
The idea of this library is to make variables in one sketch (either Arduino or Processing) available in a different sketch. A variable is always synchronized from one side to another side. 
Let's assume we have a variable called `servoAngle` on our Arduino [A] and want to use it in our Processing sketch [P]. To synchronize the variable from [A] to [P] we need a ValueSender on the [A] side and a ValueReceiver on the [P] side. 

[A] : ValueSender  -------------~servoAngle~------------->  [P] : ValueReceiver

Now every time the value of `servoAngle` changes on [A], that change will propagate to [P]. This is a one-way-thing: changing `servoAngle` on the [P] side will not induce any changes at [A] and as soon as [A] sends an update on `servoAngle` the change is overridden.
You could use another ValueSender on [P] and a ValueReceiver on [A] to synchronize in the other direction, but VSync is neither built nor tested for that use case. Bad things might happen, if the variable is changed on both sides at the same time!

These principles apply to all possible synchronization directions:
Arduino ---> Processing
Arduino <--- Processing
Arduino ---> Arduino
Arduino <--- Arduino


Usage:
------



Tip for debugging:
------------------

To make two Arduinos that are connected to you linux machine via USB talk to each other just type:

`dd if=/dev/tty<ALICE> bs=1 of=/dev/tty<BOB> bs=1`

(Adapt the <ALICE> and <BOB> part according to the addresses on your computer. Alice is the sending arduino while Bob is the receiving end)

If this does not work right away try `screen /dev/tty<ALICE>` and exit it with ctrl+a, k, y. Then try the above line again.



Technical details:
------------------

(boring, don't read if you just want to use the library)
    
VSync uses Serial to synchronize int variables, which means that it will work through your
USB cable as well as with your XBee. 
It also means it can only do integers and will block any other (e.g. debugging) uses of
the Serial interface.
    
The library uses a very simple human readable protocol that is optimized for minimal 
data traffic (e.g. only when a value changes it is sent over the line).
The protocol is package based with two kinds of packages:
  * Full-Sync-Package: contains the values of all variables
  * Diff-Sync-Package: contains the values only of variables that changed
  
The Full-Sync-Package looks like this: A<val1>|<val1>|<val3># if you are syncing three
variables. E.g if the variables have the values 4, -8, 555 the package looks like:

`A|4|-8|555#`

The A at the beginning stands for all values, the # is the end-character.
The Diff-Sync-Package looks like this: 0|4|2|555# if the first and the last value
changed to 4 and 555. If only few (1 or 2) variables changed a Diff-Sync-Package is sent;
if lots of variables changed it is more efficient to send a Full-Sync-Package, because
the id's of the variables do not need to be sent.
